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

#ifndef HELPER_H
#define HELPER_H

#include <QDebug> // TODO Remove me!
#include <QImage>
#include <QtGlobal>
#include <QWheelEvent>

#include <lcms2.h>

/** @file
 * 
 * Declaration of the @ref PerceptualColor::Helper namespace and its members.
 * 
 * Also contains some general documentation for this library.
 * 
 * The source code of the library is in UTF8. A static_assert within the
 * header @ref helper.h makes sure your compiler actually treats it as UTF8.
 */
// Test if the compiler treats the source code actually as UTF8.
// A test string is converted to UTF8 code units (u8"") and each
// code unit is checked to be correct.
static_assert(
    (static_cast<quint8>(*((u8"🖌")+0)) == 0xF0)
        && (static_cast<quint8>(*((u8"🖌")+1)) == 0x9F)
        && (static_cast<quint8>(*((u8"🖌")+2)) == 0x96)
        && (static_cast<quint8>(*((u8"🖌")+3)) == 0x8C)
        && (static_cast<quint8>(*((u8"🖌")+4)) == 0x00),
    "This source code has to be read-in as UTF8 by the compiler."
);

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
 *      with Qt 5.7, Qt itself requires C++11. (Source:
 *      https://doc.qt.io/qt-5.9/cmake-manual.html#using-qt-5-with-cmake-older-than-3-1-0
 *      ). But our library uses C++11 features, for example “constexpr”. In
 *      the CMakeLists.txt file, we set -std=c++11 and -Wpedantic
 *      and -pedantic-errors to enforce C++11.
 * 
 *      Note that Qt 6 will require minimum C++17,
 *      as https://doc-snapshots.qt.io/qt6-dev/cmake-get-started.html explains.
 *      So that might be an option also for this library, if we
 *      decide to make it Qt-6-only. But it might
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
 * @copyright Almost all the code is published under MIT License. Only
 * <tt>cmake/Modules/FindLCMS2.cmake</tt> is licenced under BSD-3-Clause
 * license. The <tt>LICENSES</tt> subfolder contains copies of the licence
 * texts.
 * 
 * @todo Comply with KDE policies: https://community.kde.org/Policies
 * @todo Decide is it’s better to raise C++ requirement to C++17 (see the
 * hidden comment above in this source file).
 * @todo Remove all qDebug calls from the source
 * @todo Use QObject::tr() for translations. Provide po files.
 * @todo Qt6 property bindings (QProperty QPropertyBinding) for all
 * properties?
 * @todo Prepare for being ABI compatible in the
 * future. (This has to be done before 1.0.0 release. See
 * https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Note_about_ABI
 * and https://accu.org/journals/overload/18/100/love_1718/ for details.
 * The Pimpl-idiom / d-pointer-idiom: The class Car in car.h forward declares
 * in its private section the class CarPrivate (which will be defined in
 * another header file carprivate.h which is <em>not</em> included in car.h.
 * The implementation file car.cpp includes (and implements) both, car.h
 * <em>and</em> carprivate.h. Having separate headers allows for unit
 * testing, and also for polymorphy (if required – also people argue this
 * could be bad programming style). Now, a private member
 * pointerToImplementation will hold a pointer to the implementation object.
 * Attention: Now, const functions of the class Car can call non-const
 * function of the class CarPrivate, which is broken behaviour. We have
 * to provide a solution…
 * @todo Is Qt 5.6 actually enough?. Even if so, wouldn’t it
 * be better to require the last LTS release (5.15), just to be compatible if
 * in the future we depend on this? */

/** @brief The namespace of this library.
 * 
 * Everything that is provides in this library is encapsulated within this
 * namespace.
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

/** @brief Various smaller help elements.
 * 
 * This namespace groups together various smaller elements that are used
 * across the library but do not belong strictly to one of the classes.
 * 
 * @todo Decide for each member of this namespace if it can be moved into
 * a class because it’s only used in this single class.
 * @todo Maybe, get rid of this namespace completely, integrating the
 * available functionality into the abstract base classes? */
namespace Helper {

    /** @brief An RGB color.
     * 
     * Storage of floating point RGB values in a format that is practical
     * for working with <a href="http://www.littlecms.com/">LittleCMS</a>
     * (can be treated as buffer). The valid range for each component is 0‥1.
     * 
     * Example:
     * @snippet test/testhelper.cpp Helper Use cmsRGB */
    struct cmsRGB {
        /** @brief The red value. */
        cmsFloat64Number red;
        /** @brief The green value. */
        cmsFloat64Number green;
        /** @brief The blue value. */
        cmsFloat64Number blue;
    };

    /** @brief Precision for gamut boundary search
     * 
     * We have to search sometimes for the gamut boundary. This value defines
     * the precision of the search:  Smaller values mean better precision and
     * slower processing. */
    constexpr qreal gamutPrecision = 0.001;

    /** @brief Template function to test if a value is within a certain range
     * @param low the lower limit
     * @param x the value that will be tested
     * @param high the higher limit
     * @returns @snippet this Helper inRange */
    template<typename T> bool inRange(const T& low, const T& x, const T& high)
    {
        return (
            //! [Helper inRange]
            (low <= x) && (x <= high)
            //! [Helper inRange]
        );
    }

    /** @brief LCh default values
     * 
     * According to the
     * <a href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
     * German Wikipedia</a>, the Lab color space has the following ranges:
     * 
     * | Lab axis  | Usual software implementation | Actual human perception |
     * | :-------- | ----------------------------: | ----------------------: |
     * | lightness |                         0‥100 |                   0‥100 |
     * | a         |                      −128‥127 |                −170‥100 |
     * | b         |                      −128‥127 |                −100‥150 |
     * 
     * The range of −128‥127 is in C++ a signed 8‑bit integer. But this
     * data type usually used in software implementations is (as the table
     * clearly shows) not enough to cover the hole range of actual human color
     * perception.
     *
     * Unfortunately, there is no information about the LCh color space. But
     * we can deduce:
     * - @b Lightness: Same range as for Lab: <b>0‥100</b>
     * - @b Chroma: The Chroma value is the distance from the center of the
     *   coordinate system in the a‑b‑plane. Therefore it cannot be bigger
     *   than Pythagoras of the biggest possible absolute value on a axis and
     *   b axis: √[(−170)² + 150²] ≈ 227. Very likely the real range is
     *   smaller, but we do not know exactly how much. At least, we can be
     *   sure that this range is big enough: <b>0‥227</b>
     * - @b Hue: As the angle in a polar coordinate system, it has to
     *   be: <b>0°‥360°</b>
     * 
     * But what could be useful default values? This struct provides some
     * proposals. All values are <tt>constexpr</tt>. */
    struct LchDefaults {
        /** @brief Default chroma value
         * 
         *  For chroma, a default value of 0 might be a good
         *  choice because it is less likely to make  out-of-gamut problems on
         *  any lightness (except maybe extreme white or extreme black). And
         *  it results in an achromatic color and is therefore perceived as
         *  neutral.
         *  @sa @ref versatileSrgbChroma
         *  @sa @ref maxSrgbChroma */
        static constexpr qreal defaultChroma = 0;
        /** @brief Default hue value
         *
         *  For the hue, a default value of 0 might be used by convention. */
        static constexpr qreal defaultHue = 0;
        /** @brief Default lightness value
         * 
         *  For the lightness, a default value of 50 seems a good choice as it
         *  is half the way in the defined lightness range of 0‥100 (thought
         *  not all gamuts offer the hole range from 0 to 100). As it is quite
         *  in the middle of the gamut solid, it allows for quite big values
         *  for chroma at different hues without falling out-of-gamut.
         *  Together with a chroma of 0, it also approximates the color with
         *  the highest possible contrast against the hole surface of the
         *  gamut solid; this is interesting for background colors of
         *  gamut diagrams. */
        static constexpr qreal defaultLightness = 50;
        /** @brief Maximum chroma value in
         * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
         * sRGB gamut
         * 
         *  @sa @ref defaultChroma
         *  @sa @ref versatileSrgbChroma */
        static constexpr qreal maxSrgbChroma = 132;
        /** @brief Versatile chroma value in
         * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
         * sRGB gamut
         * 
         *  Depending on the use case, there might be an alternative to
         *  the neutral gray defaultChroma(). For a lightness of 50, this
         *  value is the maximum chroma available at all possible hues within
         *  a usual sRGB gamut.
         *  @sa @ref defaultChroma
         *  @sa @ref maxSrgbChroma */
        static constexpr qreal versatileSrgbChroma = 32;
        /** @brief Neutral gray color as Lab value.
         * 
         * Neutral gray is a good choice for the background, as it is equally
         * distant from black and white, and also quite distant from any
         * saturated color. */
        static constexpr cmsCIELCh neutralGray {50, 0, 0};
    };

    qreal standardWheelSteps(QWheelEvent *event);

}

}

#endif // HELPER_H
