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

/** @file
 * 
 * This file defines various macros, configuring Qt to disable
 * some API functions that provide implicid casts. Therefore,
 * by including this header in CPP files, the code of this library
 * if forced to use explicit casts instead of relying implicit casta;
 * this will helps to reveal bugs.
 * 
 * This header should be included in all CPP files of this library
 * (that means, in all translation units). It should <em>not</em> be
 * included in header files; in public header files, this would
 * change the Qt configuration of library uses (which is undesired),
 * and in private headers, it is pointless as there is no code to
 * translate. */

#ifndef PERCEPTUALCOLORLIB_QTCONFIGURATION_H
#define PERCEPTUALCOLORLIB_QTCONFIGURATION_H

#ifndef QT_NO_CAST_FROM_ASCII
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_CAST_FROM_ASCII
#endif // QT_NO_CAST_FROM_ASCII

#ifndef QT_NO_CAST_FROM_BYTEARRAY
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_CAST_FROM_BYTEARRAY
#endif // QT_NO_CAST_FROM_BYTEARRAY

#ifndef QT_NO_CAST_TO_ASCII
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_CAST_TO_ASCII
#endif // QT_NO_CAST_TO_ASCII

#ifndef QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
#endif // QT_NO_NARROWING_CONVERSIONS_IN_CONNECT

#ifndef QT_NO_PROCESS_COMBINED_ARGUMENT_START
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_PROCESS_COMBINED_ARGUMENT_START
#endif // QT_NO_PROCESS_COMBINED_ARGUMENT_START

#ifndef QT_NO_URL_CAST_FROM_STRING
/** @brief By defining this macro, this implicit cast of Qt is disabled. */
#define QT_NO_URL_CAST_FROM_STRING
#endif // QT_NO_URL_CAST_FROM_STRING

#endif // PERCEPTUALCOLORLIB_QTCONFIGURATION_H