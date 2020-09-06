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

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent The widget’s parent widget. This paramenter will be passed
 * to the QWidget base class constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
: QWidget(parent)
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

/** @brief The rounded size of the widget mesured in <em>physical</em>
 * pixels.
 * 
 * The function <tt>QWidget::size()</tt> returns the size of the widget
 * mesured in logical pixels. On low-dpi devices, this is identical to the
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
 * <tt>QImage::setDevicePixelRatio()</tt>. Non-integer results
 * are always rounded <em>down</em> to the next integer. As we cannot be
 * sure that pixel alignment is identical on all platforms, it’s saver to
 * always round down. Maybe in some situations, there will be an
 * unused pixel at the right or at the botton. So, a length of
 * 101 logical pixels at a device pixel ratio of 1.5 will result in
 * 151 physical pixels.
 * 
 * @returns The rounded size of the widget mesured in <em>physical</em>
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

}
