// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2020 Lukas Sommer somerluk@gmail.com
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

#ifndef SIMPLECOLORWHEEL_P_H
#define SIMPLECOLORWHEEL_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/simplecolorwheel.h"
#include "constpropagatingrawpointer.h"

#include <QPointer>

#include "PerceptualColor/polarpointf.h"

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class SimpleColorWheel::SimpleColorWheelPrivate final
{
public:
    SimpleColorWheelPrivate(SimpleColorWheel *backLink);
    /** @brief Default destructor
     * 
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~SimpleColorWheelPrivate() noexcept = default;

    /** @brief If a mouse event is active
     * 
     * Holds if currently a mouse event is active or not.
     * @sa mousePressEvent()
     * @sa mouseMoveEvent()
     * @sa mouseReleaseEvent()
     */ 
    bool m_mouseEventActive;
    /** @brief A cache for the wheel picture as QImage. Might be outdated!
     *  @sa updateWheelImage() 
     *  @sa m_wheelImageReady() */
    QImage m_wheelImage;
    /** Holds whether or not m_wheelImage is up-to-date.
     *  @sa refreshWheelImage()
     *  @sa updateWheelImage
     * @todo It might be better to erase m_wheelImageReady when it gets
     * invalid (and get rid of m_wheelImageReady). So memory can be
     * freed more quickly, without having outdated data taking space
     * on the heap. */
    bool m_wheelImageReady = false;
    /** @brief Internal storage of the hue() property */
    qreal m_hue;
    /** @brief Pointer to RgbColorSpace() object */
    QPointer<RgbColorSpace> m_rgbColorSpace;

    QPointF fromWheelCoordinatesToWidgetCoordinates(
        const PolarPointF wheelCoordinates
    ) const;
    PolarPointF fromWidgetCoordinatesToWheelCoordinates(
        const QPoint widgetCoordinates
    ) const;
    void updateWheelImage();

private:
    Q_DISABLE_COPY(SimpleColorWheelPrivate)
    
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<SimpleColorWheel> q_pointer;
};

}

#endif // SIMPLECOLORWHEEL_P_H
