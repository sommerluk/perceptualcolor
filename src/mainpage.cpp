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

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// This file contains the following documentation:
// – @mainpage
// – All the @page documentation. Putting all @page documentation in this
//   single file allows that they show up in the documentation in
//   alphabetical order.
// – The namespace documentation

/** @mainpage
 * This library provides various Qt GUI components for choosing colors, with
 * focus on an intuitive and perceptually uniform presentation. The GUI
 * widgets are based internally on the LCh color model, which does reflect
 * the human perception much better than RGB or its transforms like HSV.
 * However, the widgets do not require the user itself to know anything
 * about LCh at all, because the graphical representations is
 * intuitive enough.
 *
 * How to get started? @ref PerceptualColor::ColorDialog provides a
 * perceptual replacement for QColorDialog:
 * @snippet test/testcolordialog.cpp ColorDialog Get color
 *
 * @image html ColorDialog.png "ColorDialog" width=500
 *
 * And there are also individual widgets available. Among others:
 * - @ref PerceptualColor::WheelColorPicker (a full-featured color wheel)
 * - @ref PerceptualColor::ColorPatch (to show a particular color)
 * - @ref PerceptualColor::ChromaHueDiagram (for selecting colors at a
 *   given lightness)
 *
 * @section namespacepollution Namespace pollution and collisions:
 * - Macros are prefixed with <tt>PERCEPTUALCOLOR_</tt> to avoid collisions.
 *   (See <em>Files → File members</em> for a list of all macros.)
 * - To avoid namespace pollution, all symbols of this library live within
 *   the namespace @ref PerceptualColor.
 *
 * This library relies internally on LittleCMS for all the color management.
 * Anyway, you can uses this library without knowing about the internals of
 * LittleCMS.
 *
 * @sa @ref build
 * @sa @ref hidpisupport
 *
 * @copyright Almost all the code is published under MIT License.
 * Only <tt>cmake/Modules/FindLCMS2.cmake</tt> is licenced under BSD-3-Clause
 * license. The <tt>LICENSES</tt> subfolder contains copies of the licence
 * texts.
 *
 * @internal
 *
 * @todo Declare a non-const dereference operator within
 * @ref PerceptualColor::ConstPropagatingUniquePointer and get rid of all
 * <tt>.get()</tt> within <tt>connect()</tt> calls.
 *
 * @todo From KDE’s binary compatibility info page: In order to make a class
 * to extend in the future you should follow these rules:
 * - add d-pointer. See below.
 * - add non-inline virtual destructor even if the body is empty.
 * - reimplement event in QObject-derived classes, even if the body for
 *   the function is just calling the base class' implementation. This is
 *   specifically to avoid problems caused by adding a reimplemented virtual
 *   function as discussed below.
 * - make all constructors non-inline.
 * - write non-inline implementations of the copy constructor and assignment
 *   operator unless the class cannot be copied by value (e.g. classes
 *   inherited from QObject can't be)
 *
 * @todo Where possible: Make the pimpl inherit from <tt>public QObject</tt>
 * to allow to get rid of lambdas in the connect statements.
 *
 * @todo In https://phabricator.kde.org/T12359 is recommanded to provide
 * RESET statements for all properties for better compatibility with QML.
 * As we provide widgets, this should not be too important. Are there also
 * good arguments for widgets to provide RESET?
 *
 * @todo Provide Q_DECLARE_TYPEINFO for our types? Provide an init()
 * function that calls qRegisterMetaType() for all our types?
 *
 * @todo Support i18n language change on-the-fly?
 *
 * @todo We prevent division by 0 in
 * @ref PerceptualColor::ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::fromWidgetPixelPositionToColor().
 * We should make sure this happens also in the other diagram widgets!
 *
 * @todo Add a @ref PerceptualColor::ConstPropagatingUniquePointer to
 * all classes, including the non-pimpl classes, to allow for later
 * enhancements.
 *
 * @todo Remove setDevicePixelRatioF from all *Image classes. (It is
 * confusing, and at the same time there is no real need/benefit.)
 * Complete list: @ref PerceptualColor::ChromaHueImage,
 * @ref PerceptualColor::ColorWheelImage, @ref PerceptualColor::GradientImage.
 *
 * @todo No caller graph/referenced by, in doxygen. (Result is not reliably. Better no graph
 * than an incomplete graph.)
 *
 * @todo Test also on Windows. (Does it work well with VisualStudio?
 * Isn’t there a free version of VisualStudio? Test it with this
 * library!)
 *
 * @todo Test opaque RGB color space object with a non-export-all version
 * of this library to make sure it actually works for third-party developers.
 *
 * @todo Sometimes, on dual-screen setup, one screen has another DPI than
 * the other screen. Does this library behave correctly in these situations?
 *
 * @todo Would it make sense for @ref PerceptualColor::ChromaHueDiagram and
 * @ref PerceptualColor::ChromaLightnessDiagram to split up their property
 * <tt>currentColor</tt> into two properties: A two-dimensional property
 * for what the user can change, and a one-dimensional property
 * for what only the programmer can change? Or at least provide
 * a Q_INVOKABLE getter and maybe also setter support? So
 * @ref PerceptualColor::WheelColorPicker could use this
 * instead of a lambda expression to set the hue of the
 * @ref PerceptualColor::ChromaLightnessDiagram. And: Also when we don’t do
 * that: When setting <tt>currentColor</tt> to an out-of-gamut color,
 * what happens? Does @ref PerceptualColor::ChromaHueDiagram preserve
 * lightness, while @ref PerceptualColor::ChromaLightnessDiagram preserves
 * hue? Would this make sense?
 *
 * @todo Paint grayed-out handles for all widgets when setEnabled(false)
 * is used! For example 25% lightness instead of black. And 75%
 * lightness instead of white. But: Provide this information
 * in @ref PerceptualColor::AbstractDiagram!
 *
 * @todo Provide a link to (the online version of) this page in README.md
 *
 * @todo The doxygen documentation should not recommand <tt>filename.h</tt>
 * but <tt>PerceptualColor/filename.h</tt>!?
 *
 * @todo Review and unit tests for these classes:
 * @ref PerceptualColor::ChromaLightnessDiagram,
 * @ref PerceptualColor::RgbColorSpace
 * <br/>
 *
 * @todo It might be interesting to use <tt>QStyle::PM_FocusFrameHMargin</tt>
 * <em>(Horizontal margin that the focus frame will outset the widget
 * by.)</em> Or: <tt>QStyle::PM_FocusFrameVMargin</tt>. Using this for the
 * distance between the focus indicator and the actual content of the widget
 * maybe give a more <tt>QStyle</tt> compliant look. But: If using this,
 * ensurePolished() must be called before!
 *
 * @todo Use <tt>explicit</tt> on all constructors?
 *
 * @todo Screen picker with magnifier glass in two steps
 * similar to https://colorsnapper.com ?
 *
 * @todo Multi-licensing? Add Boost licence and Unlicense as an additional
 * choise?
 *
 * @todo The image cache for the gamut widgets should be updated
 * asynchronously (in its own thread or even various own threads
 * in parallel). While waiting for the result, an empty image could be used.
 * Or it might be useful to provide first a low-resolution version, and only
 * later-on a high-resolution version. Anyway, KDE provides an interesting
 * recommandation: <tt>int Units::humanMoment = 2000;</tt> <em>Time in
 * milliseconds equivalent to the theoretical human moment, which can be
 * used to determine whether how long to wait until the user should be
 * informed of something, or can be used as the limit for how long something
 * should wait before being automatically initiated. / Some examples: /
 * When the user types text in a search field, wait no longer than this
 * duration after the user completes typing before starting the search /
 * When loading data which would commonly arrive rapidly enough to not
 * require interaction, wait this long before showing a spinner</em> See
 * https://api.kde.org/frameworks/plasma-framework/html/classUnits.html#ab22ad7033b2e3d00a862650e82f5ba5e
 * for details.
 *
 * @todo HLC @ref PerceptualColor::MultiSpinBox Allow entering (on the
 * keyboard) of too big hues (361°), negatif hues (-1°), negatif chroma (-20)
 * and too big chroma (201 or 256) – but do not allow this with the arrows
 * (and how do the arrows react when currently one of these values is
 * shown?). Does this make sense? Anyway do <em>not</em> allow this for
 * lightness, because the lightness is <em>by definition</em> bound
 * to <tt>[0, 100]</tt>.
 *
 * @todo Multi-threaded application of color transforms. It seems okay to
 * create the color transforms in one thread and use the same color
 * transform (once created) from various other threads at the same time
 * as long as the flag <tt>cmsFLAGS_NOCACHE</tt> is used to create the
 * transform.
 *
 * @todo Automatically scale the thickness of the wheel (and maybe even the
 * handle) with varying widget size?
 *
 * @todo Support more color spaces? https://pypi.org/project/colorio/ for
 * example supports a lot of (also perceptually uniform) color spaces…
 *
 * @todo Export lesss symbols?
 *
 * @todo Check in all classes that take a @ref PerceptualColor::RgbColorSpace
 * that the shared pointer is actually not a <tt>nullptr</tt>. If is
 * <em>is</em> a <tt>nullptr</tt> than throw an exception. Throwing the
 * exception early might make error detection easier for users of the library.
 *
 * * @todo Avoid default arguments like <tt>void test(int i = 0)</tt> in
 * public headers, as changes require re-compilation of the client application
 * to take effect, which might lead to a miss-match of behaviour between
 * application and library, if  compile-time and run-time version of the
 * library are not the same. Is the problem  for default constructors
 * like <tt>ClassName() = default</tt> similar?
 *
 * @todo mark all public non-slot functions with Q_INVOKABLE (except property
 * setters and getters)
 *
 * @todo A good widget library should
 * - provide a complete documentation (✓)
 * - run on all systems (✓ We do not use system-specific code nor do we
 *   rely on byte order)
 * - internationalization (i18n) done. This include support for right-to-left
 *   layouts, as well as be ready for translation. (✓)
 * - localization (l10n)
 * - support hight-dpi (✓)
 * - stylable by QStyle (? partial)
 * - stylable by style sheets (✗)
 * - accessibility (✗) [This includes to work well with bigger fonts. Should then the gradient be thicker and the marker thicker? setAccessibleName(). ]
 *
 * @todo A good widget library should also be touchscreen-ready. Find
 * an alternative to @ref PerceptualColor::MultiSpinBox? How, for up
 * to 360 values (degrees in step by 1)? Or should the steps simply be bigger?
 *
 * @todo KDE Frameworks / https://marketplace.qt.io/ ?
 * https://community.kde.org/Incubator
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
 * @todo Apparently QWidget cannot be used from QML. (Though there is
 * https://www.kdab.com/declarative-widgets/ – how does that work?) Is it
 * therefore worth to have complete support for signals in all our QWidget
 * code if this is not really necessary for QWidget (for example for
 * properties that can only be changed programatically and not by the
 * user)?
 *
 * @todo Comply with KDE policies: https://community.kde.org/Policies
 *
 * @todo Remove all qDebug calls from the source
 *
 * @todo Use QObject::tr() for translations.
 * Provide po files? How can a library be localized?
 * See https://community.kde.org/Frameworks/Frameworks_Localization_Policy for
 * ideas.
 *
 * @todo Translations: Color picker/Select Color → Farbwähler/Farbauswahl etc…
 *
 * @todo Qt Designer support for the widgets. Quote from a blog from Viking
 * about Qt Designer plugins:
 * The problem is that you have to build it with exactly the same compiler
 * tool chain as designer was built with, and you have to do it in release
 * mode. Unless your Qt is built in debug, then your plugin needs to be
 * built in debug mode as well. So you can’t just always use the same
 * compiler as you build the application with, if you use the system Qt or
 * a downloaded Qt version.
 *
 * @todo Use <a href="https://lvc.github.io/abi-compliance-checker/">
 * abi-compliance-checker</a> to control ABI compatibility.
 *
 * @todo Follow KDE’s <a href="https://hig.kde.org/index.html">HIG</a>
 *
 * @todo Test linking against lcms.h in version 2.0.0 for compatibility
 * (or require more recent version?)
 *
 * @todo Require (by static cast additional to CMake conditions) a minimum
 * Qt version?
 *
 * @todo Would it be a good idea to implement Q_PROPERTY RESET overall? See
 * also https://phabricator.kde.org/T12359
 *
 * @todo Better design on small widget sizes for the whole library.
 *
 * @todo Anti-aliasing the gamut diagrams? Wouldn't this be bad for
 * performance?
 *
 * @todo Use a cross-hair cursor on @ref PerceptualColor::ChromaHueDiagram
 * and @ref PerceptualColor::ChromaLightnessDiagram when the mouse is
 * hovering over the gamut, to show that this surface can be clicked?
 *
 * @todo Touch-friendly interface: Would it be good to have buttons for
 * plus and minus on the various LCH axis which would be companings
 * for @ref PerceptualColor::ChromaHueDiagram and
 * @ref PerceptualColor::ChromaLightnessDiagram and would allow
 * more exactly choose colors also on touch devices?
 *
 * @todo Would it be a good idea to have plus and minus buttons that
 * manipulate the current color along the depth and vividness axis
 * as proposed in “Extending CIELAB - Vividness, V, depth, D, and clarity, T”
 * by Roy S. Berns?
 *
 * @todo Spell checking for the documentation, if possible also grammar
 * checking with LanguageTool */

