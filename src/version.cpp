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
#include "version.h"

static_assert(
    std
        ::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR)>
        ::is_integer
);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR >= 0);

static_assert(
    std
        ::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR)>
        ::is_integer
);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR >= 0);

static_assert(
    std
        ::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH)>
        ::is_integer
);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH >= 0);

static_assert(
    std
        ::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION)>
        ::is_integer
);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= 1);

namespace PerceptualColor {

/** @brief Against which version of this library you are <em>running</em>.
 *
 * @returns The library version with major, minor and patch version.
 * Note that there is no pre-release identifier included. For example, if
 * your application is running against version 1.2.3-alpha of this library,
 * this function will return <tt>QVersionNumber(1, 2, 3)</tt>.
 *
 * To use this function, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
QVersionNumber perceptualColorRunTimeVersion()
{
    return QVersionNumber(
        PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR,
        PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR,
        PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH
    );
}

} // namespace PerceptualColor
