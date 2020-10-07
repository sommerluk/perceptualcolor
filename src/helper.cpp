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

/** @file
 * 
 * Definition of @ref PerceptualColor::Helper namespace and its members. */

// Own header
#include "PerceptualColor/helper.h"

#include <QPainter>

#include <math.h>

namespace PerceptualColor {

namespace Helper {

    /** @brief Number of vertical <em>standard</em> wheel steps done by a
     *  wheel event
     * 
     * As the QWheelEvent documentation explains, there is a common physical
     * standard wheel step size for mouse wheels: 15°. But there are some
     * mouse models which use non-standard physical wheel step sizes for
     * their mouse wheel, for example because they have a higher wheel
     * resolution.
     * 
     * This function converts the values in a QMouseEvent to the
     * <em>standard</em> wheel step count.
     * 
     * @param event the QWheelEvent
     * @returns the count of vertical <em>standard</em> wheel steps done
     * within this mouse event. The value is positive for up-steps and
     * negative for down-steps. On a standard mouse wheel, moving the wheel
     * one physical step up will return the value 1. On a non-standard,
     * higher resolution mouse wheel, moving the wheel one physical step up
     * will return a smaller value, for example 0.7 */
    qreal standardWheelSteps(QWheelEvent *event)
    {
        // QWheelEvent::angleDelta() returns 8 units for each degree.
        // The standard wheel step is 15°. So on a standard
        // mouse, one wheel step results in (8 × 15) units.
        return event->angleDelta().y() / static_cast<qreal>(8 * 15);
    }
}

}
