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

#include "perceptualcolorlib_qtconfiguration.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromahueimage.h"

#include "lchvalues.h"

#include <QPainter>
#include <QtMath>

namespace PerceptualColor {

/** @brief Constructor */
ChromaHueImage::ChromaHueImage(
    const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
) :
    m_rgbColorSpace (colorSpace)
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
 * of <tt>QSize(newImageSize, newImageSize</tt>.
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

// TODO xxx

/** @brief Setter for the lightness property.
 * 
 * @param newLightness The new lightness. Valid range: 0‥100 */
void ChromaHueImage::setLightness(const qreal newLightness)
{
    const qreal temp = qBound(
        static_cast<qreal>(0),
        newLightness,
        static_cast<qreal>(100)
    );
    if (m_lightness != temp) {
        m_lightness = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Setter for the lightness property.
 * 
 * @param newChromaRange The new lightness. Valid
 * range: 0‥@ref LchValues::humanMaximumChroma */
void ChromaHueImage::setChromaRange(const qreal newChromaRange)
{
    // TODO Is 0 valid or will it crash our image generation code?
    const qreal temp = qBound(
        static_cast<qreal>(0),
        newChromaRange,
        static_cast<qreal>(LchValues::humanMaximumChroma)
    );
    if (m_chromaRange != temp) {
        m_chromaRange = temp;
        // Free the memory used by the old image.
        m_image = QImage();
    }
}

/** @brief Delivers an image of a color wheel
* 
* @returns Delivers a square image of a color wheel. Its size
* is <tt>QSize(imageSize, imageSize)</tt>. All pixels
* that do not belong to the wheel itself will be transparent.
* Antialiasing is used, so there is no sharp border between
* transparent and non-transparent parts. Depending on the
* values for lightness and chroma and the available colors in
* the current color space, there may be some hue who is out of
*  gamut; if so, this part of the wheel will be transparent.
* 
* @todo Out-of-gamut situations should automatically be handled. */
QImage ChromaHueImage::getImage()
{
    if (m_image.isNull()) {
        generateNewImage();
    }
    return m_image;
}

/** @brief in image of a-b plane of the color space at a given lightness */
// How to make sure the diagram has at least a few pixels?
void ChromaHueImage::generateNewImage()
{
    const int maxIndex = m_imageSizePhysical - 1;
    // Test if image size is too small.
    if (maxIndex < 1) {
        // maxIndex must be at least >= 1 for our algorithm. If they are 0,
        // this would crash (division by 0). TODO how to solve this?
        m_image = QImage();
        return;
    }

    // Setup
    cmsCIELab lab; // uses cmsFloat64Number internally
    int x;
    int y;
    QColor tempColor;
    // TODO If the gamut does not touch the outline of the circle, than
    // we could also paint directly on m_image, which would save memory.
    // And the antialiasing result would be the same. If the gamut however
    // touchs the outline, the antialiasing would be ugly there…
    QImage tempImage = QImage(
        QSize(m_imageSizePhysical, m_imageSizePhysical),
        QImage::Format_ARGB32_Premultiplied
    );
    tempImage.fill(
        m_rgbColorSpace->colorRgbBound(
            LchValues::neutralGray
        )
    ); // Initialize the image background
    const qreal scaleFactor =
        static_cast<qreal>(2 * m_chromaRange)
            / (m_imageSizePhysical - 2 * m_borderPhysical);
 
    // Paint the gamut.
    lab.L = m_lightness;
    for (y = 0; y <= maxIndex; ++y) {
        lab.b = m_chromaRange - (y - m_borderPhysical) * scaleFactor;
        for (x = 0; x <= maxIndex; ++x) {
            lab.a = (x - m_borderPhysical) * scaleFactor - m_chromaRange;
            if ((qPow(lab.a, 2) + qPow(lab.b, 2)) <= qPow(m_chromaRange, 2)) {
                tempColor = m_rgbColorSpace->colorRgb(lab);
                if (tempColor.isValid()) {
                    // The pixel is within the gamut
                    tempImage.setPixelColor(x, y, tempColor);
                }
            }
        }
    }

    m_image = QImage(
        QSize(m_imageSizePhysical, m_imageSizePhysical),
        QImage::Format_ARGB32_Premultiplied
    );
    m_image.fill(Qt::transparent);
    // Cut off everything outside the circle
    QPainter myPainter(&m_image);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::NoPen));
    myPainter.setBrush(QBrush(tempImage));
    myPainter.drawEllipse(
        QPointF(
            static_cast<qreal>(m_imageSizePhysical) / 2,
            static_cast<qreal>(m_imageSizePhysical) / 2
        ),                                                // center
        (m_imageSizePhysical - 2 * m_borderPhysical) / 2, // width
        (m_imageSizePhysical - 2 * m_borderPhysical) / 2  // height
    );

    // Set the correct scaling information for the image
    m_image.setDevicePixelRatio(m_devicePixelRatioF);
}

}
