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

#include "qtconfiguration.h"

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
