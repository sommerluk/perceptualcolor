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
#include "PerceptualColor/abstractdiagram.h"
// Second, the private implementation.
#include "abstractdiagram_p.h"

#include <cmath>

#include <QPainter>

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the QWidget base class constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
: QFrame(parent)
{
}

/** @brief Destructor */
AbstractDiagram::~AbstractDiagram() noexcept
{
}

/** @brief The color for painting focus indicators
 * 
 * @returns The color for painting focus indicators. This color is taken from
 * the current widget style at the moment this function is called. The value
 * might therefore be different on the next function call, if the widget style
 * has been switched by the user in the meantime.
 * 
 * @note If is known that some (external) Qt style returns a wrong value
 * for this color. */
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
 * the result of this function is rounded down. Qt’s widget geometry code
 * has no documentation about how this is handeled. However, Qt seems to
 * round up starting with 0.5, at least on Linux/X11. But there are a few
 * themes (for example the “Kvantum style engine” with the style
 * “MildGradientKvantum”) that seem to round down: This becomes visible,
 * as the corresponding last pixels are not automatically redrawn before
 * executing the <tt>paintEvent()</tt> code. To avoid relying on undocumented
 * behaviour and to avoid known problems with some styles, this function
 * is concervative and always rounds down.
 * 
 * @returns The size of the widget measured in <em>physical</em>
 * pixels, as recommended image size for calling
 * <tt>QPainter::drawImage()</tt> during a paint event. */
QSize AbstractDiagram::physicalPixelSize() const
{
    // Assert that static_cast<int> always rounds down.
    static_assert(static_cast<int>(1.9) == 1);
    static_assert(static_cast<int>(1.5) == 1);
    static_assert(static_cast<int>(1.0) == 1);
    // Multiply the size with the (floating point) scale factor
    // and than round down (by using static_cast<int>).
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
** @returns An image of a mosaic of neutral gray rectangles of different
** lightness. You can use this as tiles to paint a background.
** 
** @note The image is considering QWidget::devicePixelRatioF() to deliver
** sharp (and correctly scaled) images also for HiDPI devices.
** The painting does not use floating point drawing, but rounds
** to full integers. Therefore, the result is always a sharp image.
** This function takes care that each square has the same pixel size,
** without scaling errors or anti-aliasing errors.
** 
** Example code (to use within a class that inherits from
** @ref PerceptualColor::AbstractDiagram):
** @snippet test/testabstractdiagram.cpp AbstractDiagram Use transparency background
** 
** @todo Provide color management support? Currently, we use the same
** value for red, green and blue, this might <em>not</em> be perfectly
** neutral gray depending on the color profile of the monitor… */
QImage AbstractDiagram::transparencyBackground() const
{
    constexpr int lightnessOne = 210; // valid range is [0, 255]
    constexpr int lightnessTwo = 240; // valid range is [0, 255]
    constexpr int squareSizeInLogicalPixel = 10;
    const int squareSize = qRound(
        squareSizeInLogicalPixel * devicePixelRatioF()
    );

    QImage temp(squareSize * 2, squareSize * 2, QImage::Format_RGB32);
    temp.fill(QColor(lightnessOne, lightnessOne, lightnessOne));
    QPainter painter(&temp);
    QColor foregroundColor(lightnessTwo, lightnessTwo, lightnessTwo);
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
    temp.setDevicePixelRatio(devicePixelRatioF());
    return temp;
}

}
