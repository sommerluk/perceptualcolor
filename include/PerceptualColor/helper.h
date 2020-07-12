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

// TODO use forward-declarations instead of including too many headers?

#include <QImage>
#include <QVersionNumber>
#include <QWheelEvent>

class RgbColorSpace; // It seems that including the header isn't possible because the header itself depends on _this_ header.

#include <lcms2.h>

// TODO Does the OS-side color management change the RGB value we want to display? If so, how to prevent this?
// TODO Do only expose the headers that are absolutely necessary
// TODO Switch to pimpl?
// TODO QML integration?
// TODO Qt Designer support for the widgets

/** @brief The namespace of this library.
 * 
 * Everything that is provides in this library is encapsulated within this namespace.
 * 
 * The library uses generally @c int for integer values, because QSize() also QPoint() also do,
 * and the library relies heavily on the usage of QSize() and QPoint().
 * It uses generally @c qreal for floating point values, because QPointF() also does, for the
 * same reasons as above.
 * 
 * The source code of the library is in UTF8. A static_assert makes sure your
 * compiler actually treats it also as UTF8.
 * 
 * @todo Translations: Color picker -> Farbwähler etc…
 */
namespace PerceptualColor {

// Test if the compiler treats the source code actually as UTF8.
// A test string is converted to UTF8 code units (u8"") and each
// code unit is checked to be correct.
static_assert(
    (static_cast<quint8>(*((u8"🖌")+0)) == 0xF0) &&
        (static_cast<quint8>(*((u8"🖌")+1)) == 0x9F) &&
        (static_cast<quint8>(*((u8"🖌")+2)) == 0x96) &&
        (static_cast<quint8>(*((u8"🖌")+3)) == 0x8C) &&
        (static_cast<quint8>(*((u8"🖌")+4)) == 0x00),
    "This source code has to be read-in as UTF8 by the compiler."
);

/** @brief Various smaller help elements. */
namespace Helper {

    /** @brief Mesh size for gamut boundary search
     * 
     * This class can make sure that the color is within the gamut. To do so,
     * we have to search for the gamut boundary, because LittleCMS does not
     * provide a build-in function for this. When searching the nearest
     * in-gamut color for a given out-of-gamut color, the algorithm goes
     * step by step. Each step has the size gamutMeshSize. Once the first
     * in-gamut color is found, the algorithm refines more the precision between
     * the first in-gamut color and the last out-of-gamut color up to
     * gamutPrecision(), so gamutPrecision() has to be smaller than
     * gamutMeshSize().
     * 
     * This approach is taken because the gamut might be discontinous, so
     * a simple approach that just with each step takes the double or the
     * half of the distance, might miss some smaller patches of a discontinous
     * gamut. With gamutMeshSize() at least we can control the precision.
     * 
     * In short: Smaller values mean better precision and slower processing.
     * 
     * \sa gamutPrecision */
    static constexpr qreal gamutMeshSize = 0.01;
    /** @brief precision for gamut boundary search
     * 
     * For details, see gamutMeshSize() documentation. */
    static constexpr qreal gamutPrecision = 0.001;

    /** @brief Template function to test if a value is in a certain range
     * @param low the low limit
     * @param x the value that will be tested
     * @param high the high limit
     * @returns <tt>(low <= x) && (x <= high)</tt>
     */
    template<typename T> bool inRange(const T& low, const T& x, const T& high)
    {
        return ( (low <= x) && (x <= high) );
    }

    /** @brief Informations about L*a*b* gamut boundaries
     * 
     * According to the
     * <a href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
     * German Wikipedia</a>, in L*a*b* color space, in typical software implementations 
     * are used usually these ranges:
     * - Lightness axis: 0..100
     * - a axis: -128..127 (in C++ a signed 8 bit integer)
     * - b axis: -128..127 (in C++ a signed 8 bit integer)
     * 
     * According to the same Wikipedia article, the physical range for a and b axis
     * goes up to:
     * - Lightness axis: 0..100
     * - a axis: -170..100
     * - b axis: -100..150
     * 
     * in some situations.
     */
    struct LabBoundaries {
        static constexpr int physicalMinimumA = -170;
        static constexpr int physicalMaximumA = 100;
        static constexpr int physicalMinimumB = -100;
        static constexpr int physicalMaximumB = 150;
        static constexpr int usualMinimumA = -128;
        static constexpr int usualMaximumA = 127;
        static constexpr int usualMinimumB = -128;
        static constexpr int usualMaximumB = 127;
    };

    /** @brief Informations about LCh gamut boundaries
     *
     * Following @ref labBoundaries, calculating Pythagoras and using on each
     * axis (a and b) the most extreme value, the chroma value must definitively
     * be smaller than √((−170)² + 150²) ≈ 227 and will effectively be even
     * smaller.
     * 
     * For chroma, a default value of @c 0 might be a good choise because it is less likely to make 
     * out-of-gamut problems as it is really quite in the middle of the three-dimensional gamut body.
     * And it results in an achromatic, neutral color.
     * 
     * Depending on your use case, a versatile alternative might be @c 29 which is the highest chroma
     * that is just within sRGB gamut for a Lightness of @50 on all possible hues.
     * 
     * In sRGB, the maximum chroma value (as practically observed - I have no
     * mathematics to proof that) is 132.
     * 
     * For the lightness, a default value of @c 50 seems a good choise as it is half
     * the way in the defined lightness range <tt>0..100</tt>, and it also approximates the lightness
     * that offers the most highest possible chroma at different hues. This is less likely to make
     * out-of-gamut problems as it is really quite in the middle of the three-dimensional gamut body.
     * 
     * For hue, a default value of @c 0 can be used by convention.
     */
    struct LchBoundaries {
        static constexpr int physicalMaximumChroma = 227;
        static constexpr qreal defaultChroma = 0;
        static constexpr qreal defaultHue = 0;
        static constexpr qreal defaultLightness = 50;
        static constexpr qreal versatileSrgbChroma = 29;
        static constexpr qreal maxSrgbChroma = 132;
    };

    /** @brief An RGB color.
     * 
     * Storange of floating point in a practical format for working with LittleCMS. The range is 0..1
     */
    struct cmsRGB {
        /** The red value. */
        cmsFloat64Number red;
        /** The green value. */
        cmsFloat64Number green;
        /** The blue value. */
        cmsFloat64Number blue;
    };

    cmsCIELab toLab(const cmsCIELCh &lch);

    cmsCIELCh toLch(const cmsCIELab &lab);

    QImage transparencyBackground();

    QVersionNumber version();

    QPoint nearestNeighborSearch(const QPoint originalPoint, const QImage &image);

    qreal wheelSteps(QWheelEvent *event);
}

}

#endif // HELPER_H