/** @page build Build instructions and requirements
 * The library depends on (and therefore you has to link against) these
 * shared/dynamic libraries:
 *
 * |                         | Qt                 | LittleCMS               |
 * | :---------------------- | :----------------- | :---------------------- |
 * | <b>Major release</b>    | 5                  | 2                       |
 * | <b>Minimum version</b>  | ≥ 5.6*             | ≥ 2.0                   |
 * | <b>Required modules</b> | Core, Gui, Widgets | <em>not applicable</em> |
 *
 * <em>* Qt 5.6 introduces <tt>QPaintDevice::devicePixelRatioF()</tt> which is
 * used in this library.</em>
 *
 * Please make sure that you comply with the licences of the libraries
 * you are using.
 *
 * There is also a LittleCMS plugin called <em>fast_float plug-in</em> that
 * you can include into the source code of your application and load it
 * in your main function before using this library. This can make
 * color management faster. (Note that this plugin has a different license
 * than LittleCMS itself.)
 *
 * This library requires minimum C++17.
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
 *      also -Wpedantic and -pedantic-errors to enforce it. That is
 *      a useful option for this library if we decide to make it Qt-6-only.
 *      But it is even be useful if we support Qt 5, so we have future-proof
 *      requirements that we do not have to raise soon, and that are a
 *      good base for LTS.
 * -->
 *
 * To compile this library, both the input character set and the execution
 * character set have to be UTF8. (See @ref compilercharacterset for more
 * details.)
 *
 * You also need CMake for the build process.
 *
 * @internal
 *
 * @todo Is Qt 5.6 actually enough?. Even if so, wouldn’t it
 * be better to require the last LTS release (5.15), just to be compatible if
 * in the future we depend on this?
 *
 * @todo Provide detailed build instructions.
 *
 * @todo Provide a CMake find module for this library and install it. */

