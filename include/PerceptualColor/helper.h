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

#include <QImage>
#include <QWheelEvent>

#include <lcms2.h>

/** @brief The namespace of this library.
 * 
 * Everything that is provides in this library is encapsulated within this
 * namespace.
 * 
 * The library provides various Qt GUI components for choosing colors, with
 * focus on an intuitive and perceptually uniform presentation. The GUI
 * widgets are based internally on the LCh color model, which does reflect
 * the human perceptuan much better than RGB or its transforms like HSV.
 * However, the widgets do not require the user itself to know anything
 * about LCh at all, because the graphical representations tend to be
 * intuitive enough.
 * 
 * The library depends on (and therefore you have to link against) Qt Core,
 * Qt Gui, Qt Widgets (Qt 5) and LittleCMS 2.
 * 
 * The library uses in general @c int for integer values, because QSize() and
 * QPoint() also do. As the library relies heavily on the usage of QSize()
 * and QPoint(), this seems reasonable. For the same reason, it uses generally
 * @c qreal for floating point values, because QPointF() also does.
 * 
 * The source code of the library is in UTF8. A static_assert within the
 * header @c helper.h makes sure your compiler actually treats it as UTF8.
 * 
 * @todo Translations: Color picker/Select Color → Farbwähler/Farbauswahl etc…
 * @todo Does the OS-side color management change the RGB value we want to
 * display? If so, how to prevent this?
 * @todo Only expose in the headers and in the public API what is absolutely
 * necessary.
 * @todo Switch to the pimpl ideom?
 * @todo Qt Designer support for the widgets */
namespace PerceptualColor {

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

/** @brief Various smaller help elements.
 * 
 * This namespace groups together various smaller elements that are used
 * across the library but do not belong stricly to one of the classes.
 * 
 * @todo Decide for each member of this namespace if it can be moved into
 * a class because it’s only used in one single class. */
namespace Helper {

    /** @brief An RGB color.
     * 
     * Storage of floating point RGB values in a format that is practical
     * for working with LittleCMS (can be treated as buffer). The valid
     * range for each component is 0‥1.
     * 
     * Example:
     * @code
     * PerceptualColor::Helper::cmsRGB rgb;
     * rgba.red = 1;
     * rgba.green = 0;
     * rgba.blue = 0;
     * cmsCIELab lab;
     * // Convert exactly 1 value:
     * cmsDoTransform(m_transformRgbToLabHandle, &rgb, &lab, 1);
     * @endcode */
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
     * @returns <tt>(low <= x) && (x <= high)</tt>
     */
    template<typename T> bool inRange(const T& low, const T& x, const T& high)
    {
        return ( (low <= x) && (x <= high) );
    }

    /** @brief LCh default values
     * 
     * According to the
     * <a href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
     * German Wikipedia</a>, the Lab color space has the following ranges:
     * 
     * | Lab axis  | Usual software implementation | Actual human perception |
     * | :-------- | ----------------------------: | ----------------------: |
     * | lightness |                        0..100 |                   0‥100 |
     * | a         |                      −128‥127 |                −170‥100 |
     * | b         |                      −128‥127 |                −100‥150 |
     * 
     * The range of −128‥127 is in C++ a signed 8‑bit integer. But this
     * data type usually used in software implementations is obviously not
     * enough to cover the hole range of actual human color perception.
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
     * values as proposals. All values are <tt>constexpr</tt>. */
    struct LchDefaults {
        /** @brief Default chroma value
         * 
         *  For chroma, a default value of 0 might be a good
         *  choise because it is less likely to make  out-of-gamut problems on
         *  any lightness (except maybe extreme white or extreme black). And
         *  it results in an achromatic color and is therefore perceived as
         *  neutral.
         *  @sa versatileSrgbChroma
         *  @sa maxSrgbChroma */
        static constexpr qreal defaultChroma = 0;
        /** @brief Default hue value
         *
         *  For the hue, a default value of 0 might be used by convention. */
        static constexpr qreal defaultHue = 0;
        /** @brief Default lightness value
         * 
         *  For the lightness, a default value of 50 seems a good
         *  choise as it is half the way in the defined lightness range
         *  <tt>0‥100</tt> (thought not all gamuts offer the hole range from 0
         *  to 100). As it is quite in the middle of the gamut solid, it
         *  allows for quite extreme values for Chroma and Hue without falling
         *  out-of-gamut. Together with a chroma of @c 0, it also approximates
         *  color with the highest possible contrast against the hole surface
         *  of the gamut solid; this is interesting for background colors of
         *  twodimensional gamut diagrams. */
        static constexpr qreal defaultLightness = 50;
        /** @brief Maximum chroma value in LittleCMS' build-in sRGB gamut
         * 
         *  @sa defaultChroma
         *  @sa versatileSrgbChroma */
        static constexpr qreal maxSrgbChroma = 132;
        /** @brief Versatile chroma value in LittleCMS' build-in sRGB gamut
         * 
         *  Depending on the use case, there might be an alternative to
         *  the neutral gray defaultChroma(). For a lightness of 50, this
         *  value is the maximum chroma available at all possible hues within
         *  the sRGB gamut.
         *  @sa defaultChroma
         *  @sa maxSrgbChroma */
        static constexpr qreal versatileSrgbChroma = 32;
    };

    QImage transparencyBackground();

    qreal standardWheelSteps(QWheelEvent *event);

}

}

#endif // HELPER_H
