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
#include "PerceptualColor/colorwheel.h"
// Second, the private implementation.
#include "colorwheel_p.h"

#include "helper.h"
#include "lchvalues.h"
#include "polarpointf.h"

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace PerceptualColor
{
/** @brief Constructor
 *
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
ColorWheel::ColorWheel(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ColorWheelPrivate(this, colorSpace))
{
    // Setup the color space must be the first thing to do because
    // other operations rely on a working color space.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Initialization
    d_pointer->m_hue = LchValues::neutralHue;

    // Set focus policy
    // In Qt, usually focus (QWidget::hasFocus()) by mouse click is
    // either not accepted at all or accepted always for the hole rectangular
    // widget, depending on QWidget::focusPolicy(). This is not
    // convenient and intuitive for big, circular-shaped widgets like this one.
    // It would be nicer if the focus would only be accepted by mouse clicks
    // <em>within the circle itself</em>. Qt does not provide a build-in way to
    // do this. But a workaround to implement this behavior is possible: Set
    // QWidget::focusPolicy() to <em>not</em> accept focus by mouse
    // click. Then, reimplement mousePressEvent() and call
    // setFocus(Qt::MouseFocusReason) if the mouse click is within the
    // circle. Therefore, this class simply defaults to
    // Qt::FocusPolicy::TabFocus for QWidget::focusPolicy().
    setFocusPolicy(Qt::FocusPolicy::TabFocus);
}

/** @brief Default destructor */
ColorWheel::~ColorWheel() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation.
 *
 * @param colorSpace The color space within which this widget should operate. */
ColorWheel::ColorWheelPrivate::ColorWheelPrivate(ColorWheel *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_wheelImage(colorSpace)
    , q_pointer(backLink)
{
}

/** @brief Convert widget coordinate points to wheel coordinate points.
 *
 * @param widgetCoordinatePoint A coordinates point relative to the coordinate
 * system of this widget. Measured in <em>device-independant pixels</em>.
 *
 * @returns The same coordinate point relative to a polar coordinate system
 * who’s center is exactly in the middle of the displayed wheel. Measured
 * in <em>device-independant pixels</em>.
 *
 * @sa @ref fromWheelToWidgetCoordinates */
PolarPointF ColorWheel ::ColorWheelPrivate ::fromWidgetToWheelCoordinates(const QPoint widgetCoordinatePoint) const
{
    const qreal radius = q_pointer->maximumWidgetSquareSize() / 2.0;
    const QPointF temp {widgetCoordinatePoint.x() - radius, radius - widgetCoordinatePoint.y()};
    return PolarPointF(temp);
}

/** @brief Convert wheel coordinate points to widget coordinate points.
 *
 * @param wheelCoordinates A coordinate point relative to a polar coordinate
 * system who’s center is exactly in the middle of the displayed wheel.
 * Measured in <em>device-independant pixels</em>.
 *
 * @returns The same coordinate point relative to the coordinate system of
 * this widget. Measured in <em>device-independant pixels</em>.
 *
 * @sa @ref fromWidgetToWheelCoordinates */
QPointF ColorWheel ::ColorWheelPrivate ::fromWheelToWidgetCoordinates(const PolarPointF wheelCoordinates) const
{
    const qreal radius = q_pointer->maximumWidgetSquareSize() / 2.0;
    QPointF result = wheelCoordinates.toCartesian();
    result.setX(result.x() + radius);
    result.setY(radius - result.y());
    return result;
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * Does not differentiate between left, middle and right mouse click.
 *
 * If the mouse is clicked within the wheel ribbon, than the handle is placed
 * here and further mouse movements are tracked.
 *
 * @param event The corresponding mouse event
 *
 * @internal
 *
 * @sa @ref ColorWheelPrivate::m_mouseEventActive */
void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    const qreal radius = maximumWidgetSquareSize() / 2.0 - spaceForFocusIndicator();
    PolarPointF myPolarPoint = d_pointer->fromWidgetToWheelCoordinates(event->pos());

    // Ignare clicks outside the wheel
    if (myPolarPoint.radial() > radius) {
        // Make sure default coordinates like drag-window
        // in KDE's Breeze widget style works:
        event->ignore();
        return;
    }

    // If inside the wheel (either in the wheel ribbon itself or in the hole
    // in the middle), take focus:
    setFocus(Qt::MouseFocusReason);

    if (myPolarPoint.radial() > radius - gradientThickness()) {
        d_pointer->m_mouseEventActive = true;
        setHue(myPolarPoint.angleDegree());
    } else {
        // Make sure default coordinates like drag-window
        // in KDE's Breeze widget style works:
        event->ignore();
    }

    return;
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * Reacts only on mouse move events if previously there had been a mouse press
 * event that had been accepted.
 *
 * If previously there had not been a mouse press event, the mouse move event
 * is ignored.
 *
 * @param event The corresponding mouse event
 *
 * @internal
 *
 * @sa @ref ColorWheelPrivate::m_mouseEventActive */
void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    if (d_pointer->m_mouseEventActive) {
        setHue(d_pointer->fromWidgetToWheelCoordinates(event->pos()).angleDegree());
    } else {
        // Make sure default coordinates like drag-window in KDE's Breeze
        // widget style works
        event->ignore();
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Does not differentiate between left,
 * middle and right mouse click.
 *
 * @param event The corresponding mouse event
 *
 * @internal
 *
 * @sa @ref ColorWheelPrivate::m_mouseEventActive
 *
 * @sa @ref ColorWheelPrivate::m_mouseEventActive */
void ColorWheel::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_pointer->m_mouseEventActive) {
        d_pointer->m_mouseEventActive = false;
        setHue(d_pointer->fromWidgetToWheelCoordinates(event->pos()).angleDegree());
    } else {
        // Make sure default coordinates like drag-window in KDE's Breeze
        // widget style works
        event->ignore();
    }
}

/** @brief React on a mouse wheel event.
 *
 * Reimplemented from base class.
 *
 * Scrolling up raises the hue value, scrolling down lowers the hue value.
 * Of course, the point at 0°/360° it not blocking.
 *
 * @param event The corresponding mouse event */
void ColorWheel::wheelEvent(QWheelEvent *event)
{
    const qreal radius = maximumWidgetSquareSize() / 2.0 - spaceForFocusIndicator();
    PolarPointF myPolarPoint = d_pointer->fromWidgetToWheelCoordinates(event->pos());
    if (
        // Do nothing while mouse movement is tracked anyway. This would
        // be confusing:
        (!d_pointer->m_mouseEventActive)
        // Only react on wheel events when its in the wheel ribbon or in
        // the inner hole:
        && (myPolarPoint.radial() <= radius)
        // Only react on good old vertical wheels, and not on horizontal wheels:
        && (event->angleDelta().y() != 0)
        // then:
    ) {
        setHue(d_pointer->m_hue + standardWheelStepCount(event) * singleStepHue);
    } else {
        event->ignore();
    }
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * Reacts on key press events. When the <em>plus</em> key or the <em>minus</em>
 * key are pressed, it raises or lowers the hue. When <tt>Qt::Key_Insert</tt>
 * or <tt>Qt::Key_Delete</tt> are pressed, it raises or lowers the hue faster.
 *
 * @param event the corresponding event
 *
 * @internal
 *
 * @todo The keys are chosen to not conflict with @ref ChromaHueDiagram. But:
 * They are a little strange. Does this really make sense? */
void ColorWheel::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        setHue(d_pointer->m_hue + singleStepHue);
        break;
    case Qt::Key_Minus:
        setHue(d_pointer->m_hue - singleStepHue);
        break;
    case Qt::Key_Insert:
        setHue(d_pointer->m_hue + pageStepHue);
        break;
    case Qt::Key_Delete:
        setHue(d_pointer->m_hue - pageStepHue);
        break;
    default:
        /* Quote from Qt documentation:
         *
         * If you reimplement this handler, it is very important
         * that you call the base class implementation if you do not
         * act upon the key.
         *
         * The default implementation closes popup widgets if the user
         * presses the key sequence for QKeySequence::Cancel (typically
         * the Escape key). Otherwise the event is ignored, so that the
         * widget's parent can interpret it. */
        QWidget::keyPressEvent(event);
        break;
    }
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * Paints the widget.
 *
 * @param event the paint event
 *
 * @internal
 *
 * The wheel is painted using @ref ColorWheelPrivate::m_wheelImage.
 * The focus indicator (if any) and the handle are painted on-the-fly.
 *
 * @todo Make the wheel to be drawn horizontally and vertically aligned?? Or
 * better top-left aligned for LTR layouts and top-right aligned for RTL
 * layouts?
 *
 * @todo Better design (smaller wheel ribbon?) for small widget sizes */
void ColorWheel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // We do not paint directly on the widget, but on a QImage buffer first:
    // Render anti-aliased looks better. But as Qt documentation says:
    //
    //      “Renderhints are used to specify flags to QPainter that may or
    //       may not be respected by any given engine.”
    //
    // Painting here directly on the widget might lead to different
    // anti-aliasing results depending on the underlying window system. This
    // is especially problematic as anti-aliasing might shift or not a pixel
    // to the left or to the right. So we paint on a QImage first. As QImage
    // (at difference to QPixmap and a QWidget) is independent of native
    // platform rendering, it guarantees identical anti-aliasing results on
    // all platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should
    //       use the platform independent QImage as paint device; i.e. using
    //       QImage will ensure that the result has an identical pixel
    //       representation on any platform.”
    QImage paintBuffer(maximumPhysicalSquareSize(),        // width
                       maximumPhysicalSquareSize(),        // height
                       QImage::Format_ARGB32_Premultiplied // format
    );
    paintBuffer.fill(Qt::transparent);
    paintBuffer.setDevicePixelRatio(devicePixelRatioF());
    QPainter bufferPainter(&paintBuffer);

    // Paint the color wheel
    bufferPainter.setRenderHint(QPainter::Antialiasing, false);
    // As devicePixelRatioF() might have changed, we make sure everything
    // that might depend on devicePixelRatioF() is updated before painting.
    d_pointer->m_wheelImage.setBorder(spaceForFocusIndicator() * devicePixelRatioF());
    d_pointer->m_wheelImage.setDevicePixelRatioF(devicePixelRatioF());
    d_pointer->m_wheelImage.setImageSize(maximumPhysicalSquareSize());
    d_pointer->m_wheelImage.setWheelThickness(gradientThickness() * devicePixelRatioF());
    bufferPainter.drawImage(QPoint(0, 0),                      // image position (top-left)
                            d_pointer->m_wheelImage.getImage() // the image itself
    );

    // Paint the handle
    const qreal wheelOuterRadius = maximumWidgetSquareSize() / 2.0 - spaceForFocusIndicator();
    // Get widget coordinates for the handle
    QPointF myHandleInner = d_pointer->fromWheelToWidgetCoordinates(
        // Inner point at the wheel:
        PolarPointF(wheelOuterRadius - gradientThickness(), // x
                    d_pointer->m_hue                        // y
                    ));
    QPointF myHandleOuter = d_pointer->fromWheelToWidgetCoordinates(
        // Outer point at the wheel:
        PolarPointF(wheelOuterRadius, d_pointer->m_hue));
    // Draw the line
    QPen pen;
    pen.setWidth(handleOutlineThickness());
    pen.setCapStyle(Qt::FlatCap);
    pen.setColor(Qt::black);
    bufferPainter.setPen(pen);
    bufferPainter.setRenderHint(QPainter::Antialiasing, true);
    bufferPainter.drawLine(myHandleInner, myHandleOuter);

    // Paint a focus indicator if the widget has the focus
    if (hasFocus()) {
        bufferPainter.setRenderHint(QPainter::Antialiasing, true);
        pen = QPen();
        pen.setWidth(handleOutlineThickness());
        pen.setColor(focusIndicatorColor());
        bufferPainter.setPen(pen);
        const qreal center = maximumWidgetSquareSize() / 2.0;
        bufferPainter.drawEllipse(
            // center:
            QPointF(center, center),
            // x radius:
            center - handleOutlineThickness() / 2.0,
            // y radius:
            center - handleOutlineThickness() / 2.0);
    }

    // Paint the buffer to the actual widget
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing, false);
    widgetPainter.drawImage(QPoint(0, 0), paintBuffer);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event */
void ColorWheel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    // Update the widget content
    d_pointer->m_wheelImage.setImageSize(maximumPhysicalSquareSize());
    /* As by Qt documentation:
     *     “The widget will be erased and receive a paint event immediately
     *      after processing the resize event. No drawing need be (or should
     *      be) done inside this handler.” */
}

