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

#ifndef VERSION_H
#define VERSION_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QVersionNumber>
#include <QtGlobal>

/** @file
 *
 * This file provides version information for this library at compile
 * time and at run time.
 *
 * @note Do not include this file itself! Instead, include <tt>version.h</tt>.
 *
 * @internal
 *
 * @note This file is configuration input: CMake processes it,
 * puts the actual version numbers (<tt>\@…\@</tt>) and saves the result
 * as <tt>version.h</tt>.
 *
 * @note This file has the extension .hpp because we want a different
 * extension for configuration input than for headers. This allows to
 * <em>not</em> process this file by clang-format. (clang-format does not
 * understand the CMake configuration syntax with <tt>\@…\@</tt> and would
 * break it.) */

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR
 *
 * @returns The <em>major</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR (@PROJECT_MAJOR_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR
 *
 * @returns The <em>minor</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR (@PROJECT_MINOR_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH
 *
 * @returns The <em>patch</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH (@PROJECT_PATCH_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 *
 * @brief Against which version of this library you are <em>compiling</em>.
 *
 * This macro has the same semantic as <tt>QT_VERSION</tt>
 *
 * @returns This macro expands a numeric value of the form 0xMMNNPP
 * (MM = major, NN = minor, PP = patch) that specifies the version number.
 * Note that there is no pre-release identifier included. For example, if
 * you compile your application against version 1.2.3-alpha of this library,
 * the macro will expand to 0x010203. You can use this macro to use the
 * latest features where available. Example:
 *
 * @snippet test/testversion.cpp Version Macro usage
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION (QT_VERSION_CHECK(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH))

namespace PerceptualColor
{

PERCEPTUALCOLOR_IMPORTEXPORT QVersionNumber perceptualColorRunTimeVersion();

} // namespace PerceptualColor

#endif // VERSION_H
