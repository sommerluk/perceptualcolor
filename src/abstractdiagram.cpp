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
#include "PerceptualColor/abstractdiagram.h"

#include <QPainter>

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the QWidget base class constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
: QFrame(parent)
{
}

/** @brief The color for painting focus indicators
 * 
 * @returns The color for painting focus indicators. This color is taken from
 * the current widget style and might therefore change if the widget style is
 * switched by the user. */
QColor AbstractDiagram::focusIndicatorColor() const
{
    return palette().color(QPalette::Highlight);
}

/** @brief The rounded size of the widget measured in <em>physical</em>
 * pixels.
 * 
 * The function <tt>QWidget::size()</tt> returns the size of the widget
 * measured in logical pixels. On low-dpi devices, this is identical to the
 * physical pixels, but on high-dpi devices it is not. To paint high-dpi
 * widgets, you have to
 * 
 * - Prepare a high-dpi version of the image you want to paint, for example
 *   as a QImage.
 * 
 * - Set <tt>QImage::setDevicePixelRatio()</tt> of the image to the same
 *   value as <tt>QPaintDevice::devicePixelRatioF()</tt> of the widget.
 * 
 * - Actually paint the image on the widget, on position <tt>(0, 0)</tt> and
 *   <em>without</em> anti-aliasing.
 * 
 * If you want know the pixel size of the image you have to prepare in a
 * paint event, this function provides the size conveniently, based on
 * <tt>QPaintDevice::devicePixelRatioF()</tt>.
 * 
 * @note If <tt>QPaintDevice::devicePixelRatioF()</tt> is not an integer,
 * the result of this function is rounded. Qt’s widget geometry code seems
 * to round up starting with 0.5, at least on Linux/X11. As this behaviour is
 * not documentated and might be different on other platforms, it’s however
 * not save to rely on this assumption. Therefore, in this function, 
 * non-integer values are always rounded <em>down</em> to the next lower
 * integer. In some situations, depending on rounding, there will be an
 * unused pixel column or row at the right or at the bottom of the widget.
 * Example: A length of 101 logical pixels at a device pixel ratio of 1.5
 * will result in 151 physical pixels.
 * 
 * @returns The rounded size of the widget measured in <em>physical</em>
 * pixels, as recommended for paint events. */
QSize AbstractDiagram::physicalPixelSize() const
{
    // Assert that static_cast<int> will always round down.:
    static_assert(
        static_cast<int>(1.9) == 1,
        "Assert that static_cast<int> will always round down."
    );
    static_assert(
        static_cast<int>(-1.9) == -1,
        "Assert that static_cast<int> will always round down."
    );

    // Multiply with the scale factor and round down to next integer:
    return QSize(
        static_cast<int>(size().width() * devicePixelRatioF()),
        static_cast<int>(size().height() * devicePixelRatioF())
    );
}

/** @brief Background for semi-transparent colors.
** 
** When showing a semi-transparent color, there has to be a background
** on which it is shown. This function provides a suitable background
** for showcasing a color.
** 
** @param devicePixelRatioF the QWidget::devicePixelRatioF() for which
** you want to have the background image
** @returns An image of a mosaic of neutral gray rectangles of different
** lightness. You can use this as tiles to paint a background.
** 
** @note The function does not use floating point drawing, but rounds
** to full integers. Therefor, the result is always a sharp image.
** This function takes care that each square has the same pixel size,
** without scaling errors or anti-aliasing errors.
** 
** Example:
** @snippet test/testabstractdiagram.cpp AbstractDiagram Use transparency background
** 
** @todo Provide color management support? Currently, we use the same
** value for red, green and blue, this might @em not be perfectly
** neutral gray depending on the color profile of the monitor… */
QImage AbstractDiagram::transparencyBackground(const qreal devicePixelRatioF)
{
    constexpr int colorValueOne = 210;
    constexpr int colorValueTwo = 240;
    const int squareSize = qRound(10 * devicePixelRatioF);

    QImage temp(squareSize * 2, squareSize * 2, QImage::Format_RGB32);
    temp.fill(QColor(colorValueOne, colorValueOne, colorValueOne));
    QPainter painter(&temp);
    QColor foregroundColor(colorValueTwo, colorValueTwo, colorValueTwo);
    painter.fillRect(
        0,
        0,
        squareSize,
        squareSize,
        foregroundColor
    );
    painter.fillRect(
        squareSize,
        squareSize,
        squareSize,
        squareSize,
        foregroundColor
    );
    temp.setDevicePixelRatio(devicePixelRatioF);
    return temp;
}

}
