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

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Own header
#include "rgbdouble.h"

#include <type_traits>

#include <lcms2.h>

/** @internal
 *
 * @file
 *
 * This file defines some static asserts for the data type
 * @ref PerceptualColor::RgbDouble. */

namespace PerceptualColor
{
/** @internal
 * @brief Adds QDebug() support for this data type.
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::RgbDouble &value)
{
    dbg.nospace() << "RgbDouble(" << value.red << " " << value.green << " " << value.blue << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::red)>);

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::green)>);

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::blue)>);

static_assert(sizeof(double) == sizeof(cmsFloat64Number));

// Assert that RgbDouble also works fine as buffer.
static_assert(sizeof(RgbDouble) == sizeof(cmsFloat64Number[3]));

static_assert(std::is_trivial_v<RgbDouble>);

static_assert(std::is_standard_layout_v<RgbDouble>);

} // namespace PerceptualColor