// No documentation here (documentation of properties
// and its getters are in the header)
qreal ColorWheel::hue() const
{
    return d_pointer->m_hue;
}

/** @brief Setter for the @ref hue property.
 *  @param newHue the new hue */
void ColorWheel::setHue(const qreal newHue)
{
    qreal temp = PolarPointF::normalizedAngleDegree(newHue);
    if (d_pointer->m_hue != temp) {
        d_pointer->m_hue = temp;
        Q_EMIT hueChanged(d_pointer->m_hue);
        update();
    }
}

/** @brief Recommmended size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize ColorWheel::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

/** @brief Recommmended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref sizeHint() */
QSize ColorWheel::minimumSizeHint() const
{
    // We interpretate the gradientMinimumLength() as the length between two
    // poles of human perception. Around the wheel, there are four of them
    // (0° red, 90° yellow, 180° green, 270° blue). So the circumference of
    // the inner circle of the wheel is 4 × gradientMinimumLength(). By
    // dividing it by π, we get the requiered inner diameter:
    const qreal innerDiameter = 4 * gradientMinimumLength() / M_PI;
    const int size = qRound(innerDiameter + 2 * gradientThickness() + 2 * spaceForFocusIndicator());
    // Expand to the global minimum size for GUI elements
    return QSize(size, size).expandedTo(QApplication::globalStrut());
}

