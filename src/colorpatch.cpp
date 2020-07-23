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

// Own header
#include "PerceptualColor/colorpatch.h"

#include <QPainter>

#include "PerceptualColor/helper.h"

namespace PerceptualColor {

/** @brief Constructor */
ColorPatch::ColorPatch(QWidget *parent) : QFrame(parent)
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
 * @sa minimumSizeHint() */
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
 * @sa sizeHint() */
QSize ColorPatch::minimumSizeHint() const
{
    return QSize(50, 50);
}

QColor ColorPatch::color() const
{
    return m_color;
}

/** @brief Set the color() property. */
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
    // Paint the frame, as provided by the base class
    QFrame::paintEvent(event);

    // We do not paint directly on the widget, but on a QImage buffer first:
    // As Qt documentation says:
    //
    //      “To get the optimal rendering result using QPainter, you should
    //       use the platform independent QImage as paint device; i.e. using
    //       QImage will ensure that the result has an identical pixel
    //       representation on any platform.”
    if (m_color.isValid()) {
        QImage paintBuffer(size(), QImage::Format_ARGB32);
        paintBuffer.fill(Qt::transparent);
        QPainter painter(&paintBuffer);
        if (m_color.alphaF() < 1) {
            // Background for colors that are not fully opaque
            painter.fillRect(
                contentsRect(),
                QBrush(Helper::transparencyBackground())
            );
        }
        painter.fillRect(contentsRect(), m_color);
        // Paint the buffer to the actual widget
        QPainter(this).drawImage(0, 0, paintBuffer);
    }
}

}