/** @internal
 *
 * @page codingstyle Coding style
 *
 * - Document your code.
 * - Provide unit tests for your code.
 * - If working with children within Qt’s object hierarchy, allocate on the
 *   heap and use raw pointers or guarded pointers (`QPointer`). If not,
 *   allocate on the stack or use smart pointers. Prefer Qt’s smart pointers
 *   over the <tt>std</tt> smart pointers of C++. */

/** @page compilercharacterset Compiler character sets
 *
 * @section compilercharacterset_ Compiler character sets
 *
 * Compilers have three different character sets:
 * - Input character set (the character set of the source code)
 * - Narrow execution character set
 *   (for “char” data type and string literals without prefix)
 * - Wide execution character set
 *   (for “wchar_t” data type and string literals with L prefix)
 *
 * @subsection inputcharacterset Input character set
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
 * @subsection narowexecutioncharacterset Narrow execution character set
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
 * @subsection wideexecutioncharacterset Wide execution character set
 *
 * We do not use actively the wide execution character set. There is
 * a usage when communicating with LittleCMS, but there we depend anyway
 * from LittleCMS. Therefore, currently, no static assert forces a specific
 * wide execution character set.
 *
 * @internal
 *
 * @note The static asserts that enforce the character sets are located
 * in @ref perceptualcolorinternal.h. */

