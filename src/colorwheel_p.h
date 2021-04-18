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

#ifndef COLORWHEEL_P_H
#define COLORWHEEL_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/colorwheel.h"

#include <QPointer>

#include "colorwheelimage.h"
#include "constpropagatingrawpointer.h"
#include "polarpointf.h"

namespace PerceptualColor
{
/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ColorWheel::ColorWheelPrivate final
{
public:
    ColorWheelPrivate(ColorWheel *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ColorWheelPrivate() noexcept = default;

    /** @brief If a mouse event is active
     *
     * Holds if currently a mouse event is active or not.
     * @sa mousePressEvent()
     * @sa mouseMoveEvent()
     * @sa mouseReleaseEvent()
     */
    bool m_mouseEventActive;
    ColorWheelImage m_wheelImage;
    /** @brief Internal storage of the hue() property */
    qreal m_hue;
    /** @brief Pointer to RgbColorSpace() object */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;

    int border() const;
    int contentDiameter() const;
    QPointF fromWheelCoordinatesToWidgetCoordinates(const PolarPointF wheelCoordinates) const;
    PolarPointF fromWidgetCoordinatesToWheelCoordinates(const QPoint widgetCoordinates) const;
    qreal wheelRibbonChroma() const;

private:
    Q_DISABLE_COPY(ColorWheelPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorWheel> q_pointer;
};

} // namespace PerceptualColor

#endif // COLORWHEEL_P_H
