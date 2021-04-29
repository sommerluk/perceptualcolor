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
#include "colorwheelimage.h"

#include "helper.h"
#include "lchvalues.h"
#include "polarpointf.h"

#include <QPainter>
#include <QtMath>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which the image should operate
 * Can be created with @ref RgbColorSpaceFactory. */
ColorWheelImage::ColorWheelImage(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_rgbColorSpace(colorSpace)
{
}

/** @brief Setter for the border property.
 *
 * The border is the space between the outer outline of the wheel and the
 * limits of the image. The wheel is always centered within the limits of
 * the image. The default value is <tt>0</tt>, which means that the wheel
 * touchs the limits of the image.
 *
 * @param newBorder The new border size, measured in <em>physical</em>
 * pixels. */
void ColorWheelImage::setBorder(const qreal newBorder)
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
 * <tt>QImage</tt> that this class holds. It does <em>not</em> change
 * the <em>pixel</em> size of the image or the pixel size of wheel
 * thickness or border.
 *
 * This is for HiDPI support. You can set this to
 * <tt>QWidget::devicePixelRatioF()</tt> to get HiDPI images in the correct
 * resolution for your widgets. Within a method of a class derived
 * from <tt>QWidget</tt>, you could write:
 *
 * @snippet test/testcolorwheelimage.cpp ColorWheelImage HiDPI usage
 *
 * The default value is <tt>1</tt> which means no special scaling.
 *
 * @param newDevicePixelRatioF the new device pixel ratio as a
 * floating point data type. */
void ColorWheelImage::setDevicePixelRatioF(const qreal newDevicePixelRatioF)
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
void ColorWheelImage::setImageSize(const int newImageSize)
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

/** @brief Setter for the wheel thickness property.
 *
 * The wheel thickness is the distance between the inner outline and the
 * outer outline of the wheel.
 *
 * @param newWheelThickness The new wheel thickness, measured
 * in <em>physical</em> pixels. */
void ColorWheelImage::setWheelThickness(const qreal newWheelThickness)
{
    qreal temp;
    if (newWheelThickness >= 0) {
        temp = newWheelThickness;
    } else {
        temp = 0;
    }
    if (m_wheelThicknessPhysical != temp) {
        m_wheelThicknessPhysical = temp;
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
QImage ColorWheelImage::getImage()
{
    // If image is in cache, simply return the cache.
    if (!m_image.isNull()) {
        return m_image;
    }

    // If no cache is available (m_image.isNull()), render a new image.

    // Special case: zero-size-image
    if (m_imageSizePhysical <= 0) {
        return m_image;
    }

    // construct our final QImage with transparent background
    m_image = QImage(QSize(m_imageSizePhysical, m_imageSizePhysical), QImage::Format_ARGB32_Premultiplied);
    m_image.fill(Qt::transparent);

    // Calculate diameter of the outer circle
    const qreal outerCircleDiameter = m_imageSizePhysical - 2 * m_borderPhysical;

    // Special case: an empty image
    if (outerCircleDiameter <= 0) {
        // Make sure to return a completly transparent image.
        // If we would continue, in spite of an outer diameter of 0,
        // we might get a non-transparent pixel in the middle.
        // Set the correct scaling information for the image and return
        m_image.setDevicePixelRatio(m_devicePixelRatioF);
        return m_image;
    }

    // Generate a temporary non-anti-aliased, intermediate, color wheel,
    // but with some pixels extra at the inner and outer side. The overlap
    // defines an overlap for the wheel, so there are some more pixels that
    // are drawn at the outer and at the inner border of the wheel, to allow
    // later clipping with anti-aliasing
    PolarPointF polarCoordinates;
    int x;
    int y;
    QColor rgbColor;
    LchDouble lch;
    qreal center = (m_imageSizePhysical - 1) / static_cast<qreal>(2);
    m_image = QImage(QSize(m_imageSizePhysical, m_imageSizePhysical), QImage::Format_ARGB32_Premultiplied);
    // Because there may be out-of-gamut colors for some hue (depending on the
    // given lightness and chroma value) which are drawn transparent, it is
    // important to initialize this image with a transparent background.
    m_image.fill(Qt::transparent);
    lch.l = LchValues::neutralLightness;
    lch.c = LchValues::srgbVersatileChroma;
    // minimumRadial: Adding "+ 1" would reduce the workload (less pixel to
    // process) and still work mostly, but not completely. It creates sometimes
    // artifacts in the anti-aliasing process. So we don't do that.
    const qreal minimumRadial = center - m_wheelThicknessPhysical - m_borderPhysical - overlap;
    const qreal maximumRadial = center - m_borderPhysical + overlap;
    for (x = 0; x < m_imageSizePhysical; ++x) {
        for (y = 0; y < m_imageSizePhysical; ++y) {
            polarCoordinates = PolarPointF(QPointF(x - center, center - y));
            if (inRange<qreal>(minimumRadial, polarCoordinates.radial(), maximumRadial)

            ) {
                // We are within the wheel
                lch.h = polarCoordinates.angleDegree();
                rgbColor = m_rgbColorSpace->colorRgb(lch);
                if (rgbColor.isValid()) {
                    m_image.setPixelColor(x, y, rgbColor);
                }
            }
        }
    }

    // Anti-aliased cut off everything outside the circle (that
    // means: the overlap)
    // The natural way would be to simply draw a circle with
    // QPainter::CompositionMode_DestinationIn which should make transparent
    // everything that is not in the circle. Unfourtunally, this does not
    // seem to work. Therefore, we use a workaround and draw a very think
    // circle outline around the circle with QPainter::CompositionMode_Clear.
    const qreal circleRadius = outerCircleDiameter / 2;
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

    // set the inner circle of the wheel to anti-aliased transparency
    const qreal innerCircleDiameter = m_imageSizePhysical - 2 * (m_wheelThicknessPhysical + m_borderPhysical);
    if (innerCircleDiameter > 0) {
        myPainter.setCompositionMode(QPainter::CompositionMode_Clear);
        myPainter.setRenderHint(QPainter::Antialiasing, true);
        myPainter.setPen(QPen(Qt::NoPen));
        myPainter.setBrush(QBrush(Qt::SolidPattern));
        myPainter.drawEllipse(QRectF(m_wheelThicknessPhysical + m_borderPhysical, m_wheelThicknessPhysical + m_borderPhysical, innerCircleDiameter, innerCircleDiameter));
    }

    // Set the correct scaling information for the image and return
    m_image.setDevicePixelRatio(m_devicePixelRatioF);
    return m_image;
}

} // namespace PerceptualColor
