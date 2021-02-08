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
 * This file provides macros for C++ symbol export. */

#ifndef PERCEPTUALCOLORLIB_GLOBAL_H
#define PERCEPTUALCOLORLIB_GLOBAL_H

#include <QtGlobal>

/** @def PERCEPTUALCOLORLIB_EXPORT
 * 
 * @brief A macro that marks library symbols for export.
 * 
 * Exported symbols will be visible symbols in the dynamic library.
 * 
 * This approach is proposed in Qt’s documentation (chapter “Creating
 * Shared Libraries”) – see there for more details. In short:
 * - When the library <em>itself</em> is build, the macro <em>exports</em> the
 *   corresponding symbol. (It expands to Qt’s <tt>Q_DECL_EXPORT</tt> macro.)
 * - When something is build <em>against</em> the dynamic library and
 *   includes the header files of the library, the macro <em>imports</em>
 *   the corresponding symbols. (It expands to Qt’s <tt>Q_DECL_IMPORT</tt>
 *   macro.)
 * 
 * It is necessary to define
 * <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt> when this library
 * itself is build. The definition can be made within CMake:
 * 
 * <tt>target_compile_definitions(PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)</tt>
 * 
 * And you have to make sure that all symbols that are <em>not</em>
 * explicitly exported have to be hidden:
 * 
 * <tt>set_target_properties(perceptualcolor PROPERTIES CXX_VISIBILITY_PRESET hidden)</tt>
 * 
 * However, when just something is build that is <em>linking</em>
 * against this library, nothing special has to be done and
 * <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt> should <em>not</em>
 * be defined.
 * 
 * @sa https://doc.qt.io/qt-5/sharedlibrary.html#using-symbols-from-shared-libraries
 * @sa http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html
 * @sa https://labjack.com/news/simple-cpp-symbol-visibility-demo */
#if defined(PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)
    #define PERCEPTUALCOLORLIB_EXPORT Q_DECL_EXPORT
#else
    #define PERCEPTUALCOLORLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // PERCEPTUALCOLORLIB_GLOBAL_H
