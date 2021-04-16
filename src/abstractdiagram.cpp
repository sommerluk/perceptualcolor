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
#include "PerceptualColor/abstractdiagram.h"
// Second, the private implementation.
#include "abstractdiagram_p.h"

#include <cmath>

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

#include "helper.h"

namespace PerceptualColor
{
/** @brief The constructor.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
    : QWidget(parent)
{
}

/** @brief Destructor */
AbstractDiagram::~AbstractDiagram() noexcept
{
}

/** @brief The color for painting focus indicators
 * @returns The color for painting focus indicators. This color is based on
 * the current widget style at the moment this function is called. The value
 * might therefore be different on the next function call, if the widget style
 * has been switched by the user in the meantime.
 * @note As there is no build-in support in Qt to get this information, we
 * have to do some best guess, which might go wrong on some styles. */
QColor AbstractDiagram::focusIndicatorColor() const
{
    // This approach goes wrong on some styles. For example, the Kvantum
    // style engine provides a style names “KvFlatRed”. This style uses
    // red to highlight the background of marked text. The focus indicators
    // however are drawn in orange. This function returns red in this case,
    // which is wrong. However, I do not know how to get around this
    // limitation.
    return
        palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight);
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
    return
        QSize(static_cast<int>(size().width() * devicePixelRatioF()), static_cast<int>(size().height() * devicePixelRatioF()));
}

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>physical</em> pixels.
 *
 * This is the shorter value of width and height of the widget.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>physical</em> pixels.
 *
 * @sa @ref maximumWidgetSquareSize */
int AbstractDiagram::maximumPhysicalSquareSize() const
{
    return qMin(physicalPixelSize().width(), physicalPixelSize().height());
}

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>widget</em> pixels.
 *
 * This is the conversion of @ref maximumPhysicalSquareSize to the unit
 * “widget pixels“. It might therefore be <em>smaller</em> than the shortest
 * value of width and height of this widget because of defensive rounding.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>widget</em> pixels. */
qreal AbstractDiagram::maximumWidgetSquareSize() const
{
    return (maximumPhysicalSquareSize() / devicePixelRatioF());
}

/** @brief Background for semi-transparent colors.
 *
 * When showing a semi-transparent color, there has to be a background
 * on which it is shown. This function provides a suitable background
 * for showcasing a color.
 *
 * Example code (to use within a class that inherits from
 * @ref PerceptualColor::AbstractDiagram):
 * @snippet test/testabstractdiagram.cpp useTransparencyBackground
 *
 * @returns An image of a mosaic of neutral gray rectangles of different
 * lightness. You can use this as tiles to paint a background.
 *
 * @note The image is considering QWidget::devicePixelRatioF() to deliver
 * sharp (and correctly scaled) images also for HiDPI devices.
 * The painting does not use floating point drawing, but rounds
 * to full integers. Therefore, the result is always a sharp image.
 * This function takes care that each square has the same pixel size,
 * without scaling errors or anti-aliasing errors.
 *
 * @sa @ref transparencyBackground(qreal devicePixelRatioF) */
QImage AbstractDiagram::transparencyBackground() const
{
    return PerceptualColor::transparencyBackground(devicePixelRatioF());
}

/** @brief The outline thickness of a (either circular or linear) handle.
 *
 * Measured in widget pixels. */
int AbstractDiagram::handleOutlineThickness() const
{
    return 2;

    //     // The following code is an alternative. However, this leeds to
    //     extremly
    //     // low values for the Fusion style and extremly high values for the
    //     // Kvantum style (with KvGnomish). This is not acceptable. Therefore,
    //     // we do not use this code…
    //     QStyleOption styleOption;
    //     styleOption.initFrom(this); // Sets also QStyle::State_MouseOver
    //     return qMax(
    //         style()->pixelMetric(
    //             QStyle::PM_DefaultFrameWidth,
    //             &styleOption,
    //             this
    //         ),
    //         1
    //     );
}

/** @brief The radius of a circular handle.
 * @returns The radius of a circular handle, measured in widget pixels. */
qreal AbstractDiagram::handleRadius() const
{
    return handleOutlineThickness() * 2.5;
}

/** @brief The thickness of a color gradient.
 *
 * This is the thickness of a one-dimensional gradient, for example in
 * a slider or a color wheel.
 * @returns The thickness of a slider or a color wheel, measured in widget
 * coordinates.
 * @sa @ref gradientMinimumLength() */
int AbstractDiagram::gradientThickness() const
{
    ensurePolished();
    int result = 0;
    QStyleOptionSlider styleOption;
    styleOption.initFrom(this); // Sets also QStyle::State_MouseOver
    styleOption.orientation = Qt::Horizontal;
    result = qMax(result, style()->pixelMetric(QStyle::PM_SliderThickness, &styleOption, this));
    styleOption.orientation = Qt::Vertical;
    result = qMax(result, style()->pixelMetric(QStyle::PM_SliderThickness, &styleOption, this));
    result = qMax(result, qRound(handleRadius()));
    // QApplication::globalStrut() is the minimum.
    // gradientMinimumLength depends on the following checks!
    result = qMax(result, QApplication::globalStrut().width());
    result = qMax(result, QApplication::globalStrut().height());
    // No supplementary space for ticks is added.
    return result;
}

/** @brief The minimum length of a color gradient.
 *
 * This is the minimum length of a one-dimensional gradient, for example in
 * a slider or a color wheel. This is also the mimimum width and minimum
 * height of two-dimensional gradients.
 * @returns The length of a gradient, measured in widget
 * coordinates.
 * @sa @ref gradientThickness() */
int AbstractDiagram::gradientMinimumLength() const
{
    ensurePolished();
    QStyleOptionSlider option;
    option.initFrom(this);
    return qMax(
        // Parameter: style-based value:
        qMax(
            // Similar to QSlider sizeHint():
            84,
            // Similar to QSlider::minimumSizeHint():
            style()->pixelMetric(QStyle::PM_SliderLength, &option, this)),
        // Parameter: (Considers implicitly QApplication::globalStrut)
        gradientThickness()
    );
}

/** @brief The empty space around diagrams reserverd for the focus indicator.
 *
 * Measured in widget coordinates.
 *
 * @returns The empty space around diagrams reserverd for the focus
 * indicator. */
int AbstractDiagram::spaceForFocusIndicator() const
{
    return handleOutlineThickness()     // Space for the focus indicator itself
        + 2 * handleOutlineThickness(); // Add some more spacing
}

/** @brief An appropriate color for a handle, depending on the background
 * lightness.
 * @param lightness The background lightness. Valid range: <tt>[0, 100]</tt>.
 * @returns An appropriate color for a handle. This color will provide
 * contrast to the background. */
QColor AbstractDiagram::handleColorFromBackgroundLightness(qreal lightness) const
{
    if (lightness >= 50) {
        return Qt::black;
    }
    return Qt::white;
}

}