/** @internal
 *
 * @page datatypes Data types
 *
 * The library uses in general <tt>int</tt> for integer values, because
 * <tt>QSize()</tt> and <tt>QPoint()</tt> also do. As the library relies
 * heavily on the usage of <tt>QSize()</tt> and <tt>QPoint()</tt>, this
 * seems reasonable.
 *
 * For the same reason, it uses generally <tt>qreal</tt>
 * for floating point values, because <tt>QPointF()</tt> also does.
 *
 * Output colors that are shown on the screen, are usually 8-bit-per-channel
 * colors. For internal transformation, usually <tt>qreal</tt>
 * is used for each channel, giving a better precision and reducing rounding
 * errors. */

/** @page hidpisupport High DPI support
 * This library supports High DPI out of the box. You do not need to do
 * much to use it. The widgets provide High DPI support automatically.
 *
 * The only problem are icons. Icons are used for
 * @ref PerceptualColor::MultiSpinBox::addActionButton and for
 * the “refresh” icon and (on some widget styles) for the “Ok”
 * button and the “Cancel” button in @ref PerceptualColor::ColorDialog.
 *
 * @section loadicons Load icons
 *
 * This library uses by default a possibly existing icon theme
 * if available in Qt.
 *
 * - Windows and Mac do not provide icon themes by default, while Linux
 *   usually provides them.
 *
 * - You might bundle icons (as resource) with your application.
 *
 * There are different file formats for icon themes:
 *
 * - Loading raster image icons is supported out-of-the-box by Qt.
 *
 * - Loading SVG icons is supported by Qt’s SVG icon
 *   support plugin. (On Linux this is the file
 *   <tt>plugins/iconengines/libqsvgicon.so</tt>). This
 *   plugin is loaded by Qt automatically if present.
 *
 * SVG is pretty much the standard nowadays and the only
 * reliably way to have crisp icons also on desktop scales like 1.25 or 1.5.
 * Make sure that the plugin is present if you want that SVG icons
 * can be loaded. (On Linux, it seems possible to enforce this by linking
 * dynamically to the plugin itself, if you want to. This forces Linux
 * package managers to produce packages of your application that depend
 * not only on Qt base, but also on the SVG plugin.)
 *
 * If no external icon can be loaded by Qt, this library uses hard-coded
 * fallback icon where necessary.
 *
 * @section rendericons Render icons
 *
 * - <a href="https://bugreports.qt.io/browse/QTBUG-89279">Qt6 renders icons
 *   always with high-DPI.</a>
 * - Qt5 renders icons by default in low resolution. This applies even
 *   for SVG icons on high-DPI displays! Application developers have to enable
 *   high-DPI icon rendering manually with the following code (which should be
 *   put by convention <em>before</em> creating the <tt>QCoreApplication</tt>
 *   object):
 *   <br/><tt>QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);</tt>
 *
 * Exception: The hard-coded fallback icons of this library render <em>always
 * and on all Qt versions (even if no SVG support is available at all
 * in Qt)</em> at high-DPI! */

