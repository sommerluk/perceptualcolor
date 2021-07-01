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

// Own header
#include "multicolor.h"

namespace PerceptualColor
{
/** @brief Constructor for an uninitialized object.
 *
 * @note This constructor is quite useless except for declaring variables
 * of this type. Use the static functions to get an actual color object. */
MultiColor::MultiColor()
{
    m_lch.l = 0;
    m_lch.c = 0;
    m_lch.h = 0;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color LCH color
 * @returns A @ref MultiColor object representing this color.
 * @note The color will neither be normalised nor moved into gamut. If it’s
 * an out-of-gamut color, the resulting @ref toRgbQColor will obviously have
 * an incorrect color. */
MultiColor MultiColor::fromLch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color)
{
    MultiColor result;
    result.m_lch = color;
    result.m_rgbQColor = colorSpace->toQColorRgbBound(color);
    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color RGB color
 * @returns A @ref MultiColor object representing this color.
 * @note The resulting @ref toLch is guaranteed
 * to be @ref RgbColorSpace::isInGamut. */
MultiColor MultiColor::fromRgbQColor(const QSharedPointer<RgbColorSpace> &colorSpace, const QColor &color)
{
    MultiColor result;
    result.m_rgbQColor = color;
    result.m_lch = colorSpace->nearestInGamutColorByAdjustingChromaLightness(
        // TODO Adjust not only C and L, but also H?
        colorSpace->toLch(color));
    return result;
}

/** @brief Equal operator
 *
 * @returns <tt>true</tt> if all data members have the same coordinates.
 * <tt>false</tt> otherwise. */
bool MultiColor::operator==(const MultiColor &other) const
{
    return (
        // Test equality for all data members
        m_lch.hasSameCoordinates(other.m_lch) && (m_rgbQColor == other.m_rgbQColor));
}

/** @brief QColor object with the RGB values
 * @returns QColor object with the RGB values */
QColor MultiColor::toRgbQColor() const
{
    return m_rgbQColor;
}

/** @brief LCH values
 * @returns LCH values
 * @sa @ref toHlc */
LchDouble MultiColor::toLch() const
{
    return m_lch;
}

/** @brief HCL values
 *
 * Convenience function that provedes the same value
 * as @ref toLch, but as a different data type.
 *
 * @returns HCL values */
QList<double> MultiColor::toHlc() const
{
    return QList<double> {//
                          m_lch.h,
                          m_lch.l,
                          m_lch.c};
}

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::MultiColor
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value)
{
    dbg.nospace() //
        << "MultiColor(\n"
        << " - RGBQColor: " << value.toRgbQColor() << "\n"
        << " - LCH: " << value.toLch() << "\n"
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_standard_layout_v<MultiColor>);

} // namespace PerceptualColor
