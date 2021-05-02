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

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "PerceptualColor/colorwheel.h"

#include "colorwheelimage.h"
#include "constpropagatingrawpointer.h"
#include "polarpointf.h"

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
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

    /** @brief Internal storage of the @ref hue() property */
    qreal m_hue;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a @ref mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref mouseMoveEvent() will move the diagram’s
     *   color handle.
     * - Once a @ref mouseReleaseEvent() occurs, the value is set to
     *   <tt>false</tt>. Further mouse movements will not move the handle
     *   anymore. */
    bool m_mouseEventActive = false;
    /** @brief Pointer to @ref RgbColorSpace object used to describe the
     * color space. */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;
    /** @brief The image of the wheel itself. */
    ColorWheelImage m_wheelImage;

    int border() const;
    int contentDiameter() const;
    QPointF fromWheelToWidgetCoordinates(const PolarPointF wheelCoordinates) const;
    PolarPointF fromWidgetToWheelCoordinates(const QPoint widgetCoordinatePoint) const;
    int innerDiameter() const;
    qreal wheelRibbonChroma() const;

private:
    Q_DISABLE_COPY(ColorWheelPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorWheel> q_pointer;
};

} // namespace PerceptualColor

#endif // COLORWHEEL_P_H