/** @internal
 *
 * @page measurementdetails Measurement details
 *
 * When this library deals with raster graphics, it simultaniously uses
 * concepts concerning measurement. This page describes the terminology
 * used within the documentation of this library.
 *
 * @section introduction Introduction
 * Today’s displays have a wide range of physical pixel density (pixels
 * per length). Displays with a high physical pixel density are called
 * <b>High-DPI displays</b> or <b>HiDPI displays</b> or <b>Retina displays</b>.
 *
 * @section unitsofmeasurement Units of measurement
 * As Qt docuemntation says:
 *      “<em>Qt uses a model where the application coordinate system is
 *      independent of the display device resolution. The application
 *      operates in </em>device-independent pixels<em>, which are then
 *      mapped to the physical pixels of the display via a scale
 *      factor, known as the </em>device pixel ratio<em>.</em>”
 *
 * So when rendering widgets, there are two different units of measurement
 * to consider:
 * - <b>Device-indepentend pixels</b> are the  unit of measurement for
 *   widgets, windows, screens, mouse events and so on in Qt.
 * - <b>Physical pixels</b> are the unit that measures actual physical
 *   display pixels.
 *
 * The conversion factor between these two units of measurement is
 * <tt>QPaintDevice::devicePixelRatioF()</tt>, a floating point number.
 * It is usually <tt>1.00</tt> on classic low-resolution screens. It could be
 * for example <tt>1.25</tt> or <tt>2.00</tt> on displays with a higher
 * pixel density.
 *
 * @section coordinatepointsversuspixelpositions Coordinate points versus pixel positions
 *
 * - <b>Coordinate points</b> are points in the mathematical sense, that
 *   means they have zero surface. Coordinate points should be stored as
 *   <em>floating point numbers</em>.
 * - <b>Pixel positions</b> describe the position of a particular pixel
 *   within the pixel grid. Pixels are surfaces, not points. A pixel is a
 *   square of the width and length <tt>1</tt>. The pixel at position
 *   <tt>QPoint(x, y)</tt> is the square with the top-left edge at coordinate
 *   point <tt>QPoint(x, y)</tt> and the botton-right edge at coordinate
 *   point <tt>QPoint(x+1, y+1)</tt>. Pixel positions should be stored
 *   as <em>integer numbers</em>.
 *
 * Some functions (like mouse events) work with pixel positions, other
 * functions (like antialiased floatting-point drawing operations) work
 * with coordinate points. It’s important to always distinguish correctly
 * these two different concepts. See https://doc.qt.io/qt-6/coordsys.html
 * for more details about integer precision vs floating poin precision
 * on drawing operations. */

