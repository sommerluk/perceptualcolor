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

#include "helper.h"
#include "polarpointf.h"

namespace PerceptualColor
{
/** @brief Constructor for an invalid object. */
MultiColor::MultiColor()
{
    m_isValid = false;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param rgb color (in RGB mode)
 * @param alpha the alpha channel for color */
MultiColor::MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, const PerceptualColor::RgbDouble &rgb, qreal alpha)
    : m_rgbColorSpace(colorSpace)
{
    //     m_rgb = rgb;
    //     m_rgbQColor = QColor::fromRgbF(m_rgb.red, m_rgb.green, m_rgb.blue, alpha);
    //     m_hsvQColor = m_rgbQColor.toHsv();
    //     m_lab = colorSpace->colorLab(rgb);
    //     m_lch = toLch(m_lab);
    //     m_alpha = alpha;
    //     m_rgbQColor.setAlphaF(alpha);
    //     m_hsvQColor.setAlphaF(alpha);
    //     m_valid = true;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is
 * created. Only needed during constructor call. Can be deleted afterwards.
 * @param color either an RGB color, a HSV color or an invalid color. (If it
 * is another type, it will be converted.)
 */
MultiColor::MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, QColor color)
    : m_rgbColorSpace(colorSpace)
{
    //     switch (color.spec()) {
    //     case QColor::Spec::Invalid:
    //         m_valid = false;
    //         return;
    //     case QColor::Spec::Hsv:
    //         m_hsvQColor = color;
    //         m_rgbQColor = color.toRgb();
    //         break;
    //     case QColor::Spec::Rgb:
    //         m_rgbQColor = color;
    //         m_hsvQColor = color.toHsv();
    //         break;
    //     case QColor::Spec::Cmyk:
    //     case QColor::Spec::Hsl:
    //     default:
    //         m_rgbQColor = color.toRgb();
    //         m_hsvQColor = color.toHsv();
    //         break;
    //     }
    //     m_rgb.red = m_rgbQColor.redF();
    //     m_rgb.green = m_rgbQColor.greenF();
    //     m_rgb.blue = m_rgbQColor.blueF();
    //     m_lab = colorSpace->colorLab(m_rgbQColor);
    //     m_lch = toLch(m_lab);
    //     m_alpha = color.alphaF();
    //     m_valid = true;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param lch lch color (if out-of-gamut, it will be maintained as-is, but the
 * RGB value will be forced into the gamut.
 * @param coordinates how to tread out-of-gamut values
 * @param alpha the alpha channel
 */
MultiColor::MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &lch, OutOfGamutBehaviour coordinates, qreal alpha)
    : m_rgbColorSpace(colorSpace)
{
    //     m_lch = lch;
    //     normalizeLch();
    //     if (coordinates == OutOfGamutBehaviour::sacrifyChroma) {
    //         moveChromaIntoGamut(colorSpace);
    //     }
    //     m_rgb = colorSpace->colorRgbBoundSimple(m_lch);
    //     m_rgbQColor = QColor::fromRgbF(m_rgb.red, m_rgb.green, m_rgb.blue, alpha);
    //     m_hsvQColor = m_rgbQColor.toHsv();
    //     m_alpha = alpha;
    //     m_rgbQColor.setAlphaF(alpha);
    //     m_hsvQColor.setAlphaF(alpha);
    //     m_valid = true;
}

QSharedPointer<RgbColorSpace> MultiColor::rgbColorSpace() const
{
    return m_rgbColorSpace;
}

void MultiColor::setAlpha(qreal alpha)
{
    m_alpha = alpha;
    m_hsvQColor.setAlphaF(alpha);
    m_rgbQColor.setAlphaF(alpha);
}

/**
 * @returns true if equal, otherwise false.
 */
bool MultiColor::operator==(const MultiColor &other) const
{
    return (
        // Compare RGB components
        (m_rgb.red == other.m_rgb.red) && (m_rgb.green == other.m_rgb.green) &&
        (m_rgb.blue == other.m_rgb.blue)
        // Compare LCH components
        && (m_lch.l == other.m_lch.l) && (m_lch.c == other.m_lch.c) &&
        (m_lch.h == other.m_lch.h)
        // Compare alpha
        && (m_alpha == other.m_alpha)
        // Compare RGB QColor
        && (m_rgbQColor == other.m_rgbQColor)
        // Compare HSV QColor
        && (m_hsvQColor == other.m_hsvQColor)
        // Compare validity
        && (m_isValid == other.m_isValid));
}

/**
 * @returns true if unequal, otherwise false. */
bool MultiColor::operator!=(const MultiColor &other) const
{
    return !(*this == other);
}

/**
 * @returns if this object is valid, the color as RGB, otherwise an
 * arbitrary value */
RgbDouble MultiColor::toRgb() const
{
    return m_rgb;
}

/**
 * @returns QColor object corresponding at rgb() */
QColor MultiColor::toRgbQColor() const
{
    return m_rgbQColor;
}

/**
 * @returns QColor object corresponding at HSV */
QColor MultiColor::toHsvQColor() const
{
    return m_hsvQColor;
}

/**
 * @returns if this object is valid, the color as LCh, otherwise an
 * arbitrary value */
LchDouble MultiColor::toLch() const
{
    return m_lch;
}

/**
 * @returns if this object is valid, the alpha channel, otherwise an arbitrary
 * value. 0 is fully transparent, 1 is fully opaque.
 *
 * @todo Rename this to alphaF() for consistence with QColor? Do the same
 * thing for other field of this class?
 */
qreal MultiColor::alpha() const
{
    return m_alpha;
}

/**
 * @returns true if this object is valid, false otherwise.
 */
bool MultiColor::isValid() const
{
    return m_isValid;
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
        << " - RGB: " << value.toRgb().red << " " << value.toRgb().green << " " << value.toRgb().blue << "\n"
        << " - RGBQColor: " << value.toRgbQColor() << "\n"
        << " - HSVQColor: " << value.toHsvQColor() << "\n"
        << " - LCh: " << value.toLch().l << " " << value.toLch().c << " " << value.toLch().h << "°\n"
        << " - Alpha: " << value.alpha() << "\n"
        << ")";
    return dbg.maybeSpace();
}

/** Normalizes m_lch. */
void MultiColor::normalizeLch()
{
    PolarPointF temp(m_lch.c, m_lch.h);
    m_lch.c = temp.radial();
    m_lch.h = temp.angleDegree();
}

/** @brief  A string with a hexadecimal representation of the color.
 *
 * This function is similar to QColor::name(), but provides correct rounding.
 * @returns A string of the form <em>\#RRGGBB</em> with the red (R), green (G)
 * and blue (B) values represented as two hexadecimal digits in the range
 * 00‑FF. The digits A‑⁠F are always upper-case.
 */
QString MultiColor::toRgbHexString() const
{
    // Format of the numbers:
    // - Minimal field width:                   2
    // - The base of the number representation:  16 (hexadecimal)
    // - The fill character (leading zero):     QLatin1Char('0')
    return QStringLiteral(u"#%1%2%3")
        // RGB components
        .arg(qRound(m_rgb.red * 255), 2, 16, QLatin1Char('0'))
        .arg(qRound(m_rgb.green * 255), 2, 16, QLatin1Char('0'))
        .arg(qRound(m_rgb.blue * 255), 2, 16, QLatin1Char('0'))
        // Convert to upper case
        .toUpper();
}

static_assert(std::is_standard_layout_v<MultiColor>);

// TODO Isn’t it inconsistent if toRgbHexString is generated on-the-fly
// while all others are generated previously?

} // namespace PerceptualColor
