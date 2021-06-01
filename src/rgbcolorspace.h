// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2020 Lukas Sommer sommerluk@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RGBCOLORSPACE_H
#define RGBCOLORSPACE_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include <QObject>

#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/lchadouble.h"
#include "PerceptualColor/lchdouble.h"

#include <lcms2.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief Access to LittleCMS color management
 *
 * @todo Do not pimpl anymore?
 *
 * @todo (Optionally) use the fast-float plug-in for faster caluculation?
 *       Is this possible as a run-time choise?
 *
 * @todo We return double precision values. But doesn’t use LittleCMS
 *       only 16-bit-integer internally? On the other hand: Using double
 *       precision allows to filter out out-of-range values…
 *
 * @todo Declare Q_PROPERTY for @ref profileInfoCopyright(),
 *       @ref profileInfoDescription(), @ref profileInfoManufacturer(),
 *       @ref profileInfoModel() */
class RgbColorSpace : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE RgbColorSpace(QObject *parent = nullptr);
    virtual ~RgbColorSpace() noexcept override;
    Q_INVOKABLE bool isInGamut(const cmsCIELab &lab) const;
    Q_INVOKABLE bool isInGamut(const PerceptualColor::LchDouble &lch) const;
    Q_INVOKABLE int maximumChroma() const;
    Q_INVOKABLE PerceptualColor::LchDouble nearestInGamutColorByAdjustingChroma(const PerceptualColor::LchDouble &color) const;
    Q_INVOKABLE PerceptualColor::LchDouble nearestInGamutColorByAdjustingChromaLightness(const PerceptualColor::LchDouble &color);
    Q_INVOKABLE QString profileInfoCopyright() const;
    Q_INVOKABLE QString profileInfoDescription() const;
    Q_INVOKABLE QString profileInfoManufacturer() const;
    Q_INVOKABLE QString profileInfoModel() const;
    Q_INVOKABLE PerceptualColor::LchDouble toLch(const cmsCIELab &lab) const;
    Q_INVOKABLE PerceptualColor::LchDouble toLch(const QColor &rgbColor) const;
    Q_INVOKABLE QColor toQColorRgbBound(const PerceptualColor::LchDouble &lch) const;
    Q_INVOKABLE QColor toQColorRgbBound(const PerceptualColor::LchaDouble &lcha) const;
    Q_INVOKABLE QColor toQColorRgbUnbound(const cmsCIELab &Lab) const;                  // TODO Isn’t QColor _always_ bound???
    Q_INVOKABLE QColor toQColorRgbUnbound(const PerceptualColor::LchDouble &lch) const; // TODO Isn’t QColor _always_ bound???

private:
    Q_DISABLE_COPY(RgbColorSpace)

    class RgbColorSpacePrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class RgbColorSpacePrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<RgbColorSpacePrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestRgbColorSpace;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACE_H