/** @internal
 *
 * @page multithreading Multithreading
 *
 * Currently, this library does not use multithreading. However, is seems
 * a good idea to implement multithreading in the future, particulary
 * for generating the gamut images, which seems to be the slowest
 * operation of this library. It could get disconnected from the GUI
 * thread, leading to a more responsive GUI. And the image could be
 * calculated by various threads simultaniously, so that the generation
 * could be faster.
 *
 * Points to consider:
 * - LittleCMS seems to allow using the same transform simultaniously
 *   from variouis threads as long as the 1-pixel-cache is disabled.
 * - QPixmap may only be used in the GUI thread. To generate the images
 *   in another thread, QImage must be used. */

/** @internal
 *
 * @page pimpl Pointer to implementation idiom
 *
 * This library uses the <em>pointer to implementation</em> idiom
 * (also known as pimpl idiom, d-pointer idiom or opaque-pointer idiom)
 * in almost all classes that are part of the public API, and also in
 * some classes that are part of the private API.
 *
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
 * pimpl idiom without inheritance of the private implementation. It has
 * however all the other features of the Qt pimpl idiom, including
 * <tt>const</tt> propagating access to the private implementation
 * thanks to @ref PerceptualColor::ConstPropagatingUniquePointer and
 * @ref PerceptualColor::ConstPropagatingRawPointer. And, at difference
 * to Qt’s pimpl idiom, it keeps private code strictly private.
 * Note however, that switching later from our current pimpl idiom to
 * the polymorph Qt pimpl idiom would break the binary
 * compatibility. See also the document <em>
 * <a href="https://accu.org/journals/overload/18/100/love_1718/">Interface
 * Versioning in C++</a></em> and KDE’s information document <em>
 * <a
 * href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B">
 * Binary Compatibility Issues With C++</a></em> and for details.
 *
 * @note It would be nice to have the d_pointer and q_pointer
 * be themselfs be declared <tt>const</tt>, because this would
 * clearly communicate that those pointers are not expected to change
 * the adress they point to. Unfortunely, apparently this does not
 * work with neither @ref PerceptualColor::ConstPropagatingUniquePointer nor
 * @ref PerceptualColor::ConstPropagatingRawPointer as it would change also
 * all the access rights to the pointed object to always <tt>const</tt>. */

