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

#ifndef MULTICOLOR_H
#define MULTICOLOR_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include "rgbcolorspace.h"

#include <QColor>
#include <QSharedPointer>

namespace PerceptualColor
{
/** @internal
 *
 * @brief A color stored in multiple color models simultaneously.
 *
 * This data type has been developed for private usage within @ref ColorDialog.
 * It is just an implementation detail of @ref ColorDialog. It’s also not
 * declared to Qt’s type system because the private implementation of
 * @ref ColorDialog does not require it.
 *
 * This data type is different to <tt>QColor</tt>. QColor stored the value in
 * exactly one representation, and generates different other represensations
 * (HSV, CMYK…) on the fly. @ref MultiColor however actually <em>stores</em>
 * all available representations. This makes sure there are no rounding
 * errors.
 *
 * This data type can be passed to QDebug thanks to
 * operator<<(QDebug dbg, const PerceptualColor::MultiColor &value)
 *
 * @todo Conversion HSV-RGB with Qt’s QColor: Can we get a higher precison
 * with our own implementation? */
class MultiColor final
{
public:
    MultiColor();
    static MultiColor fromLch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color);
    static MultiColor fromRgbQColor(const QSharedPointer<RgbColorSpace> &colorSpace, const QColor &color);

    /** @brief Default copy constructor
     * @param other the object to copy */
    MultiColor(const MultiColor &other) = default;
    /** @brief Default move constructor
     * @param other the object to move */
    MultiColor(MultiColor &&other) noexcept = default;
    /** @brief Default copy assignment operator
     * @param other the object to copy */
    MultiColor &operator=(const MultiColor &other) = default;
    /** @brief Default move assignment operator
     * @param other the object to move */
    MultiColor &operator=(MultiColor &&other) noexcept = default;

    bool operator==(const MultiColor &other) const;

    QList<double> toHlc() const;
    LchDouble toLch() const;
    QColor toRgbQColor() const;

private:
    /** LCh representation. */
    LchDouble m_lch;
    /** RGB representation within a QColor object */
    QColor m_rgbQColor;

    void normalizeLch();
};

QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value);

} // namespace PerceptualColor

#endif // MULTICOLOR_H
