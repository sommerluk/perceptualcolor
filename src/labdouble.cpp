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

#include "perceptualcolorlib_internal.h"

// Own header
#include "PerceptualColor/labdouble.h"

#include <QtGlobal>

#include <lcms2.h>

/** @file
 *
 * This file defines some static asserts for the data type
 * @ref PerceptualColor::LabDouble. */

namespace PerceptualColor {

static_assert(
    std::is_same_v<
        cmsFloat64Number,
        double
    >
);

static_assert(
    sizeof(double) == sizeof(cmsFloat64Number)
);


static_assert(
    std::is_same_v<
        LabDouble,
        cmsCIELab
    >
);

static_assert(
    sizeof(LabDouble) == sizeof(cmsCIELab)
);

static_assert(
    std::is_trivial_v<LabDouble>
);

static_assert(
    std::is_standard_layout_v<LabDouble>
);

} // namespace PerceptualColor

/** @brief Adds QDebug() support for this data type.
 * @note This is declared outside the global namespace instead of the
 * @ref PerceptualColor namespace, because the supported value is a
 * <tt>typedef</tt> for a LittleCMS type in the global; when declaring
 * this function in @ref PerceptualColor namespace, it would not work
 * in the global namespace. */
QDebug operator<<(QDebug dbg, const PerceptualColor::LabDouble &value)
{
    dbg.nospace()
        << "LabDouble("
        << value.L
        << "% "
        << value.a
        << " "
        << value.b
        << ")";
    return dbg.maybeSpace();
}
