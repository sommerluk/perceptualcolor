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

/** @file
 * 
 * Definition of the @ref PerceptualColor::ColorPatch class and its members. */

// Own header
#include "PerceptualColor/colorpatch.h"

#include <QPainter>

#include "PerceptualColor/helper.h"

namespace PerceptualColor {

/** @brief Constructor */
ColorPatch::ColorPatch(QWidget *parent) : AbstractDiagram(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

/** @brief Provide the size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the size hint
 * 
 * @sa @ref minimumSizeHint() */
QSize ColorPatch::sizeHint() const
{
    return minimumSizeHint();
}

/** @brief Provide the minimum size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the minimum size hint
 * 
 * @sa @ref sizeHint() */
QSize ColorPatch::minimumSizeHint() const
{
    return QSize(50, 50);
}

QColor ColorPatch::color() const
{
    return m_color;
}

/** @brief Set the @ref color property. */
void ColorPatch::setColor(const QColor &newColor)
{
    if (newColor != m_color) {
        m_color = newColor;
        Q_EMIT colorChanged(newColor);
        update(); // schedules a paint event
    }
}

/** @brief Handle paint events.
 * 
 * Just draws the frame inherited from QFrame, than paints a rectangle with
 * the current color above. */
void ColorPatch::paintEvent(QPaintEvent *event)
{
    
    if (!m_color.isValid()) {
        // Paint the frame, as provided by the base class
        QFrame::paintEvent(event);
        return;
    }

    // Create an image to paint on
    QImage tempImage = QImage(
        qRound(contentsRect().width() * devicePixelRatioF()),
        qRound(contentsRect().height() * devicePixelRatioF()),
        QImage::Format_RGB32
    );
    if (m_color.alphaF() < 1) {
        // Prepare the image with (semi-)transparent color
        QImage tempBackground;
            // Background for colors that are not fully opaque
            tempBackground = transparencyBackground(
                devicePixelRatioF()
            );
        // Paint the color above
        QPainter(&tempBackground).fillRect(
            0,
            0,
            size().width(),
            size().height(),
            m_color
        );
        // Fill the image with tiles. (QBrush will ignore
        // the devicePixelRatioF.)
        QPainter(&tempImage).fillRect(
            0,
            0,
            tempImage.width(),
            tempImage.height(),
            QBrush(tempBackground)
        );
        if (layoutDirection() == Qt::RightToLeft) {
            // Horizontally mirrored image for right-to-left layout,
            // so that the “nice” part is the first you see in reading
            // direction.
            tempImage = tempImage.mirrored(
                true,   // horizontally mirrored 
                false   // vertically mirrored
            );
        }
    } else {
        // Prepare the image with plain color
        QPainter(&tempImage).fillRect(
            0,
            0,
            tempImage.width(),
            tempImage.height(),
            m_color
        );
    }
    // Set correct devicePixelRatioF for image
    tempImage.setDevicePixelRatio(devicePixelRatioF());
    // Paint the image on the widget
    QPainter(this).drawImage(
        contentsRect().x(),
        contentsRect().y(),
        tempImage
    );
    // Paint the frame, as provided by the base class
    QFrame::paintEvent(event);
}

}
