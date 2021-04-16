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

#ifndef LCHVALUES_H
#define LCHVALUES_H

#include "PerceptualColor/lchdouble.h"

namespace PerceptualColor
{
/** @brief LCh default values
 *
 * The gamut of actual human perception within the LAB color model (and
 * its alternative representation LCH) has an irregular shape. Its maximum
 * extensions:
 *
 * - LAB/LCH lightness: <tt>[0, 100]</tt>
 * - LCH hue: The full circle, <tt>[0°, 360°[</tt>.
 *
 * |                               |        a         |         b         | C |
 * | :---------------------------  |:---------------: | :---------------: |
 * :---------: | | Usual implementation¹         |   [−128, 127]    |
 * [−128, 127]    |             | | Human perception (Wikipedia)² | [−170, 100]
 * |    [−100, 150]    |             | | Human perception (2° D50)³
 * |[−165.39, 129.05] | [−132.62, 146.69] | [0, 183.42] | | Human perception (2°
 * D65)³    |[−170.84, 147.84] | [−129.66, 146.78] | [0, 194.84] | | Human
 * perception (10° D65)³   |[−164.29, 115.14] | [−116.10, 145.53] | [0, 186.17]
 * |
 *
 * 1. The range of  <tt>[−128, 127]</tt> is in C++ a signed 8‑bit integer. But
 *    this data type usually used in software implementations is (as the table
 *    clearly shows) not enough to cover the hole range of actual human
 *    color perception.
 * 2. Ranges of LAB coordinates according to the
 *    <a
 * href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
 *    German Wikipedia</a>.
 * 3. The German association <em>Freie Farbe e. V.</em> has
 *    published a calculation of the
 *    <a href="https://www.freiefarbe.de/artikel/grenzen-des-cielab-farbraums/">
 *    shape of actual human perception</a> for various observation angles
 *    and illuminants. This data contains only the LAB coordinates. From
 *    this data, the C component can be calculated easily as Pythagoras of
 *    the a axis and b axis value pairs: √(a² + b²) = C.
 *
 * But what could be useful default values? This struct provides some
 * proposals. All values are <tt>constexpr</tt>.
 *
 * @note It is not always a good idea to have <tt>constexpr</tt> in header
 * files, as they are evaluated at compile time, so when linking dynamically
 * to a different library version that the compile time library version, this
 * could lead to confusioni. But this class is not part of the public API,
 * therefore <tt>constexpr</tt> are okay. */
struct LchValues final {
public:
    /** @brief Maximum chroma value of human perception.
     *
     * Following the table in the description of this class, the
     * maximum chroma in human perception is <tt>194.84</tt>. As aparently
     * this depends on viewing conditions, it might be a good idea
     * to use a slightly higher limit, to be sure that the value will
     * never be too small. Here, <tt>200</tt> is proposed as maximum
     * chroma of human perception. */
    static constexpr int humanMaximumChroma = 200;
    /** @brief Neutral chroma value
     *
     *  For chroma, a neutral value of 0 might be a good
     *  choice because it is less likely to make  out-of-gamut problems on
     *  any lightness (except maybe extreme white or extreme black). And
     *  it results in an achromatic color and is therefore perceived as
     *  neutral. */
    static constexpr int neutralChroma = 0;
    /** @brief Neutral hue value
     *
     *  For the hue, a default value of 0 might be used by convention. */
    static constexpr int neutralHue = 0;
    /** @brief Neutral lightness value
     *
     *  For the lightness, a neutral value of 50 seems a good choice as it
     *  is half the way in the defined lightness range of <tt>[0, 100]</tt>
     *  (thought not all gamuts offer the hole range of <tt>[0, 100]</tt>). As
     *  it is quite in the middle of the gamut solid, it allows for quite big
     *  values for chroma at different hues without falling out-of-gamut.
     *  Combined with a chroma of 0, it also approximates the color with
     *  the highest possible contrast against the hole surface of the
     *  gamut solid; this is interesting for background colors of
     *  gamut diagrams. */
    static constexpr int neutralLightness = 50;
    /** @brief Neutral gray color as Lab value.
     *
     * Neutral gray is a good choice for the background, as it is equally
     * distant from black and white, and also quite distant from any
     * saturated color. */
    static constexpr LchDouble neutralGray {neutralLightness, neutralChroma, neutralHue};
    /** @brief Maximum chroma value in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     *
     *  @sa @ref neutralChroma */
    static constexpr int srgbMaximumChroma = 132;
    /** @brief Versatile chroma value in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     *
     *  Depending on the use case, there might be an alternative to
     *  the neutral gray @ref neutralChroma. For a lightness of 50, this
     *  value is the maximum chroma available at all possible hues within
     *  a usual sRGB gamut.
     *  @sa @ref neutralChroma */
    static constexpr int srgbVersatileChroma = 32;

private:
    /** @brief Delete the constructor to disallow creating an instance
     * of this class. */
    LchValues() = delete;
};

}

#endif // LCHVALUES_H
