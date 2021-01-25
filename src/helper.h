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

#ifndef HELPER_H
#define HELPER_H

#include <QDebug> // TODO Remove me!
#include <QImage>
#include <QtGlobal>
#include <QWheelEvent>

#include "PerceptualColor/lchdouble.h"

// Test if the compiler treats the source code actually as UTF8.
// A test string is converted to UTF8 code units (u8"") and each
// code unit is checked to be correct.
static_assert(
    (static_cast<quint8>(*((u8"🖌")+0)) == 0xF0)
        && (static_cast<quint8>(*((u8"🖌")+1)) == 0x9F)
        && (static_cast<quint8>(*((u8"🖌")+2)) == 0x96)
        && (static_cast<quint8>(*((u8"🖌")+3)) == 0x8C)
        && (static_cast<quint8>(*((u8"🖌")+4)) == 0x00),
    "This source code has to be read-in as UTF8 by the compiler."
);

namespace PerceptualColor {
    
/** @file
 * 
 * @brief Various smaller help elements.
 * 
 * This file groups together various smaller elements that are used
 * across the library but do not belong strictly to one of the classes.
 * 
 * @todo Decide for each member of this namespace if it can be moved into
 * a class because it’s only used in this single class. */

/** @brief Precision for gamut boundary search
 * 
 * We have to search sometimes for the gamut boundary. This value defines
 * the precision of the search:  Smaller values mean better precision and
 * slower processing. */
constexpr qreal gamutPrecision = 0.001;

/** @brief Template function to test if a value is within a certain range
 * @param low the lower limit
 * @param x the value that will be tested
 * @param high the higher limit
 * @returns @snippet this Helper inRange */
template<typename T> bool inRange(const T& low, const T& x, const T& high)
{
    return (
        //! [Helper inRange]
        (low <= x) && (x <= high)
        //! [Helper inRange]
    );
}

qreal standardWheelSteps(QWheelEvent *event);

}

#endif // HELPER_H