/** @page lchrange Range of LCH values
 *
 * The LCH values in this library are implemented with the following range:
 *
 * |    L     |    C     |    H     |
 * | :------: |:-------: | :------: |
 * | [0, 100] | [0, 200] | [0, 360[ |
 *
 * This range is enough to cover the hole range of human perception. Note
 * that the actual range of human perception has an irregular shape and
 * covers only parts of all possible combinations of LCH values. And
 * the actual gamut of real-word output devices is smaller than the
 * human perception.
 *
 * @internal
 *
 * @section lchrangerationale Rationale
 *
 * The gamut of actual human perception within the LAB color model (and
 * its alternative representation LCH) has an irregular shape. Its maximum
 * extensions:
 *
 * <b>Lightness (L)</b>
 * The maximum range for LAB/LCH lightness is limited by
 * definition: <tt>[0, 100]</tt>.
 *
 * <b>Hue (H)</b>
 * The maximum range for LCH hue is limited by definition to
 * the full circle: <tt>[0°, 360°[</tt>.
 *
 * <b>a, b, Chroma (C)</b>
 * The maximum range for a, b and Chroma (C) is complex. It is <em>not</em>
 * limited by definition. A useful limit is the actual human perception.
 *
 * |                               |        a          |         b         |      C      |
 * | :---------------------------- |:----------------: | :---------------: | :---------: |
 * | Usual implementation¹         |    [−128, 127]    |    [−128, 127]    |             |
 * | Human perception (Wikipedia)² |    [−170, 100]    |    [−100, 150]    |             |
 * | Human perception (2° D50)³    | [−165.39, 129.05] | [−132.62, 146.69] | [0, 183.42] |
 * | Human perception (2° D65)³    | [−170.84, 147.84] | [−129.66, 146.78] | [0, 194.84] |
 * | Human perception (10° D65)³   | [−164.29, 115.14] | [−116.10, 145.53] | [0, 186.17] |
 *
 * 1. The range of  <tt>[−128, 127]</tt> is in C++ a signed 8‑bit integer. But
 *    this data type usually used in software implementations is (as the table
 *    clearly shows) not enough to cover the hole range of actual human
 *    color perception.
 * 2. Ranges of LAB coordinates according to the
 *    <a href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
 *    German Wikipedia</a>.
 * 3. The German association <em>Freie Farbe e. V.</em> has
 *    published a calculation of the
 *    <a href="https://www.freiefarbe.de/artikel/grenzen-des-cielab-farbraums/">
 *    shape of actual human perception</a> for various observation angles
 *    and illuminants. This data contains only the LAB coordinates. From
 *    this data, the C component can be calculated easily as Pythagoras of
 *    the a axis and b axis value pairs: √(a² + b²) = C. */

/** @page versioninfo Version information at compiletime and runtime
 *
 * This library uses
 * <a href="https://semver.org/">Semantic Versioning 2.0.0</a>.
 *
 * Version information is provided by the header <tt>version.h</tt>
 *
 * To know against which version of this library you are <em>running</em>, use
 * - @ref PerceptualColor::perceptualColorRunTimeVersion
 *
 * To know against which version of this library you are <em>compiling</em>,
 * use
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH */

/** @brief The namespace of this library.
 *
 * All symbols that are provided in this library are encapsulated within this
 * namespace. */
namespace PerceptualColor
{
} // namespace PerceptualColor
