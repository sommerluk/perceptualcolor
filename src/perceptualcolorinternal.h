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

/** @page characterset Compiler character sets
 *
 * Compilers have three different character sets:
 * - Input character set (the character set of the source code)
 * - Narrow execution character set
 *   (for “char” data type and string literals without prefix)
 * - Wide execution character set
 *   (for “wchar_t” data type and string literals with L prefix)
 *
 * <b>Input character set</b>
 *
 * This source code of this library is encoded in UTF8. Therefore, your
 * compiler must treat is also as UTF-8.
 *
 * Why are we using UTF-8 instead of ASCII?
 * - UTF-8 is more complete than ASCII. ASCII does not even provide basic
 *   typographic symbols like en-dash, em-dash or non-breaking space
 *   characters or quotes.
 * - Unicode exists since 1991, UTF-8 since 1993. It’s time to get rid of
 *   the insufficient ASCII character. It’s time to use Unicode.
 * - We use non-ASCII characters for (typographically
 *   correct) Doxygen documentation and partially also for non-Doxygen
 *   source code comments. It would be quite annoying to use HTML
 *   entinies for each non-ASCII character in the Doxygen documentation;
 *   and it would be pointless to do it for non-Doxygen source code
 *   comments.
 * - <tt>i18n()</tt> and <tt>ki18n()</tt> and <tt>tr()</tt> require both,
 *   the source file and <tt>char*</tt> to be encoded in UTF-8; no other
 *   encodings are supported. (Only ASCII would be UTF-8 compatible,
 *   but in practice this encoding is not supported, but only 8859-Latin
 *   encodings, which allow code points higher than 127, which risks to
 *   introduce incompatibilities. Therefore, this would not be a good
 *   option.)
 * - The C++ identifiers of library symbols are however (currently)
 *   ASCII-only.
 *
 * So we use a <tt>static_assert</tt> statement to control this.
 *
 * <b>Narrow execution character set</b>
 *
 * Why are we using UTF-8 as narrow execution character set?
 * - <tt>i18n()</tt> and <tt>ki18n()</tt> and <tt>tr()</tt> require both,
 *   the source file and <tt>char*</tt> to be encoded in UTF-8; no other
 *   encodings are supported.
 * - Implicit conversion from <tt>char*</tt> to <tt>QString</tt> assumes
 *   that <tt>char*</tt> is UTF-8 encoded. Thus we disable this implicit
 *   conversion in <tt>CMakeLists.txt</tt>, it’s wise to stay compatible.
 *
 * Therefore, a static assert controls that really UTF-8 is used
 * as narrow execution character set.
 *
 * <b>Wide execution character set</b>
 *
 * We do not use actively the wide execution character set. There is
 * a usage when communicating with LittleCMS, but there we depend anyway
 * from LittleCMS. Therefore, currently, no static assert forces a specific
 * wide execution character set. */

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
