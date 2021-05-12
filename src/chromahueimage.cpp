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
#include "chromahueimage.h"

#include "helper.h"
#include "lchvalues.h"

#include <QPainter>
#include <QtMath>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which the image should operate.
 * Can be created with @ref RgbColorSpaceFactory. */
ChromaHueImage::ChromaHueImage(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_rgbColorSpace(colorSpace)
{
}

/** @brief Setter for the border property.
 *
 * The border is the space between the outer outline of the diagram and the
 * limits of the image. The diagram is always centered within the limits of
 * the image. The default value is <tt>0</tt>, which means that the diagram
 * touchs the limits of the image.
 *
 * @param newBorder The new border size, measured in <em>physical</em>
 * pixels. */
void ChromaHueImage::setBorder(const qreal newBorder)
{
    qreal tempBorder;
    if (newBorder >= 0) {
        tempBorder = newBorder;
    } else {
        tempBorder = 0;
    }
    if (m_borderPhysical != tempBorder) {
        m_borderPhysical = tempBorder;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the device pixel ratio (floating point).
 *
 * This value is set as device pixel ratio (floating point) in the
 * QImage that this class holds. It does <em>not</em> change
 * the <em>pixel</em> size of the image or the pixel size or the border.
 *
 * This is for HiDPI support. You can set this to
 * <tt>QWidget::devicePixelRatioF()</tt> to get HiDPI images in the correct
 * resolution for your widgets. Within a method of a class derived
 * from <tt>QWidget</tt>, you could write:
 *
 * @snippet test/testchromahueimage.cpp ChromaHueImage HiDPI usage
 *
 * The default value is <tt>1</tt> which means no special scaling.
 *
 * @param newDevicePixelRatioF the new device pixel ratio as a
 * floating point data type. */
void ChromaHueImage::setDevicePixelRatioF(const qreal newDevicePixelRatioF)
{
    qreal tempDevicePixelRatioF;
    if (newDevicePixelRatioF >= 1) {
        tempDevicePixelRatioF = newDevicePixelRatioF;
    } else {
        tempDevicePixelRatioF = 1;
    }
    if (m_devicePixelRatioF != tempDevicePixelRatioF) {
        m_devicePixelRatioF = tempDevicePixelRatioF;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the image size property.
 *
 * This value fixes the size of the image. The image will be a square
 * of <tt>QSize(newImageSize, newImageSize)</tt>.
 *
 * @param newImageSize The new image size, measured in <em>physical</em>
 * pixels. */
void ChromaHueImage::setImageSize(const int newImageSize)
{
    int tempImageSize;
    if (newImageSize >= 0) {
        tempImageSize = newImageSize;
    } else {
        tempImageSize = 0;
    }
    if (m_imageSizePhysical != tempImageSize) {
        m_imageSizePhysical = tempImageSize;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the lightness property.
 *
 * @param newLightness The new lightness. Valid range is <tt>[0, 100]</tt>. */
void ChromaHueImage::setLightness(const qreal newLightness)
{
    const qreal temp = qBound(static_cast<qreal>(0), newLightness, static_cast<qreal>(100));
    if (m_lightness != temp) {
        m_lightness = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the chroma range property.
 *
 * @param newChromaRange The new chroma range. Valid
 * range is <tt>[0, @ref LchValues::humanMaximumChroma]</tt>. */
void ChromaHueImage::setChromaRange(const qreal newChromaRange)
{
    const qreal temp = qBound(static_cast<qreal>(0), static_cast<qreal>(newChromaRange), static_cast<qreal>(LchValues::humanMaximumChroma));
    if (m_chromaRange != temp) {
        m_chromaRange = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Delivers an image of the chroma hue plane.
 *
 * @returns Delivers a square image of the chroma hue plane. It consists
 * of a circle with a background color. The circle has a distance of
 * @ref setBorder() to the border of the <tt>QImage</tt>. The <tt>QImage</tt>
 * itself has the size <tt>QSize(imageSize, imageSize)</tt>. All pixels
 * outside the circle will be transparent. Antialiasing is used, so there
 * is no sharp border between transparent and non-transparent parts. The
 * chroma hue plane is drawn within the background circle and will not exceed
 * it. */
QImage ChromaHueImage::getImage()
{
    // If there is an image in cache, simply return the cache.
    if (!m_image.isNull()) {
        return m_image;
    }

    // If no image is in cache, create a new one (in the cache) with
    // correct image size.
    m_image = QImage(QSize(m_imageSizePhysical, m_imageSizePhysical), QImage::Format_ARGB32_Premultiplied);
    // Calculate the radius of the circle we want to paint (and which will
    // finally have the background color, while everything around will be
    // transparent).
    const qreal circleRadius = (m_imageSizePhysical - 2 * m_borderPhysical) / 2.;
    if (circleRadius <= 0) {
        // The border is too big the and image size too small: The size
        // of the circle is zero. The image will therefore be transparent.
        // Initialize the image as completly transparent and return.
        m_image.fill(Qt::transparent);
        // Set the correct scaling information for the image and return
        m_image.setDevicePixelRatio(m_devicePixelRatioF);
        return m_image;
    }
    // If we continue, the circle will at least be visible.
    // Initialize the hole image background to the background color
    // of the circle.
    m_image.fill(m_rgbColorSpace->colorRgbBound(LchValues::neutralGray));

    // Prepare for gamut painting
    cmsCIELab lab;
    lab.L = m_lightness;
    int x;
    int y;
    QColor tempColor;
    const qreal scaleFactor = static_cast<qreal>(2 * m_chromaRange)
        // The following line will never be 0 because we have have
        // tested above that circleRadius is > 0, so this line will
        // we > 0 also.
        / (m_imageSizePhysical - 2 * m_borderPhysical);

    // Paint the gamut.
    // The pixel at position QPoint(x, y) is the square with the top-left
    // edge at coordinate point QPoint(x, y) and the botton-right edge at
    // coordinate point QPoint(x+1, y+1). This pixel is supposed to have
    // the color from coordinate point QPoint(x+0.5, y+0.5), which is
    // the middle of this pixel. Therefore, with an offset of 0.5 we can
    // convert from the pixel position to the point in the middle of the pixel.
    constexpr qreal pixelOffset = 0.5;
    // TODO Could this be further optimized? For example not go from zero
    // up to m_imageSizePhysical, but exclude the border (and add the
    // tolerance)? Tought anyway the color transform (which is the heavy
    // work) is only done when within a given diameter, reducing loop runs
    // itself might also increase performance at least a little bit…
    for (y = 0; y < m_imageSizePhysical; ++y) {
        lab.b = m_chromaRange - (y + pixelOffset - m_borderPhysical) * scaleFactor;
        for (x = 0; x < m_imageSizePhysical; ++x) {
            lab.a = (x + pixelOffset - m_borderPhysical) * scaleFactor - m_chromaRange;
            if ((qPow(lab.a, 2) + qPow(lab.b, 2)) <= (qPow(m_chromaRange + overlap, 2))) {
                tempColor = m_rgbColorSpace->colorRgb(lab);
                if (tempColor.isValid()) {
                    // The pixel is within the gamut!
                    m_image.setPixelColor(x, y, tempColor);
                }
            }
        }
    }

    // Cut off everything outside the circle.
    // If the gamut does not touch the outline of the circle, than
    // we could also paint directly on m_image, which would save memory.
    // And the antialiasing result would be the same. If the gamut however
    // touchs the outline, the antialiasing would be ugly there, therefore
    // we cut everything outside the circle explicitly.
    // The natural way would be to simply draw a circle with
    // QPainter::CompositionMode_DestinationIn which should make transparent
    // everything that is not in the circle. Unfourtunally, this does not
    // seem to work. Therefore, we use a workaround and draw a very think
    // circle outline around the circle with QPainter::CompositionMode_Clear.
    const qreal cutOffThickness = qSqrt(qPow(m_imageSizePhysical, 2) * 2) / 2 // ½ of image diagonal
        - circleRadius                                                        // circle radius
        + overlap;                                                            // just to be sure
    QPainter myPainter(&m_image);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::SolidPattern, cutOffThickness));
    myPainter.setCompositionMode(QPainter::CompositionMode_Clear);
    myPainter.drawEllipse(QPointF(static_cast<qreal>(m_imageSizePhysical) / 2,
                                  static_cast<qreal>(m_imageSizePhysical) / 2), // center
                          circleRadius + cutOffThickness / 2,                   // width
                          circleRadius + cutOffThickness / 2                    // height
    );

    // Set the correct scaling information for the image and return
    m_image.setDevicePixelRatio(m_devicePixelRatioF);
    return m_image;
}

} // namespace PerceptualColor
