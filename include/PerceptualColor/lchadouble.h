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

#ifndef LCHADOUBLE_H
#define LCHADOUBLE_H

#include "PerceptualColor/perceptualcolorlib_global.h"

#include <QDebug>

namespace PerceptualColor {

/** @brief A LCH color with alpha channel.
 *
 * Storage of floating point LCH values with <tt>double</tt> precision.
 *
 * The data is not default-initializad; it is undefined when the object
 * is created.
 *
 * Example:
 * @snippet test/testLchaDouble.cpp Use LchaDouble
 *
 * @sa @ref LchValues explains more details about the valid
 * range.
 *
 * This class intentionally does not provide the operators <em>equal
 * to</em> (<tt>operator==</tt>) and <em>not equal to</em>
 * (<tt>operator!=</tt>). As LCH colors are polar coordinates,
 * there are various valid representations of the same angle.
 * And h is even meaningless when C is zero; on the other hand,
 * there might nevertheless be an interest in preserving h. And
 * invalid values with L=200 or L=300: Should they be equal because
 * both are invalid? Or are they different? The anser to all
 * these questions depends on your use case. To avoid confusion,
 * no coparision operators are provided by this class. See also
 * @ref hasSameCoordinates.
 *
 * This class is declared as type to Qt's type system:
 * <tt>Q_DECLARE_METATYPE(PerceptualColor::LchaDouble)</tt>.
 * Depending on your use case (for example if you want to use it
 * reliably in Qt's signals and slots), you might consider calling
 * <tt>qRegisterMetaType()</tt> for this type, once you have a QApplication
 * object.
 *
 * @todo We could normalize @ref LchaDouble values, just like @ref PolarPointF
 * also does. Performance should not matter for this use case! But: Does
 * it make sense? */
struct LchaDouble {
public:
    LchaDouble();
    LchaDouble(double newL, double newC, double newH, double newA);
    /** @brief Lightness, mesured in percent.
     *
     * The valid range is <tt>[0, 100]</tt>. */
    double l;
    /** @brief Chroma.
     *
     * <tt>0</tt> means no chroma (grayscale). The maximum
     * value depends on the gamut, for sRGB for example it’s
     *   @ref LchValues::srgbMaximumChroma. Other gamuts can
     *   be bigger, but the final limit is the gamut of human
     *   perception, out of which a Chroma value does not make
     *   sense: @ref LchValues::humanMaximumChroma */
    double c;
    /** @brief Hue, measured in degree.
     *
     * The valid range is <tt>[0, 360[</tt>. */
    double h;
    /** @brief Opacity (alpha channel)
     *
     * The valid range is <tt>[0, 1]</tt>. <tt>0</tt> is fully
     * transparent, <tt>1</tt> is fully opaque. */
    double a;
    bool hasSameCoordinates(const LchaDouble &other) const;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::LchaDouble &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::LchaDouble)

#endif // LCHADOUBLE_H
