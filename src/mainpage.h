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

#ifndef MAINPAGE_H
#define MAINPAGE_H

/** @mainpage
 * This library provides various Qt GUI components for choosing colors, with
 * focus on an intuitive and perceptually uniform presentation. The GUI
 * widgets are based internally on the LCh color model, which does reflect
 * the human perception much better than RGB or its transforms like HSV.
 * However, the widgets do not require the user itself to know anything
 * about LCh at all, because the graphical representations is
 * intuitive enough. This library lives in the namespace #PerceptualColor.
 * 
 * How to get started? @ref PerceptualColor::ColorDialog provides a perceptual
 * replacement for QColorDialog:
 * @snippet test/testcolordialog.cpp ColorDialog Get color
 * 
 * And there are also individual widgets available. Among others:
 * - @ref PerceptualColor::WheelColorPicker (a full-featured color wheel)
 * - @ref PerceptualColor::ColorPatch (to show a particular color)
 * - @ref PerceptualColor::ChromaHueDiagram (for selecting colors at a given
 *   lightness)
 * 
 * The library depends on (and therefore you has to link against):
 * |                         | Qt                 | LittleCMS               |
 * | :---------------------- | :----------------- | :---------------------- |
 * | <b>Major release</b>    | 5                  | 2                       |
 * | <b>Minimum version</b>  | ≥ 5.6*             | ≥ 2.0                   |
 * | <b>Required modules</b> | Core, Gui, Widgets | <em>not applicable</em> |
 * <em>* Qt 5.6 introduces <tt>QPaintDevice::devicePixelRatioF()</tt> which is
 * used in this library.</em>
 *
 * This library requires minimum C++11.
 * <!--
 *      Qt 5.6 (which is the mimimum Qt version required
 *      by this library) only requires C++03. Only starting
 *      with Qt 5.7, Qt itself requires C++11. Source:
 *      https://doc.qt.io/qt-5.9/cmake-manual.html#using-qt-5-with-cmake-older-than-3-1-0
 * 
 *      Qt 6 requires minimum C++17, as
 *      https://doc-snapshots.qt.io/qt6-dev/cmake-get-started.html
 *      explains.
 * 
 *      Our library code uses C++11 features, for example “constexpr”.
 * 
 *      In the CMakeLists.txt file, we set -std=c++17 and we set
 *      also -Wpedantic and -pedantic-errors to enforce it.
 *      That might be an option for this library,
 *      if we decide to make it Qt-6-only. But it might
 *      even be useful if we support Qt 5, so we have future-proof
 *      requirements that we do not have to raise soon, and that are a
 *      good base for LTS.
 * -->
 * 
 * The library uses in general <tt>int</tt> for integer values, because
 * <tt>QSize()</tt> and <tt>QPoint()</tt> also do. As the library relies
 * heavily on the usage of <tt>QSize()</tt> and <tt>QPoint()</tt>, this
 * seems reasonable. For the same reason, it uses generally <tt>qreal</tt>
 * for floating point values, because <tt>QPointF()</tt> also does. Output
 * colors that are shown on the screen, are usually 8-bit-per-channel
 * colors. For internal transformation, usually <tt>qreal</tt>
 * is used for each channel, giving a better precision and reducing rounding
 * errors.
 * 
 * The source code of the library is in UTF8. A static_assert within the
 * header @ref helper.h makes sure your compiler actually treats it as UTF8.
 * 
 * @copyright Almost all the code is published under MIT License. Only
 * <tt>cmake/Modules/FindLCMS2.cmake</tt> is licenced under BSD-3-Clause
 * license. The <tt>LICENSES</tt> subfolder contains copies of the licence
 * texts.
 * 
 * @note This library uses the <em>pointer to implementation</em> idiom
 * (also known as pimpl idiom, d-pointer idiom or opaque-pointer idiom).
 * This idiom is also used by Qt itself, and Qt even provides some macros
 * and extension points (<tt>Q_DECLARE_PRIVATE</tt>, <tt>Q_D</tt>, a protected
 * member called <tt>d_ptr</tt> in almost all classes…), that help dealing
 * with the pimpl idiom. Though available, these Qt features are not
 * officially documentated; and they would also interfer with private
 * implementations of Qt itself without documented behaviour, which seems
 * inappropriate. Furthermore, the Qt pimpl idiom is complicate because
 * it uses (for performance reasons) inheritance between the private
 * implementation classes. This breaks, however, the encapsulation, because
 * all formerly private elements of a class become protected know. Our class
 * hierarchy is not that deep, so the performance gain might not be worth
 * the additional code complexity. Therefore, this library uses a more simple
 * pimpl idiom without inheritance of the private implementation, It has
 * however all the other features of the Qt pimpl idiom, including
 * <tt>const</tt> propagating access to the private implementation thanks to
 * @ref PerceptualColor::ConstPropagatingUniquePointer. And, at difference
 * to Qt’s pimpl idiom, it keeps private code strictly private.
 * Note however, that switching later from our current pimpl idiom to
 * the polymorph Qt pimpl idiom would break the binary compatibility. See also
 * https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B
 * and https://accu.org/journals/overload/18/100/love_1718/ for details.
 * 
 * @todo Review all static class functions: Which should be made non-static?
 * (Maybe in the future, it might be necessary to access object data, so
 * better making them non-static so this stays possible.)
 * 
 * @todo Avoid default parameters as they would require recompilation of
 * applications?
 * 
 * @todo <tt>private Q_SLOTS</tt> can be connected to (with the old connection
 * syntax) from everywhere, so they are indeed not private. That’s not great.
 * As we use almost exclusively the new connection syntax, we can connect
 * to all functions, not only slots, so we could simply change private slots
 * to private functions. Or, better: If switching to pimpl, in the private
 * class, the private slots will become public slots, which is fine and
 * expressive.
 * 
 * @todo mark all public non-slot functions with Q_INVOKABLE (except property
 * setters and getters)
 * 
 * @todo Delete all default arguments in public headers, as changes require
 * re-compilation of the client application to take effect, which might lead
 * to a miss-match of behaviour between app and lib, if compile-time and
 * run-time version of the lib are not the same.
 * 
 * @todo A good widget library should
 * - run on all systems (✓ We do not use system-specific code nor do we
 *   rely on byte order / Is this correct? Image generation code?)
 * - support hight-dpi (? work in progress)
 * - stylable by QStyle (? partial)
 * - stylable by style sheets (✗)
 * - accessibility (✗)
 * 
 * @todo A good widget library should also be touchscreen-ready.
 * - An alternative to
 *   the spin box? How, for up to 360 values (degrees in step by 1)? Or
 *   should the steps simply be bigger?
 * 
 * @todo KDE Frameworks / https://marketplace.qt.io/ ?
 * 
 * @todo Provide property bindings as described in 
 * https://www.qt.io/blog/property-bindings-in-qt-6 or not? It is worth
 * when we do not support QML? What are the pitfalls? Imagine a property
 * that holds a percent value from 0 to 100; the setter enforces this
 * range; the binding bypasses the setter and allows every value? And:
 * How can callbacks know about when a setter was called in C++? See
 * also: https://doc.qt.io/qt-5/qtqml-cppintegration-exposecppattributes.html
 * and https://doc.qt.io/qt-5/qtqml-tutorials-extending-qml-example.html and
 * http://blog.aeguana.com/2015/12/12/writing-a-gui-using-qml-for-a-c-project/
 * for interaction between QML and C++. Pitfalls: Example of color() property
 * stored internally at m_color: Much implementation code of the clas will
 * access directly m_color instead of color(), so when using bindings,
 * this code is broken?
 * 
 * @todo Provide QML support so that for
 * https://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html (or its
 * Qt6 counterpart) we provide a source compatible alternative, like for
 * QColorWidget? Split the library in three parts (Common, Widgets, QML)?
 * 
 * @todo no constexpr should be exposted in any header as they are determined
 * on compile time. So if later we change the value, an application could
 * have a different value in the lib file (compile time of the library) and
 * the executable (compile time of the executable, which could be compoiled
 * against a different header/version).
 * 
 * @todo Comply with KDE policies: https://community.kde.org/Policies
 * 
 * @todo Decide is it’s better to raise C++ requirement to C++17 (see the
 * hidden comment above in this source file).
 * 
 * @todo Remove all qDebug calls from the source
 * 
 * @todo Use QObject::tr() for translations. Provide po files.
 * 
 * @todo Qt6 property bindings (QProperty QPropertyBinding) for all
 * properties?
 * 
 * @todo Is Qt 5.6 actually enough?. Even if so, wouldn’t it
 * be better to require the last LTS release (5.15), just to be compatible if
 * in the future we depend on this? */

