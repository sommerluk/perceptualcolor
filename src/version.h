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

#include <QtGlobal>
#include <QVersionNumber>

#include "config.h"

/** @file
 *
 * This file provides version information for this library. */

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 *
 * @brief Against which version of this library you are <em>compiling</em>.
 *
 * This library uses
 * <a href="https://semver.org/">Semantic Versioning 2.0.0</a>.
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
 * To use this macro, include @ref version.h
 *
 * @sa @ref perceptualColorRunTimeVersion() */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION \
    ( \
        QT_VERSION_CHECK( \
            PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, \
            PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, \
            PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH \
        ) \
    )

namespace PerceptualColor {

QVersionNumber perceptualColorRunTimeVersion();

}

#endif // VERSION_H