/** @brief The empty space around the reserverd for the focus indicator.
 *
 * This is a simple redirect of @ref AbstractDiagram::spaceForFocusIndicator().
 * It is meant to allow access to friend classes of @ref ColorWheel.
 *
 * Measured in <em>device-independant pixels</em>.
 *
 * @returns The empty space around diagrams (distance between widget outline
 * and color wheel outline) reserverd for the focus indicator. */
int ColorWheel::ColorWheelPrivate::border() const
{
    return q_pointer->spaceForFocusIndicator();
}

/** @brief The inner diameter of the color wheel.
 *
 * It is meant to allow access to friend classes of @ref ColorWheel.
 *
 * @returns The inner diameter of the color wheel, measured in
 * <em>device-independant pixels</em>. This is the diameter of the empty
 * circle within the color wheel.
 *
 * @todo Measured in physical pixel or device-independant pixel? */
qreal ColorWheel::ColorWheelPrivate::innerDiameter() const
{
    return
        // Size for the widget:
        q_pointer->maximumWidgetSquareSize()
        // Reduce space for the wheel ribbon:
        - 2 * q_pointer->gradientThickness()
        // Reduce space for  the focus indicator (border around wheel ribbon):
        - 2 * q_pointer->spaceForFocusIndicator();
}

} // namespace PerceptualColor
