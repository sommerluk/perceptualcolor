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
    return palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight);
}

/** @brief The rounded size of the widget measured in
 * <em>physical pixels</em>.
 *
 * @returns The rounded size of this widget,
 * measured in <em>physical pixels</em>, based on
 * <tt>QPaintDevice::devicePixelRatioF()</tt>. This is the recommended
 * image size for calling <tt>QPainter::drawImage()</tt> during a paint event.
 *
 * Example: You want to prepare a <tt>QImage</tt> of the hole widget to be
 * used in <tt>QWidget::paintEvent()</tt>. To make sure a crisp rendering,
 * you have to
 *
 * - Prepare an image with the size that this function returns.
 * - Set <tt>QImage::setDevicePixelRatio()</tt> of the image to the same
 *   value as <tt>QPaintDevice::devicePixelRatioF()</tt> of the widget.
 * - Actually paint the image on the widget at position <tt>(0, 0)</tt>
 *   <em>without</em> anti-aliasing.
 *
 * @note If <tt>QPaintDevice::devicePixelRatioF()</tt> is not an integer,
 * the result of this function is rounded down. Qt’s widget geometry code
 * has no documentation about how this is handeled. However, Qt seems to
 * round up starting with 0.5, at least on Linux/X11. But there are a few
 * themes (for example the “Kvantum style engine” with the style
 * “MildGradientKvantum”) that seem to round down: This becomes visible, as
 * the corresponding last physical pixels are not automatically redrawn before
 * executing the <tt>paintEvent()</tt> code. To avoid relying on undocumented
 * behaviour and to avoid known problems with some styles, this function
 * is concervative and always rounds down. */
QSize AbstractDiagram::physicalPixelSize() const
{
    // Assert that static_cast<int> always rounds down.
    static_assert(static_cast<int>(1.9) == 1);
    static_assert(static_cast<int>(1.5) == 1);
    static_assert(static_cast<int>(1.0) == 1);
    // Multiply the size with the (floating point) scale factor
    // and than round down (by using static_cast<int>).
    const int width = static_cast<int>(size().width() * devicePixelRatioF());
    const int height = static_cast<int>(size().height() * devicePixelRatioF());
    return QSize(width, height);
}

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>physical pixels</em>.
 *
 * This is the shorter value of width and height of the widget.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>physical pixels</em>.
 *
 * @sa @ref maximumWidgetSquareSize */
int AbstractDiagram::maximumPhysicalSquareSize() const
{
    return qMin(physicalPixelSize().width(), physicalPixelSize().height());
}

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>device-independant pixels</em>.
 *
 * This is the conversion of @ref maximumPhysicalSquareSize to the unit
 * <em>device-independent pixels</em>. It might be <em>smaller</em> than
 * the shortest value of <tt>QWdiget::width()</tt> and
 * <tt>QWidget::height()</tt> because @ref maximumPhysicalSquareSize
 * might have rounded down.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>device-independant pixels</em>. */
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
 * crisp (correctly scaled) images also for high-DPI devices.
 * The painting does not use floating point drawing, but rounds
 * to full integers. Therefore, the result is always a sharp image.
 * This function takes care that each square has the same physical pixel
 * size, without scaling errors or anti-aliasing errors.
 *
 * @internal
 * @sa @ref transparencyBackground(qreal devicePixelRatioF)
 * @endinternal */
QImage AbstractDiagram::transparencyBackground() const
{
    return PerceptualColor::transparencyBackground(devicePixelRatioF());
}

/** @brief The outline thickness of a handle.
 *
 * @returns The outline thickness of a (either circular or linear) handle.
 * Measured in <em>device-independant pixels</em>. */
int AbstractDiagram::handleOutlineThickness() const
{
    /** @note The return value is constant. For a given object instance, this
     * function returns the same value every time it is called. This constant
     * value may be different for different instances of the object. */
    return 2;
}

/** @brief The radius of a circular handle.
 * @returns The radius of a circular handle, measured in
 * <em>device-independant pixels</em>. */
qreal AbstractDiagram::handleRadius() const
{
    /** @note The return value is constant. For a given object instance, this
     * function returns the same value every time it is called. This constant
     * value may be different for different instances of the object. */
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
        gradientThickness());
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

} // namespace PerceptualColor
