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

#ifndef LCHDOUBLE_H
#define LCHDOUBLE_H

#include "PerceptualColor/perceptualcolorlib_global.h"

#include <QDebug>

namespace PerceptualColor {

/** @brief A LCH color.
 *
 * Storage of floating point LCH values with <tt>double</tt> precision.
 *
 * The data is not default-initializad; it is undefined when the object
 * is created.
 *
 * Example:
 * @snippet test/testlchdouble.cpp Use LchDouble
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
 * <tt>Q_DECLARE_METATYPE(PerceptualColor::LchDouble)</tt>.
 * Depending on your use case (for example if you want to use it
 * reliably in Qt's signals and slots), you might consider calling
 * <tt>qRegisterMetaType()</tt> for this type, once you have a QApplication
 * object.
 *
 * @todo Would it make sense to normalize the hue (1° instead
 * of 361°, and only non-negatif radials) and/or to bound the
 * @ref l component to the range <tt>[0, 100]</tt> and/or
 * to bound the radial to the range <tt>[0, 255]</tt> or
 * <tt>[0, @ref LchValues::humanMaximumChroma]</tt>? */
struct LchDouble {
public:
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
    bool hasSameCoordinates(const LchDouble &other) const;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::LchDouble &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::LchDouble)

#endif // LCHDOUBLE_H