/** @brief The namespace of this library.
 * 
 * Everything that is provides in this library is encapsulated within this
 * namespace.
 * 
 * 
 * @todo This library provides some constexpr in the public API. If now we
 * release version 1.0.0 of this library with constexpr x == 1. Now, in
 * version 1.1.0 of this library, we change its value to constexpr x == 5.
 * Now, two questions: 1) Does this break binary compatibility? 2) As
 * constexpr is (or at least <em>might</em>) be evaluated at compile
 * time; if a program is compiled against version 1.0.0 but executed
 * with version 1.1.0, doest’t then constexpr x have different values
 * within the library code and within the program code, which might
 * lead to undefined behaviour?
 * @todo Test well the scaling for all widgets from 106.25% up to 200%.
 * @todo When scaling is used, the icons on the OK button and the
 * Cancel button are ugly. Why isn’t this handled automatically correctly,
 * though on other Qt apps it seems to be handled automatically correctly?
 * @todo Translations: Color picker/Select Color → Farbwähler/Farbauswahl etc…
 * @todo Provide more than 8 bit per channel for more precision? 10 bit?
 * 12 bit?
 * @todo Only expose in the headers and in the public API what is absolutely
 * necessary.
 * @todo Switch to the pimpl idiom? Export symbols like in
 * https://doc.qt.io/qt-5/sharedlibrary.html#using-symbols-from-shared-libraries
 * See also http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html
 * and https://labjack.com/news/simple-cpp-symbol-visibility-demo
 * @todo A program that uses our library could also use LittleCMS itself. If
 * it would use LittleCMS without thread-save API, but using it always in the
 * very same thread which is <em>not</em> the main thread, this could make
 * problems for our library if we use non-thread-save LittleCMS APIs. So
 * best would be that our library uses exclusively <em>thread-save</em>
 * APIs of LittleCMS.
 * @todo Qt Designer support for the widgets
 * @todo It might be useful to support for all widgets grayed out appearance
 * when they are disabled. Just fade out, maybe with some transparency, and
 * let colors still be visible, would be a bad idea: It would be highly
 * confusing seeing colors, but colors that are wrong. So everything would
 * have to be gray. For @ref ColorPatch and @ref GradientSelector this could
 * be simply the default background, similar to grayed out spin boxes. And for
 * the diagram widgets, only the shape would stay visible, with uniform
 * standard gray colors coming from <tt>QStyle</tt>. The markers might not
 * even be displayed at all.
 * @todo Follow KDE's <b><a href="https://hig.kde.org/index.html">HIG</a></b>
 * @todo Add screenshots of widgets to the documentation
 * @todo Would it be a good idea to implement Q_PROPERTY RESET overall? See
 * also https://phabricator.kde.org/T12359
 * @todo Spell checking for the documentation */
namespace PerceptualColor {
}

#endif // MAINPAGE_H
