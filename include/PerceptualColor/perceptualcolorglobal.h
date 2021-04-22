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

/** @file perceptualcolorglobal.h
 *
 * This file provides macros for C++ symbol export and some static asserts.
 * It is ment to be included in header files of this library. */

#ifndef PERCEPTUALCOLORLIB_GLOBAL_H
#define PERCEPTUALCOLORLIB_GLOBAL_H

#include <QtGlobal>

/** @def PERCEPTUALCOLOR_IMPORTEXPORT
 *
 * @brief A macro that either exports or imports library symbols.
 *
 * This approach is proposed in Qt’s documentation (chapter “Creating
 * Shared Libraries”) – see there for more details. In short:
 *
 * <b>When your application is build <em>against</em> the dynamic
 * library</b> and includes the header files of the library, the
 * macro <em>imports</em> the corresponding symbols of the library
 * for you by expanding to Qt’s <tt>Q_DECL_IMPORT</tt> macro. This
 * is the default behaviour of this macro (happens always when
 * <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt> is <em>not</em>
 * defined).
 *
 * <b>When the dynamic library <em>itself</em> is build</b>, the macro
 * <em>exports</em> the corresponding symbol by expanding to Qt’s
 * <tt>Q_DECL_EXPORT</tt> macro. Exported symbols will be visible
 * symbols in the dynamic library. To get this behaviour, it is
 * necessary to define <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt>
 * always when this library itself is build. The definition can be made
 * within CMake:
 *
 * <tt>target_compile_definitions(PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)</tt>
 *
 * And you also have to make sure that all symbols that are <em>not</em>
 * explicitly exported will be actually hidden:
 *
 * <tt>set_target_properties(perceptualcolor PROPERTIES CXX_VISIBILITY_PRESET
 * hidden)</tt>
 *
 * @sa https://doc.qt.io/qt-5/sharedlibrary.html#using-symbols-from-shared-libraries
 * @sa http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html
 * @sa https://labjack.com/news/simple-cpp-symbol-visibility-demo */
#if defined(PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_EXPORT
#else
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_IMPORT
#endif

#endif // PERCEPTUALCOLORLIB_GLOBAL_H
