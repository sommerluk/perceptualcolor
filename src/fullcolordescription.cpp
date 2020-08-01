/*
 * Copyright 2020 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// own header
#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/polarpointf.h"

// other includes
// #include "PerceptualColor/polarpointf.h"

namespace PerceptualColor {

/** @brief Constructor for an invalid object. */
FullColorDescription::FullColorDescription()
{
    m_valid = false;
}

// TODO The LCh-hue (and so the graphical widgets) jumps forward and backward
// when changing RGB-based values (also HSV) when entering and leaving the gray
// axis, due to lack of hue information. Would it be an option to accept an
// old/previous FullColorDescription (for the last selected value before
// entering the gray axis) as additional parameter to get meaningfull hue?
// Should it be only really for the gray axis, or allow a certain tolerance
// around the gray axis is necessary to make this work well - and if so,
// how much tolerance? Would it be useful to define a certain hue, for
// example 0°, as default hue for when no old hue is available but the
// new value is on the gray axis?

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param rgb rgb color
 */
FullColorDescription::FullColorDescription(RgbColorSpace *colorSpace, const Helper::cmsRGB &rgb, qreal alpha)
{
    m_rgb = rgb;
    m_rgbQColor = QColor::fromRgbF(m_rgb.red, m_rgb.green, m_rgb.blue, alpha);
    m_hsvQColor = m_rgbQColor.toHsv();
    m_lab = colorSpace->colorLab(rgb);
    m_lch = toLch(m_lab);
    m_alpha = alpha;
    m_rgbQColor.setAlphaF(alpha);
    m_hsvQColor.setAlphaF(alpha);
    m_valid = true;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param color either an rgb color, a hsv color or an invalid color. (If it is
 * another type, it will be converted.)
 */
FullColorDescription::FullColorDescription(RgbColorSpace *colorSpace, QColor color)
{
    if (!color.isValid()) {
        m_valid = false;
        return;
    }
    switch (color.spec()) {
        case QColor::Spec::Hsv:
            m_hsvQColor = color;
            m_rgbQColor = color.toRgb();
            break;
        case QColor::Spec::Rgb:
            m_rgbQColor = color;
            m_hsvQColor = color.toHsv();
            break;
        default:
            m_rgbQColor = color.toRgb();
            m_hsvQColor = color.toHsv();
            break;
    }
    m_rgb.red = m_rgbQColor.redF();
    m_rgb.green = m_rgbQColor.greenF();
    m_rgb.blue = m_rgbQColor.blueF();
    m_lab = colorSpace->colorLab(m_rgbQColor);
    m_lch = toLch(m_lab);
    m_alpha = color.alphaF();
    m_valid = true;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param lab lab color (if out-of-gamut, it will be maintained as-is, but the
 * RGB value will be forced into the gamut.
 */
FullColorDescription::FullColorDescription(RgbColorSpace *colorSpace, const cmsCIELab &lab, outOfGamutBehaviour behaviour, qreal alpha)
{
    m_lch = toLch(lab);
    normalizeLch();
    if (behaviour == outOfGamutBehaviour::sacrifyChroma) {
        moveChromaIntoGamut(colorSpace);
    }
    m_lab = toLab(m_lch);
    m_rgb = colorSpace->colorRgbBoundSimple(m_lab);
    m_rgbQColor = QColor::fromRgbF(m_rgb.red, m_rgb.green, m_rgb.blue, alpha);
    m_hsvQColor = m_rgbQColor.toHsv();
    m_alpha = alpha;
    m_valid = true;
}

/** @brief Constructor
 *
 * @param colorSpace The color space in which the color description is created.
 * Only needed during constructor call. Can be deleted afterwards.
 * @param lch lch color (if out-of-gamut, it will be maintained as-is, but the
 * RGB value will be forced into the gamut.
 */
FullColorDescription::FullColorDescription(
    RgbColorSpace *colorSpace,
    const cmsCIELCh &lch,
    outOfGamutBehaviour behaviour,
    qreal alpha
)
{
    m_lch = lch;
    normalizeLch();
    if (behaviour == outOfGamutBehaviour::sacrifyChroma) {
        moveChromaIntoGamut(colorSpace);
    }
    m_lab = toLab(m_lch);
    m_rgb = colorSpace->colorRgbBoundSimple(m_lab);
    m_rgbQColor = QColor::fromRgbF(m_rgb.red, m_rgb.green, m_rgb.blue, alpha);
    m_hsvQColor = m_rgbQColor.toHsv();
    m_alpha = alpha;
    m_rgbQColor.setAlphaF(alpha);
    m_hsvQColor.setAlphaF(alpha);
    m_valid = true;
}

/** Makes sure that m_lch() will be within the gamut.
 * Implements outOfGamutBehaviour::sacrifyChroma */
void FullColorDescription::moveChromaIntoGamut(RgbColorSpace *colorSpace)
{
    // Test special case: If we are yet in-gamut…
    if (colorSpace->inGamut(m_lch)) {
        return;
    }

    // Now we know: We are out-of-gamut…
    cmsCIELCh lowerChroma {m_lch.L, 0, m_lch.h};
    cmsCIELCh upperChroma {m_lch};
    cmsCIELCh candidate;
    if (colorSpace->inGamut(lowerChroma)) {
        // Now we know for sure that lowerChroma is in-gamut and upperChroma is out-of-gamut…
        candidate = upperChroma;
        while (upperChroma.C - lowerChroma.C > Helper::gamutPrecision) {
            // Our test candidate is half the way between lowerChroma and upperChroma:
            candidate.C = (
                (lowerChroma.C + upperChroma.C) / 2
            );
            if (colorSpace->inGamut(candidate)) {
                lowerChroma = candidate;
            } else {
                upperChroma = candidate;
            }
        }
        m_lch = lowerChroma;
    } else {
        if (m_lch.L < colorSpace->blackpointL()) {
            m_lch.L = colorSpace->blackpointL();
            m_lch.C = 0;
        } else {
            if (m_lch.L > colorSpace->whitepointL()) {
                m_lch.L = colorSpace->blackpointL();
                m_lch.C = 0;
            }
        }
    }
}


void FullColorDescription::setAlpha(qreal alpha)
{
    m_alpha = alpha;
    m_hsvQColor.setAlphaF(alpha);
    m_rgbQColor.setAlphaF(alpha);
}

/**
 * @returns true if equal, otherwise false.
 */
bool FullColorDescription::operator==(const FullColorDescription& other) const
{
    return (
        (m_rgb.red == other.m_rgb.red) &&
        (m_rgb.green == other.m_rgb.green) &&
        (m_rgb.blue == other.m_rgb.blue) &&
        (m_lab.L == other.m_lab.L) &&
        (m_lab.a == other.m_lab.a) &&
        (m_lab.b == other.m_lab.b) &&
        (m_lch.L == other.m_lch.L) &&
        (m_lch.C == other.m_lch.C) &&
        (m_lch.h == other.m_lch.h) &&
        (m_alpha == other.m_alpha) &&
        (m_rgbQColor == other.m_rgbQColor) &&
        (m_hsvQColor == other.m_hsvQColor) &&
        (m_valid == other.m_valid)
    );
}


/**
 * @returns true if unequal, otherwise false.
 */
bool FullColorDescription::operator!=(const FullColorDescription& other) const
{
    return !(*this == other);
}

/**
 * @returns if this object is valid, the color as RGB, otherwise an arbitrary value
 */
Helper::cmsRGB FullColorDescription::toRgb() const
{
    return m_rgb;
}

/**
 * @returns QColor object corresponding at rgb()
 */
QColor FullColorDescription::toRgbQColor() const
{
    return m_rgbQColor;
}


/**
 * @returns QColor object corresponding at hsv
 */
QColor FullColorDescription::toHsvQColor() const
{
    return m_hsvQColor;
}

/**
 * @returns if this object is valid, the color as Lab, otherwise an arbitrary value
 */
cmsCIELab FullColorDescription::toLab() const
{
    return m_lab;
}

/**
 * @returns if this object is valid, the color as LCh, otherwise an arbitrary value
 */
cmsCIELCh FullColorDescription::toLch() const
{
    return m_lch;
}

/**
 * @returns if this object is valid, the alpha channel, otherwise an arbitrary value. 0 is fully transparent, 1 is fully opaque.
 */
qreal FullColorDescription::alpha() const
{
    return m_alpha;
}

/**
 * @returns true if this object is valid, false otherwise.
 */
bool FullColorDescription::isValid() const
{
    return m_valid;
}

/** @brief Adds QDebug() support for this data type. */
QDebug operator<<(QDebug dbg, const PerceptualColor::FullColorDescription &value)
{
    dbg.nospace() << "FullColorDescription(\n"
        << " - RGB: " << value.toRgb().red << " " << value.toRgb().green << " " << value.toRgb().blue << "\n"
        << " - RGBQColor: " << value.toRgbQColor() << "\n"
        << " - HSVQColor: " << value.toHsvQColor() << "\n"
        << " - Lab: " << value.toLab().L << " " << value.toLab().a << " " << value.toLab().b << "\n"
        << " - LCh: " << value.toLch().L << " " << value.toLch().C << " " << value.toLch().h << "°\n"
        << " - Alpha: " << value.alpha() << "\n"
        << ")";
    return dbg.maybeSpace();
}

/** Normalizes m_lch. */
void FullColorDescription::normalizeLch()
{
    PolarPointF temp(m_lch.C, m_lch.h);
    m_lch.C = temp.radial();
    m_lch.h = temp.angleDegree();
}

/** @brief  A string with a hexadecimal representation of the color.
 * 
 * This function is similar to QColor::name(), but provides correct rounding.
 * @returns A string of the form @em #RRGGBB with the red (R), green (G) and
 * blue (B) values represented as two hexadecimal digits in the range 00⁠–⁠FF.
 * The digits A–⁠F are always upper-case.
 */
QString FullColorDescription::toRgbHexString() const
{
    // Format of the numbers:
    // - Minimal field width:                   2
    // - The base of the number represenation:  16 (hexadecimal)
    // - The fill character (leading zero):     QLatin1Char('0')
    return QStringLiteral(u"#%1%2%3")
        .arg(qRound(m_rgb.red   * 255), 2, 16, QLatin1Char('0'))
        .arg(qRound(m_rgb.green * 255), 2, 16, QLatin1Char('0'))
        .arg(qRound(m_rgb.blue  * 255), 2, 16, QLatin1Char('0'))
        .toUpper();
}

/** @brief Convert to LCh
 * 
 * @param lab a point in Lab representation
 * @returns the same point in LCh representation */
cmsCIELCh FullColorDescription::toLch(const cmsCIELab &lab)
{
    cmsCIELCh temp;
    cmsLab2LCh(&temp, &lab);
    return temp;
}

/** @brief Convert to Lab
 * 
 * @param lch a point in LCh representation
 * @returns the same point in Lab representation */
cmsCIELab FullColorDescription::toLab(const cmsCIELCh &lch)
{
    cmsCIELab temp;
    cmsLCh2Lab(&temp, &lch);
    return temp;
}

// TODO Isn't it inconsistent if toRgbHexString is gererated on-the-fly while all others are generated previously?

} // namespace PerceptualColor
