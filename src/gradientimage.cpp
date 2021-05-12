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

// Own headers
// First the interface, which forces the header to be self-contained.
#include "gradientimage.h"

#include <math.h>

#include "helper.h"

#include <QPainter>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which the image should operate.
 * Can be created with @ref RgbColorSpaceFactory. */
GradientImage::GradientImage(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_rgbColorSpace(colorSpace)
{
    setFirstColor(LchaDouble(0, 0, 0, 1));
    setFirstColor(LchaDouble(1000, 0, 0, 1));
}

/** @brief Normalizes the value and bounds it to the LCH color space.
 * @param color the color that should be treated.
 * @returns A normalized and bounded version. If the chroma was negative,
 * it gets positive (which implies turning the hue by 180°). The hue is
 * normalized to the range <tt>[0°, 360°[</tt>. Lightness is bounded to the
 * range <tt>[0, 100]</tt>. Alpha is bounded to the range <tt>[0, 1]</tt>. */
LchaDouble GradientImage::completlyNormalizedAndBounded(const LchaDouble &color)
{
    LchaDouble result;
    if (color.c < 0) {
        result.c = color.c * (-1);
        result.h = fmod(color.h + 180, 360);
    } else {
        result.c = color.c;
        result.h = fmod(color.h, 360);
    }
    if (result.h < 0) {
        result.h += 360;
    }
    result.l = qBound<qreal>(0, color.l, 100);
    result.a = qBound<qreal>(0, color.a, 1);
    return result;
}

/** @brief Setter for the first color property.
 * @param newFirstColor The new first color.
 * @sa @ref m_firstColorCorrected */
void GradientImage::setFirstColor(const LchaDouble &newFirstColor)
{
    LchaDouble correctedNewFirstColor = completlyNormalizedAndBounded(newFirstColor);
    if (!m_firstColorCorrected.hasSameCoordinates(correctedNewFirstColor)) {
        m_firstColorCorrected = correctedNewFirstColor;
        updateSecondColor();
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the second color property.
 * @param newSecondColor The new second color.
 * @sa @ref m_secondColorCorrectedAndAltered */
void GradientImage::setSecondColor(const LchaDouble &newSecondColor)
{
    LchaDouble correctedNewSecondColor = completlyNormalizedAndBounded(newSecondColor);
    if (!m_secondColorCorrectedAndAltered.hasSameCoordinates(correctedNewSecondColor)) {
        m_secondColorCorrectedAndAltered = correctedNewSecondColor;
        updateSecondColor();
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Updates @ref m_secondColorCorrectedAndAltered
 *
 * This update takes into account the current values of
 * @ref m_firstColorCorrected and @ref m_secondColorCorrectedAndAltered. */
void GradientImage::updateSecondColor()
{
    m_secondColorCorrectedAndAltered = completlyNormalizedAndBounded(m_secondColorCorrectedAndAltered);
    if (qAbs(m_firstColorCorrected.h - m_secondColorCorrectedAndAltered.h) > 180) {
        if (m_firstColorCorrected.h > m_secondColorCorrectedAndAltered.h) {
            m_secondColorCorrectedAndAltered.h += 360;
        } else {
            m_secondColorCorrectedAndAltered.h -= 360;
        }
    }
}

/** @brief Delivers an image of a gradient
 *
 * @returns Delivers an image of a gradient. Its size is @ref m_gradientLength
 * and its height is @ref m_gradientThickness. The first color will be
 * at the left, and the second color will be at the right. The background
 * of transparent colors (if any) will be aligned to the top-left edge.
 *
 * If a color is out-of-gamut, a nearby substitution color will be used. */
QImage GradientImage::getImage()
{
    // If image is in cache, simply return the cache.
    if (!m_image.isNull()) {
        return m_image;
    }

    // If no cache is available (m_image.isNull()), render a new image.

    // Special case: zero-size-image
    if ((m_gradientLength <= 0) || (m_gradientThickness <= 0)) {
        return m_image;
    }

    // If no image is in cache, create a new one (in the cache) and return it.

    // First, create an image of the gradient with only one pixel thickness.
    // (Color management operations are expensive in CPU time; we try to
    // minimize this.)
    QImage temp(m_gradientLength, 1, QImage::Format_ARGB32_Premultiplied);
    temp.fill(Qt::transparent); // Initialize the image with transparency.
    LchaDouble color;
    for (int i = 0; i < m_gradientLength; ++i) {
        color = colorFromValue((i + 0.5) / static_cast<qreal>(m_gradientLength));
        temp.setPixelColor(i, 0, m_rgbColorSpace->colorRgbBound(color));
    }

    // Now, create a full image of the gradient
    m_image = QImage(m_gradientLength, m_gradientThickness, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&m_image);
    // Transparency background
    if ((m_firstColorCorrected.a != 1) || (m_secondColorCorrectedAndAltered.a != 1)) {
        // Fill the image with tiles. (QBrush will ignore
        // the devicePixelRatioF of the image of the tile.)
        painter.fillRect(0, 0, m_gradientLength, m_gradientThickness, QBrush(transparencyBackground(m_devicePixelRatioF)));
    }
    // Paint the gradient itself.
    for (int i = 0; i < m_gradientThickness; ++i) {
        painter.drawImage(0, i, temp);
    }

    // Set the correct scaling information for the image and return
    m_image.setDevicePixelRatio(m_devicePixelRatioF);
    return m_image;
}

/** @brief The color that the gradient has at a given position of the gradient.
 * @param value The position. Valid range: <tt>[0.0, 1.0]</tt>. <tt>0.0</tt>
 * means the first color, <tt>1.0</tt> means the second color, and everything
 * in between means a color in between.
 * @returns If the position is valid: The color at the given position and
 * its corresponding alpha value. If the position is out-of-range: An
 * arbitrary value. */
LchaDouble GradientImage::colorFromValue(qreal value) const
{
    LchaDouble color;
    color.l = m_firstColorCorrected.l + (m_secondColorCorrectedAndAltered.l - m_firstColorCorrected.l) * value;
    color.c = m_firstColorCorrected.c + (m_secondColorCorrectedAndAltered.c - m_firstColorCorrected.c) * value;
    color.h = m_firstColorCorrected.h + (m_secondColorCorrectedAndAltered.h - m_firstColorCorrected.h) * value;
    color.a = m_firstColorCorrected.a + (m_secondColorCorrectedAndAltered.a - m_firstColorCorrected.a) * value;
    return color;
}

/** @brief Setter for the device pixel ratio (floating point).
 *
 * This value is set as device pixel ratio (floating point) in the
 * <tt>QImage</tt> that this class holds. It does <em>not</em> change
 * the <em>pixel</em> size of the image or the pixel size of wheel
 * thickness or border.
 *
 * This is for HiDPI support. You can set this to
 * <tt>QWidget::devicePixelRatioF()</tt> to get HiDPI images in the correct
 * resolution for your widgets. Within a method of a class derived
 * from <tt>QWidget</tt>, you could write:
 *
 * @snippet test/testgradientimage.cpp GradientImage HiDPI usage
 *
 * The default value is <tt>1</tt> which means no special scaling.
 *
 * @param newDevicePixelRatioF the new device pixel ratio as a
 * floating point data type. (Values smaller than <tt>1.0</tt> will be
 * considered as <tt>1.0</tt>.) */
void GradientImage::setDevicePixelRatioF(const qreal newDevicePixelRatioF)
{
    const qreal tempDevicePixelRatioF = qMax<qreal>(1, newDevicePixelRatioF);
    if (m_devicePixelRatioF != tempDevicePixelRatioF) {
        m_devicePixelRatioF = tempDevicePixelRatioF;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the gradient length property.
 *
 * @param newGradientLength The new gradient length, measured
 * in <em>physical pixels</em>. */
void GradientImage::setGradientLength(const int newGradientLength)
{
    const int temp = qMax(0, newGradientLength);
    if (m_gradientLength != temp) {
        m_gradientLength = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the gradient thickness property.
 *
 * @param newGradientThickness The new gradient thickness, measured
 * in <em>physical pixels</em>. */
void GradientImage::setGradientThickness(const int newGradientThickness)
{
    const int temp = qMax(0, newGradientThickness);
    if (m_gradientThickness != temp) {
        m_gradientThickness = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

} // namespace PerceptualColor
