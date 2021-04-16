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

#include "perceptualcolorlib_internal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/colorpatch.h"
// Second, the private implementation.
#include "colorpatch_p.h"

#include <QApplication>
#include <QPainter>
#include <QStyleOption>

#include "PerceptualColor/abstractdiagram.h"

#include "helper.h"

namespace PerceptualColor
{
/** @brief Constructor
 * @param parent The parent of the widget, if any
 */
ColorPatch::ColorPatch(QWidget *parent)
    : QFrame(parent)
    , d_pointer(new ColorPatchPrivate())
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

/** @brief Destructor */
ColorPatch::~ColorPatch() noexcept
{
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
    // Use a size similar to a QToolButton with an icon (and without text)
    ensurePolished();
    QStyleOptionToolButton option;
    option.initFrom(this);
    option.font = font();
    const int iconSize =
        style()->pixelMetric(QStyle::PM_ButtonIconSize, nullptr, this);
    option.iconSize = QSize(iconSize, iconSize);
    return style()
        ->sizeFromContents(
            QStyle::CT_ToolButton, &option, option.iconSize, this)
        .expandedTo(QApplication::globalStrut());
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor ColorPatch::color() const
{
    return d_pointer->m_color;
}

/** @brief Set the @ref color property. */
void ColorPatch::setColor(const QColor &newColor)
{
    if (newColor != d_pointer->m_color) {
        d_pointer->m_color = newColor;
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
    // First of all, draw the frame
    QFrame::paintEvent(event);

    // Initialization
    QPainter widgetPainter {this};
    widgetPainter.setRenderHint(QPainter::Antialiasing, true);
    QStyleOptionFrame opt;
    opt.initFrom(this); // Sets also QStyle::State_MouseOver if appropriate
    const int defaultFrameWidth =
        qMax(style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt), 1);

    // Draw content of an invalid color (and return)
    if (!d_pointer->m_color.isValid()) {
        widgetPainter.setRenderHint(QPainter::Antialiasing, true);
        QPalette::ColorGroup paletteColorGroup;
        if (isEnabled()) {
            paletteColorGroup = QPalette::ColorGroup::Normal;
        } else {
            paletteColorGroup = QPalette::ColorGroup::Disabled;
        }
        QPen pen {palette().color(paletteColorGroup, QPalette::WindowText)};
        pen.setWidth(defaultFrameWidth);
        pen.setCapStyle(Qt::PenCapStyle::RoundCap);
        widgetPainter.setPen(pen);
        // Calculate an adjustmend to allow for rounded line caps without
        // touching the frame.
        const qreal adjustmentForRoundedLineCaps = pen.widthF() / 2.0;
        const QRectF noColorMarkerRectangle =
            QRectF(contentsRect())
                .adjusted(adjustmentForRoundedLineCaps,
                          adjustmentForRoundedLineCaps,
                          -adjustmentForRoundedLineCaps,
                          -adjustmentForRoundedLineCaps);
        widgetPainter.drawLine(noColorMarkerRectangle.topLeft(),
                               noColorMarkerRectangle.bottomRight());
        widgetPainter.drawLine(noColorMarkerRectangle.topRight(),
                               noColorMarkerRectangle.bottomLeft());
        return;
    }

    // Draw content of a valid color
    widgetPainter.setRenderHint(QPainter::Antialiasing, false);
    // Create an image to paint on
    QImage tempImage = QImage(
        // deliberately rounding down with static_cast<int>
        static_cast<int>(contentsRect().width() * devicePixelRatioF()),
        static_cast<int>(contentsRect().height() * devicePixelRatioF()),
        QImage::Format_RGB32);
    if (d_pointer->m_color.alphaF() < 1) {
        // Prepare the image with (semi-)transparent color
        // Background for colors that are not fully opaque
        QImage tempBackground = transparencyBackground(devicePixelRatioF());
        // Paint the color above
        QPainter(&tempBackground)
            .fillRect(
                0, 0, size().width(), size().height(), d_pointer->m_color);
        // Fill a given rectangle with tiles. (QBrush will ignore
        // the devicePixelRatioF of the image of the tile.)
        QPainter(&tempImage)
            .fillRect(0,
                      0,
                      tempImage.width(),
                      tempImage.height(),
                      QBrush(tempBackground));
        if (layoutDirection() == Qt::RightToLeft) {
            // Horizontally mirrored image for right-to-left layout,
            // so that the “nice” part is the first you see in reading
            // direction.
            tempImage = tempImage.mirrored(true, // horizontally mirrored
                                           false // vertically mirrored
            );
        }
    } else {
        // Prepare the image with plain color
        tempImage.fill(d_pointer->m_color);
    }
    // Set correct devicePixelRatioF for image
    tempImage.setDevicePixelRatio(devicePixelRatioF());
    // Paint the image on the widget
    widgetPainter.drawImage(
        contentsMargins().left(), contentsMargins().top(), tempImage);
}

}
