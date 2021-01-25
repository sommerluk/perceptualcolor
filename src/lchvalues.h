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

#ifndef LCHVALUES_H
#define LCHVALUES_H

#include "PerceptualColor/lchdouble.h"

namespace PerceptualColor {

/** @brief LCh default values
 * 
 * The gamut of actual human perception within the LAB color model (and
 * its alternative representation LCH) has an irregular shape. Its maximum
 * extensions:
 * 
 * |                                      |   L   |        a       |        b       |    C     |    H    |
 * | :----------------------------------  | :---: | :------------: | :------------: | :------: | :-----: |
 * | Usual software implementation¹       | 0‥100 |    −128‥127    |    −128‥127    |          | 0°‥360° |
 * | Human perception (German Wikipedia)² | 0‥100 |    −170‥100    |    −100‥150    |          | 0°‥360° |
 * | Human perception (2° D50)³           | 0‥100 | −165.39‥129.05 | −132.62‥146.69 | 0‥183.42 | 0°‥360° |
 * | Human perception (2° D65)³           | 0‥100 | −170.84‥147.84 | −129.66‥146.78 | 0‥194.84 | 0°‥360° |
 * | Human perception (10° D65)³          | 0‥100 | −164.29‥115.14 | −116.10‥145.53 | 0‥186.17 | 0°‥360° |
    *
 * 1. The range of −128‥127 is in C++ a signed 8‑bit integer. But this
 *    data type usually used in software implementations is (as the table
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
 *    the a axis and b axis value pairs: √[a² + b²] = C. (However, in
 *    practice, might it be better to use 255 as maximum chroma for
 *    usability reasons?)
    *
 * But what could be useful default values? This struct provides some
 * proposals. All values are <tt>constexpr</tt>.  */
struct LchValues final {
public:
    /** @brief Default chroma value
     * 
     *  For chroma, a default value of 0 might be a good
     *  choice because it is less likely to make  out-of-gamut problems on
     *  any lightness (except maybe extreme white or extreme black). And
     *  it results in an achromatic color and is therefore perceived as
     *  neutral. */
    static constexpr int defaultChroma = 0;
    /** @brief Default hue value
        *
     *  For the hue, a default value of 0 might be used by convention. */
    static constexpr int defaultHue = 0;
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
    static constexpr int defaultLightness = 50;
    /** @brief Maximum chroma value of human perception.
     * 
     * Following the table in the description of this class, the
     * maximum chroma in human perception is 194.84. As aparently
     * this depends on viewing conditions, it might be a good idea
     * to use a slightly higher limit, to be sure that the value is
     * not too small. Here, <tt>200</tt> is proposed as maximum
     * chroma of human perception. */
    static constexpr int humanMaximumChroma = 200;
    /** @brief Neutral gray color as Lab value.
     * 
     * Neutral gray is a good choice for the background, as it is equally
     * distant from black and white, and also quite distant from any
     * saturated color. */
    static constexpr LchDouble neutralGray {defaultLightness, 0, 0};
    /** @brief Maximum chroma value in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     * 
     *  @sa @ref defaultChroma */
    static constexpr int srgbMaximumChroma = 132;
    /** @brief Versatile chroma value in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     * 
     *  Depending on the use case, there might be an alternative to
     *  the neutral gray defaultChroma(). For a lightness of 50, this
     *  value is the maximum chroma available at all possible hues within
     *  a usual sRGB gamut.
     *  @sa @ref defaultChroma */
    static constexpr int srgbVersatileChroma = 32;
private:
    /** @brief Delete the constructor to disallow creating an instance
     * of this class. */
    LchValues() = delete;
};

}

#endif // LCHVALUES_H
