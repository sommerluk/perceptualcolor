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

#include "perceptualinternal.h"

// Own header
#include "PerceptualColor/lchdouble.h"

#include <type_traits>

#include <lcms2.h>

/** @file
 *
 * This file defines some static asserts for the data type
 * @ref PerceptualColor::LchDouble. */

namespace PerceptualColor
{
// We are using double. Check that we stay compatible with cmsCIELCh
// which is based on cmsFloat64Number.
static_assert(std::is_same_v<cmsFloat64Number, double>);

static_assert(sizeof(double) == sizeof(cmsFloat64Number));

static_assert(sizeof(LchDouble) == sizeof(cmsCIELCh));

static_assert(std::is_trivial_v<LchDouble>);

static_assert(std::is_standard_layout_v<LchDouble>);

/** @brief Adds QDebug() support for this data type.
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in
 * @todo This is originally declared in the global namespace instead of the
 * @ref PerceptualColor namespace, because the supported value was a
 * <tt>typedef</tt> for a LittleCMS type in the global; when declaring
 * this function in @ref PerceptualColor namespace, it did not work
 * in the global namespace. Now, things have changed. But we should write
 * a unit test for if it works in global namespace! */
QDebug operator<<(QDebug dbg, const PerceptualColor::LchDouble &value)
{
    dbg.nospace() << "LchDouble(" << value.l << "% " << value.c << " " << value.h << "°)";
    return dbg.maybeSpace();
}

/** @brief Compares coordinates with another object.
 *
 * @param other The object to compare with
 * @returns <tt>true</tt> if all three coordinates @ref l, @ref c and @ref h
 * of <em>this</em> object are all equal to the coordinates of <em>other</em>.
 * <tt>false</tt> otherwise. Note that two objects with equal @ref l and
 * equal @ref c, but one with h = 5° and the other with h = 365°, are
 * considered non-equal thought both describe the same point in the
 * coordinate space. */
bool LchDouble::hasSameCoordinates(const PerceptualColor::LchDouble &other) const
{
    return ((l == other.l) && (c == other.c) && (h == other.h));
}

} // namespace PerceptualColor
