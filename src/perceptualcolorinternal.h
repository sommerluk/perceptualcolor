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

/** @internal @file perceptualcolorinternal.h
 *
 * This file defines various definitions for compiling this
 * library. This header should be included in all CPP files of this library
 * (that means, in all translation units). */

#ifndef PERCEPTUALCOLORLIB_INERNAL_H
#define PERCEPTUALCOLORLIB_INERNAL_H

#include "PerceptualColor/perceptualcolorglobal.h"

static_assert(
    // Test if the compiler treats the source code actually as UTF-8.
    // We use the character “🖌” who’s code point is U+1F58C.
    // We create a string literal in the form U"🖌" which creates a UTF-32
    // encoded Unicode string. So we expect the first code unit of this string
    // to be 0x1F58C, which is the correct UTF-32 representation. We do
    // a static_assert to control if the compiler has actually correctly
    // interpreted the source code.
    (*(U"🖌")) == 0x1F58C,
    "Compiler must use UTF-8 as input character set.\n"
    "(The source code has to be interpreted as UTF-8 by the compiler.)");

static_assert((static_cast<quint8>(*(("🖌") + 0)) == 0xF0) && (static_cast<quint8>(*(("🖌") + 1)) == 0x9F) && (static_cast<quint8>(*(("🖌") + 2)) == 0x96) && (static_cast<quint8>(*(("🖌") + 3)) == 0x8C) &&
                  (static_cast<quint8>(*(("🖌") + 4)) == 0x00),
              "Compiler must use UTF-8 as narrow execution character set.\n"
              "(char* must contain UTF-8 encoded data.)\n"
              "Example: gcc -fexec-charset=UTF-8");

#endif // PERCEPTUALCOLORLIB_INERNAL_H
