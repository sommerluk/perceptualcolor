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

#include "PerceptualColor/lchdouble.h"
#include "helper.h"
#include "lchvalues.h"
#include "polarpointf.h"

#include <math.h>

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

#include <lcms2.h>

namespace PerceptualColor
{
/** @brief Constructor
 *
 * @param colorSpace The color spaces within this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
ColorWheel::ColorWheel(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ColorWheelPrivate(this, colorSpace))
{
    // Setup LittleCMS (must be first thing because other operations
    // rely on working LittleCMS)
    d_pointer->m_rgbColorSpace = colorSpace;

    // Simple initialization
    // We don't use the reset methods as they would update the image/pixmap
    // each time, and this could crash if done before everything is
    // initialized.
    d_pointer->m_hue = LchValues::neutralHue;
    d_pointer->m_mouseEventActive = false;
    d_pointer->m_wheelImage.setBorder(d_pointer->border());
    d_pointer->m_wheelImage.setImageSize(qMin(width(), height()));
    d_pointer->m_wheelImage.setWheelThickness(gradientThickness());

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
 * @param colorSpace The color spaces within this widget should operate. */
ColorWheel::ColorWheelPrivate::ColorWheelPrivate(ColorWheel *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_wheelImage(colorSpace)
    , q_pointer(backLink)
{
}

/** @brief The diameter of the widget content
 *
 * @returns the diameter of the content of this widget, coordinates in pixel.
 * The content is always in form of a circle. This value includes the space
 * for the focus indicator, independently if currently the focus indicator
 * is actually displayed or not. This value corresponds to the smaller one
 * of width() and height().
 */
int ColorWheel::ColorWheelPrivate::contentDiameter() const
{
    return qMin(q_pointer->width(), q_pointer->height());
}

/** @brief Converts window coordinates to wheel coordinates.
 *
 * @param widgetCoordinates coordinates in the coordinate system of this widget
 * @returns “wheel” coordinates: Coordinates in a polar coordinate system who's
 * center is exactly in the middle of the displayed wheel.
 */
PolarPointF ColorWheel ::ColorWheelPrivate ::fromWidgetCoordinatesToWheelCoordinates(const QPoint widgetCoordinates) const
{
    qreal radius = contentDiameter() / static_cast<qreal>(2);
    return PolarPointF(QPointF(widgetCoordinates.x() - radius, radius - widgetCoordinates.y()));
}

/** @brief Converts wheel coordinates to widget coordinates.
 *
 * @param wheelCoordinates Coordinates in a polar coordinate system who's
 * center is exactly in the middle of the displayed wheel.
 * @returns coordinates in the coordinate system of this widget
 */
QPointF ColorWheel ::ColorWheelPrivate ::fromWheelCoordinatesToWidgetCoordinates(const PolarPointF wheelCoordinates) const
{
    qreal radius = contentDiameter() / static_cast<qreal>(2);
    QPointF temp = wheelCoordinates.toCartesian();
    temp.setX(temp.x() + radius);
    temp.setY(radius - temp.y());
    return temp;
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * Does not differentiate between left, middle and right mouse click.
 * If the mouse is clicked within the wheel ribbon, than the handle is placed
 * here and further mouse movements are tracked.
 *
 * @param event The corresponding mouse event
 */
void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    qreal radius = d_pointer->contentDiameter() / static_cast<qreal>(2) - d_pointer->border();
    PolarPointF myPolarPoint = d_pointer->fromWidgetCoordinatesToWheelCoordinates(event->pos());
    if (myPolarPoint.radial() > radius) {
        // Make sure default coordinates like drag-window
        // in KDE's Breeze widget style works
        event->ignore();
        return;
    }
    setFocus(Qt::MouseFocusReason);
    if (myPolarPoint.radial() > radius - gradientThickness()) {
        d_pointer->m_mouseEventActive = true;
        setHue(myPolarPoint.angleDegree());
    } else {
        // Make sure default coordinates like drag-window
        // in KDE's Breeze widget style works
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
 */
void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    if (d_pointer->m_mouseEventActive) {
        setHue(d_pointer->fromWidgetCoordinatesToWheelCoordinates(event->pos()).angleDegree());
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
 */
void ColorWheel::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_pointer->m_mouseEventActive) {
        d_pointer->m_mouseEventActive = false;
        setHue(d_pointer->fromWidgetCoordinatesToWheelCoordinates(event->pos()).angleDegree());
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
 * @param event The corresponding mouse event
 */
void ColorWheel::wheelEvent(QWheelEvent *event)
{
    // The step (coordinates in degree) that the hue angle is changed when
    // a mouse wheel event occurs.
    // TODO What is a reasonable value for this?
    static constexpr qreal wheelStep = 5;
    qreal radius = d_pointer->contentDiameter() / static_cast<qreal>(2) - d_pointer->border();
    PolarPointF myPolarPoint = d_pointer->fromWidgetCoordinatesToWheelCoordinates(event->pos());
    if (
        // Do nothing while mouse movement is tracked anyway. This would
        // be confusing.
        (!d_pointer->m_mouseEventActive) &&
        // Only react on wheel events when its in the wheel ribbon or in
        // the inner hole.
        (myPolarPoint.radial() <= radius) &&
        // Only react on good old vertical wheels, and not on horizontal wheels.
        (event->angleDelta().y() != 0)) {
        setHue(d_pointer->m_hue + standardWheelSteps(event) * wheelStep);
    } else {
        event->ignore();
    }
}

// TODO How to catch and treat invalid ranges? For QImage creation for
// example? Use also minimalSize()?

// TODO deliver also arrow keys to the child widget! WARNING DANGER

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * Reacts on key press events. When the plus key or the minus key are pressed,
 * it raises or lowers the hue. When Qt::Key_Insert or Qt::Key_Delete are
 * pressed, it raises or lowers the hue faster.
 *
 * @param event the paint event
 */
void ColorWheel::keyPressEvent(QKeyEvent *event)
{
    constexpr qreal wheelStep = 5;
    constexpr qreal bigWheelStep = 15;
    switch (event->key()) {
    case Qt::Key_Plus:
        setHue(d_pointer->m_hue + wheelStep);
        break;
    case Qt::Key_Minus:
        setHue(d_pointer->m_hue - wheelStep);
        break;
    case Qt::Key_Insert:
        setHue(d_pointer->m_hue + bigWheelStep);
        break;
    case Qt::Key_Delete:
        setHue(d_pointer->m_hue - bigWheelStep);
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

/* TODO Support more mouse buttons?
 *
 * enum Qt::MouseButton
 * flags Qt::MouseButtons
 *
 * Qt::BackButton	0x00000008
 * The 'Back' button. (Typically present on the 'thumb' side of a mouse
 * with extra buttons. This is NOT the tilt wheel.)
 *
 * Qt::ForwardButton	0x00000010
 * The 'Forward' Button. (Typically present beside the 'Back' button, and
 * also pressed by the thumb.)
 */

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * Paints the widget. Takes the existing m_wheelPixmap and paints
 * them on the widget. Paints, if appropriate, the focus indicator.
 * Paints the handle. Relies on that m_wheelPixmap are up to date.
 *
 * @param event the paint event
 *
 * @internal
 *
 * @todo Make the wheel to be drawn horizontally and vertically aligned.
 *
 * @todo Better design on small widget sizes
 */
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
    QImage paintBuffer(size(), QImage::Format_ARGB32_Premultiplied);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);

    // paint the wheel from the cache
    painter.drawImage(0, 0, d_pointer->m_wheelImage.getImage());

    // paint the handle
    qreal radius = d_pointer->contentDiameter() / static_cast<qreal>(2) - d_pointer->border();
    // get widget coordinates for our handle
    QPointF myHandleInner = d_pointer->fromWheelCoordinatesToWidgetCoordinates(PolarPointF(radius - gradientThickness(), d_pointer->m_hue));
    QPointF myHandleOuter = d_pointer->fromWheelCoordinatesToWidgetCoordinates(PolarPointF(radius, d_pointer->m_hue));
    // draw the line
    QPen pen;
    pen.setWidth(handleOutlineThickness());
    pen.setCapStyle(Qt::FlatCap);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(myHandleInner, myHandleOuter);

    // Paint a focus indicator if the widget has the focus
    if (hasFocus()) {
        pen.setWidth(handleOutlineThickness());
        pen.setColor(focusIndicatorColor());
        painter.setPen(pen);
        painter.drawEllipse(handleOutlineThickness() / 2, // Integer division (rounding down)
                            handleOutlineThickness() / 2, // Integer division (rounding down)
                            d_pointer->contentDiameter() - handleOutlineThickness(),
                            d_pointer->contentDiameter() - handleOutlineThickness());
    }

    // Paint the buffer to the actual widget
    QPainter(this).drawImage(0, 0, paintBuffer);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event
 */
void ColorWheel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // TODO The image cache is not necessarily invalid now. Thought the widget
    // was resized, the image itself might stay in the same size. See also
    // same problem for ChromaHueDiagram and for ChromaLightnessDiagram
    // ChromaLightnessDiagram’s resize() call done here within the child class
    // WheelColorPicker. This situation is relevant for the real-world usage,
    // when the user scales the window only horizontally or only vertically.
    d_pointer->m_wheelImage.setImageSize(qMin(size().width(), size().height()));
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

/** @brief the chroma with which the wheel ribbon is painted. */
qreal ColorWheel::ColorWheelPrivate::wheelRibbonChroma() const
{
    return LchValues::srgbVersatileChroma;
}

/**
 * Set the hue property. The hue property is the hue angle degree in the
 * range from 0 to 360, where the circle is again at its beginning. The
 * value is gets normalized to this range. So
 * \li 0 gets 0
 * \li 359.9 gets 359.9
 * \li 360 gets 0
 * \li 361.2 gets 1.2
 * \li 720 gets 0
 * \li -1 gets 359
 * \li -1.3 gets 358.7
 *
 * After changing the hue property, the widget gets updated.
 * @param newHue The new hue value to set.
 */
void ColorWheel::setHue(const qreal newHue)
{
    qreal temp = PolarPointF::normalizedAngleDegree(newHue);
    if (d_pointer->m_hue != temp) {
        d_pointer->m_hue = temp;
        Q_EMIT hueChanged(d_pointer->m_hue);
        update();
    }
}

// TODO The widget content should be centered horizontally and vertically

/** @brief Provide the size hint.
 *
 * Reimplemented from base class.
 *
 * @returns the size hint
 *
 * @sa minimumSizeHint()
 */
QSize ColorWheel::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

/** @brief Provide the minimum size hint.
 *
 * Reimplemented from base class.
 *
 * @returns the minimum size hint
 *
 * @sa sizeHint()
 */
QSize ColorWheel::minimumSizeHint() const
{
    // We interpretate the gradientMinimumLength() as the length of the
    // circumference of the inner circle of the wheel. By dividing it by π
    // we get the requiered inner diameter:
    const qreal innerDiameter = gradientMinimumLength() / M_PI;
    const int size = qRound(innerDiameter + 2 * gradientThickness() + 2 * d_pointer->border());
    // Expand to the global minimum size for GUI elements
    return QSize(size, size).expandedTo(QApplication::globalStrut());
}

// TODO What when some of the wheel colors are out of gamut?

/** @brief Reset the hue() property. */
void ColorWheel::resetHue()
{
    setHue(LchValues::neutralHue);
}

int ColorWheel::ColorWheelPrivate::border() const
{
    return 2 * q_pointer->handleOutlineThickness();
}

/** @brief The inner diameter of the color wheel.
 * @returns The inner diameter of the color wheel, measured in
 * widget pixel. This is the diameter of the empty circle
 * within the color wheel. */
int ColorWheel::ColorWheelPrivate::innerDiameter() const
{
    return contentDiameter() - 2 * q_pointer->gradientThickness() - 2 * border();
}

} // namespace PerceptualColor
