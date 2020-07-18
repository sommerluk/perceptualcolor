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

#include "PerceptualColor/helper.h"

#include <QDebug>
#include <QPainter>

namespace PerceptualColor {

/** @brief Constructor */
ColorPatch::ColorPatch(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_brush = QBrush(Helper::transparencyBackground());
    setColor(QColor()); // an invalid color
}

// TODO Provide a special representation (blank widget with two crossed red
// lines?) for showing absent of a color (different from transparency!)
// -> not necessary because yet implemented (background, without red
// lines)!?

/** @brief Handle paint events.
 * 
 * Just draw the frame inherited from QFrame, than paint a rectangle with
 * the current color above.
 */
void ColorPatch::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    // We do not paint directly on the widget, but on a QImage buffer first:
    // Render anti-aliased looks better. But as Qt documentation says:
    //
    //      “Renderhints are used to specify flags to QPainter that may or
    //       may not be respected by any given engine.”
    //
    // Painting here directly on the widget might lead to different
    // anti-aliasing results depending on the underlying window system. This is
    // especially problematic as anti-aliasing might shift or not a pixel to the
    // left or to the right. So we paint on a QImage first. As QImage (at
    // difference to QPixmap and a QWidget) is independant of native platform
    // rendering, it guarantees identical anti-aliasing results on all
    // platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should use
    //       the platform independent QImage as paint device; i.e. using QImage
    //       will ensure that the result has an identical pixel representation
    //       on any platform.”
    if (m_color.isValid()) {
        QImage paintBuffer(size(), QImage::Format_ARGB32);
        paintBuffer.fill(Qt::transparent);
        QPainter painter(&paintBuffer);
        if (m_color.alphaF() < 1) {
            painter.fillRect(contentsRect(), m_brush);
        }
        painter.fillRect(contentsRect(), m_color);
        // Paint the buffer to the actual widget
        QPainter(this).drawImage(0, 0, paintBuffer);
    }
}

/** @brief Provide the size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the size hint
 * 
 * @sa minimumSizeHint()
 */
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
 * @sa sizeHint()
 */
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
        update();
    }
}

}
