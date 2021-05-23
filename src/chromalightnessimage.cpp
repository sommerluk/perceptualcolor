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
#include "chromalightnessimage.h"

#include "lchvalues.h"
#include "polarpointf.h"

#include <QPainter>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which the image should operate.
 * Can be created with @ref RgbColorSpaceFactory. */
ChromaLightnessImage::ChromaLightnessImage(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_rgbColorSpace(colorSpace)
{
}

/** @brief Setter for the backgroundColor property.
 *
 * @param newBackgroundColor The new background color. Set this to an
 * invalid <tt>QColor</tt> to get the default background.
 *
 * @todo This function should become obsolete once @ref RgbColorSpace
 * does not rely anymore on an image to find nearest in-gamut colors. */
void ChromaLightnessImage::setBackgroundColor(const QColor newBackgroundColor)
{
    if (m_backgroundColor != newBackgroundColor) {
        m_backgroundColor = newBackgroundColor;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the image size property.
 *
 * This value fixes the size of the image.
 *
 * @param newImageSize The new image size, measured in <em>physical</em>
 * pixels. */
void ChromaLightnessImage::setImageSize(const QSize newImageSize)
{
    // Not all empty sizes are 0, 0. They might be something like -1, 6.
    // Therefore, we normalize it to 0, 0.
    const QSize temp = (newImageSize.isEmpty() ? QSize(0, 0) : newImageSize);
    if (m_imageSizePhysical != temp) {
        m_imageSizePhysical = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the hue property.
 *
 * @param newHue The new hue. Valid range is <tt>[0, 360[</tt>.
 * Values outside of this range will be normalized by
 * @ref PolarPointF::normalizedAngleDegree(). */
void ChromaLightnessImage::setHue(const qreal newHue)
{
    const qreal temp = PolarPointF::normalizedAngleDegree(newHue);
    if (m_hue != temp) {
        m_hue = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Delivers an image of a chroma-lightness diagram.
 *
 * @returns A chroma-lightness diagram. For the y axis, its height covers
 * the lightness range [0, 100]. Pixel <tt>(0)</tt> corresponds to
 * value 100. Pixel <tt>(height-1)</tt> corresponds to value 0.
 * Its x axis uses always the same scale as the y axis. So if the size
 * is a square, both x range and y range are from 0 to 100. If the
 * width is larger than the height, the x range goes beyond 100. The
 * image paints all the LCh values that are within the gamut and x/y range.
 *
 * Intentionally there is no anti-aliasing because this would be much slower:
 * As there is no mathematical description of the shape of the color solid,
 * the only easy way to get anti-aliasing would be to render at a higher
 * resolution (say two times higher, which would yet mean four times more
 * data), and then downscale it to the final resolution.
 *
 * @todo Review this code! */
QImage ChromaLightnessImage::getImage()
{
    // If there is an image in cache, simply return the cache.
    if (!m_image.isNull()) {
        return m_image;
    }

    // If no image is in cache, create a new one (in the cache) with
    // correct image size.
    m_image = QImage(m_imageSizePhysical, QImage::Format_ARGB32_Premultiplied);

    // Initialization
    LchDouble LCh;
    QColor rgbColor;
    int x;
    int y;
    const int maxHeight = m_imageSizePhysical.height() - 1;
    const int maxWidth = m_imageSizePhysical.width() - 1;

    // Test if image size is too small.
    if ((maxHeight < 1) || (maxWidth < 1)) {
        // maxHeight and maxWidth must be at least >= 1 for our
        // algorithm. If they are 0, this would crash (division by 0).
        return m_image;
    }

    // Initialize the image background
    if (m_backgroundColor.isValid()) {
        m_image.fill(m_backgroundColor);
    } else {
        m_image.fill(m_rgbColorSpace->toQColorRgbBound(LchValues::neutralGray));
    }

    // Paint the gamut.
    LCh.h = PolarPointF::normalizedAngleDegree(m_hue);
    for (y = 0; y <= maxHeight; ++y) {
        LCh.l = y * static_cast<cmsFloat64Number>(100) / maxHeight;
        for (x = 0; x <= maxWidth; ++x) {
            // Using the same scale as on the y axis. floating point
            // division thanks to 100 which is a "cmsFloat64Number"
            LCh.c = x * static_cast<cmsFloat64Number>(100) / maxHeight;
            rgbColor = m_rgbColorSpace->toQColorRgbUnbound(LCh);
            if (rgbColor.isValid()) {
                // The pixel is within the gamut
                m_image.setPixelColor(x, maxHeight - y, rgbColor);
                // If color is out-of-gamut: We have chroma on the x axis and
                // lightness on the y axis. We are drawing the pixmap line per
                // line, so we go for given lightness from low chroma to high
                // chroma. Because of the nature of most gamuts, if once in a
                // line we have an out-of-gamut value, all other pixels that
                // are more at the right will be out-of-gamut also. So we
                // could optimize our code and break here. But as we are not
                // sure about this (we do not know the gamut at compile time)
                // for the moment we do not optimize the code.
            }
        }
    }

    return m_image;
}

} // namespace PerceptualColor
