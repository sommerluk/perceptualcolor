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
#include "PerceptualColor/chromahuediagram.h"
// Second, the private implementation.
#include "chromahuediagram_p.h"

#include "helper.h"
#include "lchvalues.h"
#include "polarpointf.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

namespace PerceptualColor
{
/** @brief The constructor.
 * @param colorSpace The color spaces within this widget should operate.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
ChromaHueDiagram::ChromaHueDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ChromaHueDiagramPrivate(this, colorSpace))
{
    // Setup LittleCMS. This is the first thing to do, because other
    // operations rely on a working LittleCMS.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Set focus policy
    // In Qt, usually focus (QWidget::hasFocus()) by mouse click is either
    // not accepted at all or accepted always for the hole rectangular
    // widget, depending on QWidget::focusPolicy(). This is not convenient
    // and intuitive for big, circular-shaped widgets like this one. It
    // would be nicer if the focus would only be accepted by mouse clicks
    // <em>within the circle itself</em>. Qt does not provide a build-in
    // way to do this. But a workaround to implement this behavior is
    // possible: Set QWidget::focusPolicy() to <em>not</em> accept focus
    // by mouse click. Then, reimplement mousePressEvent() and call
    // setFocus(Qt::MouseFocusReason) if the mouse click is within the
    // circle. Therefore, this class simply defaults to
    // Qt::FocusPolicy::TabFocus for QWidget::focusPolicy().
    setFocusPolicy(Qt::FocusPolicy::TabFocus);

    // Initialize the color
    setCurrentColor(LchValues::srgbVersatileInitialColor);
}

/** @brief Default destructor */
ChromaHueDiagram::~ChromaHueDiagram() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation.
 * @param colorSpace The color spaces within this widget should operate. */
ChromaHueDiagram::ChromaHueDiagramPrivate::ChromaHueDiagramPrivate(ChromaHueDiagram *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_chromaHueImage(colorSpace)
    , m_wheelImage(colorSpace)
    , q_pointer(backLink)
{
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * @internal
 * @post
 * - If the mouse is clicked with the circular diagram (inside or
 *   outside of the visible gamut), than this widget gets the focus
 *   and and @ref ChromaHueDiagramPrivate::m_isMouseEventActive is
 *   set to <tt>true</tt> to track mouse movements from now on.
 *   Reacts on all clicks (left, middle, right). If the mouse was
 *   within the gamut, the diagram’s handle is displaced there. If
 *   the mouse was outside the gamut, the diagram’s handle always stays
 *   within the gamut: The hue value is correctly retained, while the chroma
 *   value is the highest possible chroma within the gamut at this hue.
 * @endinternal
 *
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mousePressEvent(QMouseEvent *event)
{
    // TODO Also accept out-of-gamut clicks when they are covered by the
    // current handle.
    if (d_pointer->isWidgetPixelPositionWithinMouseSensibleCircle(event->pos())) {
        event->accept();
        // Mouse focus is handled manually because so we can accept
        // focus only on mouse clicks within the displayed gamut, while
        // rejecting focus otherwise. In the constructor, therefore
        // Qt::FocusPolicy::TabFocus is specified, so that manual handling
        // of mouse focus is up to this code here.
        setFocus(Qt::MouseFocusReason);
        // Enable mouse tracking from now on:
        d_pointer->m_isMouseEventActive = true;
        // As clicks are only accepted within the visible gamut, the mouse
        // cursor is made invisible. Its function is taken over by the
        // handle itself within the displayed gamut.
        setCursor(Qt::BlankCursor);
        // Set the color property
        d_pointer->setColorFromWidgetPixelPosition(event->pos());
        // Schedule a paint event, so that the wheel handle will show. It’s
        // not enough to hope setColorFromWidgetCoordinates() would do this,
        // because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color handle.
        update();
    } else {
        // Make sure default behavior like drag-window in KDE’s
        // “Breeze” widget style works if this widget does not
        // actually react itself on a mouse event.
        event->ignore();
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * @internal
 * @post Reacts only on mouse move events if
 * @ref ChromaHueDiagramPrivate::m_isMouseEventActive is <tt>true</tt>:
 * - If the mouse moves within the gamut, the diagram’s handle is displaced
 *   there. The mouse cursor is invisible; only the diagram’ handle is
 *   visible.
 * - If the mouse moves outside the gamut, the diagram’s handle always stays
 *   within the gamut: The hue value is correctly retained, while the chroma
 *   value is the highest possible chroma within the gamut at this hue.
 *   Both, the diagram’s handle <em>and</em> the mouse cursor are
 *   visible.
 * @endinternal
 *
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseMoveEvent(QMouseEvent *event)
{
    if (d_pointer->m_isMouseEventActive) {
        event->accept();
        const cmsCIELab lab = d_pointer->fromWidgetPixelPositionToLab(event->pos());
        if (d_pointer->isWidgetPixelPositionWithinMouseSensibleCircle(event->pos()) && d_pointer->m_rgbColorSpace->inGamut(lab)) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        d_pointer->setColorFromWidgetPixelPosition(event->pos());
    } else {
        // Make sure default behavior like drag-window in KDE’s
        // Breeze widget style works.
        event->ignore();
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Reacts on all clicks (left, middle, right).
 *
 * @internal
 * @post If @ref ChromaHueDiagramPrivate::m_isMouseEventActive is
 * <tt>true</tt> then:
 * - If the mouse is within the gamut, the diagram’s handle is displaced
 *   there.
 * - If the mouse moves outside the gamut, the diagram’s handle always stays
 *   within the gamut: The hue value is correctly retained, while the chroma
 *   value is the highest possible chroma within the gamut at this hue.
 * - The mouse cursor is made visible (if he wasn’t yet visible anyway).
 * - @ref ChromaHueDiagramPrivate::m_isMouseEventActive is set
 *   to <tt>false</tt>.
 * @endinternal
 *
 * @todo What if the widget displays a gamut that has no L*=0.1 because its
 * blackpoint is lighter.? Sacrifying chroma alone does not help? How to
 * react (for mouse input, keyboard input, but also API functions like
 * setColor()?
 *
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_pointer->m_isMouseEventActive) {
        event->accept();
        unsetCursor();
        d_pointer->m_isMouseEventActive = false;
        d_pointer->setColorFromWidgetPixelPosition(event->pos());
        // Schedule a paint event, so that the wheel handle will be hidden.
        // It’s not enough to hope setColorFromWidgetCoordinates() would do
        // this, because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color handle.
        update();
    } else {
        // Make sure default behavior like drag-window in KDE's
        // Breeze widget style works
        event->ignore();
    }
}

/** @brief React on a mouse wheel event.
 *
 * Reimplemented from base class.
 *
 * Scrolling up raises the hue value, scrolling down lowers the hue value.
 * Of course, at the point at 0°/360° wrapping applies.
 *
 * @param event The corresponding mouse event */
void ChromaHueDiagram::wheelEvent(QWheelEvent *event)
{
    if (
        // Do nothing while a the mouse is clicked and the mouse movement is
        // tracked anyway because this would be confusing for the user.
        (!d_pointer->m_isMouseEventActive) &&
        // Only react on good old vertical wheels,
        // and not on horizontal wheels.
        (event->angleDelta().y() != 0) &&
        // Only react on wheel events when then happen in the appropriate
        // area.a
        d_pointer->isWidgetPixelPositionWithinMouseSensibleCircle(event->pos())) {
        event->accept();
        // Calculate the new hue.
        // This may result in a hue smaller then 0° or bigger then 360°.
        // This is no problem because the constructor of FullColorDescription
        // will normalize the hue.
        d_pointer->m_currentColor.h += standardWheelSteps(event) * singleStepHue;
        setCurrentColor(d_pointer->m_rgbColorSpace->nearestInGamutSacrifyingChroma(d_pointer->m_currentColor));
    } else {
        event->ignore();
    }
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * The keys do not react in form of up, down, left and right like in
 * Cartesian coordinate systems. The keys change radial and angel like
 * in polar coordinate systems, because our color model is also based
 * on a polar coordinate system.
 *
 * For chroma changes: Moves the handle as much as possible into the
 * desired direction as long as this is still in the gamut.
 * - Qt::Key_Up increments chroma a small step
 * - Qt::Key_Down decrements chroma a small step
 * - Qt::Key_PageUp increments chroma a big step
 * - Qt::Key_PageDown decrements chroma a big step
 *
 * For hue changes: If necessary, the chroma value is reduced to get an
 * in-gamut color with the new hue.
 * - Qt::Key_Left increments hue a small step
 * - Qt::Key_Right decrements hue a small step
 * - Qt::Key_Home increments hue a big step
 * - Qt::Key_End decrements hue a big step
 *
 * @param event the event
 *
 * @todo Is this behavior really a good user experience? Or is it confusing
 * that left, right, up and down don’t do what was expected? What could be
 * more intuitive keys for changing radial and angle? At least the arrow keys
 * are likely that the user tries them out by trial-and-error. */
void ChromaHueDiagram::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        d_pointer->m_currentColor.c += singleStepChroma;
        break;
    case Qt::Key_Down:
        d_pointer->m_currentColor.c -= singleStepChroma;
        break;
    case Qt::Key_Left:
        d_pointer->m_currentColor.h += singleStepHue;
        break;
    case Qt::Key_Right:
        d_pointer->m_currentColor.h -= singleStepHue;
        break;
    case Qt::Key_PageUp:
        d_pointer->m_currentColor.c += pageStepChroma;
        break;
    case Qt::Key_PageDown:
        d_pointer->m_currentColor.c -= pageStepChroma;
        break;
    case Qt::Key_Home:
        d_pointer->m_currentColor.h += pageStepHue;
        break;
    case Qt::Key_End:
        d_pointer->m_currentColor.h -= pageStepHue;
        break;
    default:
        // Quote from Qt documentation:
        //
        //     “If you reimplement this handler, it is very important that
        //      you call the base class implementation if you do not act
        //      upon the key.
        //
        //      The default implementation closes popup widgets if the
        //      user presses the key sequence for QKeySequence::Cancel
        //      (typically the Escape key). Otherwise the event is
        //      ignored, so that the widget's parent can interpret it.“
        QWidget::keyPressEvent(event);
        return;
    }
    // Here we reach only if the key has been recognized. If not, in the
    // default branch of the switch statement, we would have passed the
    // keyPressEvent yet to the parent and returned.
    if (d_pointer->m_currentColor.c < 0) {
        // Do not allow negative chroma values.
        // (Doing so would be counter-intuitive.)
        d_pointer->m_currentColor.c = 0;
    }
    setCurrentColor(d_pointer->m_rgbColorSpace->nearestInGamutSacrifyingChroma(d_pointer->m_currentColor));
}

/** @brief Recommmended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize ChromaHueDiagram::sizeHint() const
{
    int mySize = qRound(
        // Considering the gradient length two times, as the diagram
        // shows the center of the coordinate system in the middle,
        // and each side of the center should be well visible.
        2 * d_pointer->diagramBorder() + 2 * gradientMinimumLength() * scaleFromMinumumSizeHintToSizeHint);
    return QSize(mySize, mySize);
}

/** @brief Recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref sizeHint() */
QSize ChromaHueDiagram::minimumSizeHint() const
{
    int mySize =
        // Considering the gradient length two times, as the diagram
        // shows the center of the coordinate system in the middle,
        // and each side of the center should be well visible.
        2 * d_pointer->diagramBorder() + 2 * gradientMinimumLength();
    return QSize(mySize, mySize);
}

// No documentation here (documentation of properties
// and its getters are in the header)
LchDouble ChromaHueDiagram::currentColor() const
{
    return d_pointer->m_currentColor;
}

/** @brief Setter for the @ref currentColor() property.
 *
 * @param newCurrentColor the new color */
void ChromaHueDiagram::setCurrentColor(const LchDouble &newCurrentColor)
{
    LchDouble newCorrectedColor = d_pointer->m_rgbColorSpace->nearestInGamutSacrifyingChroma(newCurrentColor);

    if (newCorrectedColor.hasSameCoordinates(d_pointer->m_currentColor)) {
        return;
    }

    LchDouble oldColor = d_pointer->m_currentColor;

    d_pointer->m_currentColor = newCorrectedColor;

    // Update, if necessary, the diagram.
    if (d_pointer->m_currentColor.l != oldColor.l) {
        d_pointer->m_chromaHueImage.setLightness(d_pointer->m_currentColor.l);
    }

    // Schedule a paint event:
    update();

    // Emit notify signal
    Q_EMIT currentColorChanged(newCorrectedColor);
}

/** @brief The point that is the center of the diagram coordinate system.
 * @returns The point that is the center of the diagram coordinate system,
 * measured in widget coordinates.
 * @sa @ref diagramOffset provides a one-dimensional
 * representation of this same fact. */
QPointF ChromaHueDiagram ::ChromaHueDiagramPrivate ::diagramCenterInWidgetCoordinates() const
{
    const qreal tempOffset {diagramOffset()};
    return QPointF(tempOffset, tempOffset);
}

/** @brief The point that is the center of the diagram coordinate system.
 * @returns The offset between the center of the widget coordinate system
 * and the diagram coordinate system. The value is measured in widget
 * coordinates. The value is identical for both, x axis and y axis.
 * @sa @ref diagramCenterInWidgetCoordinates provides a two-dimensional
 * representation of this same fact. */
qreal ChromaHueDiagram::ChromaHueDiagramPrivate::diagramOffset() const
{
    return q_pointer->maximumWidgetSquareSize() / static_cast<qreal>(2);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event */
void ChromaHueDiagram::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    // Update the widget content
    d_pointer->m_chromaHueImage.setImageSize(maximumPhysicalSquareSize());
    d_pointer->m_wheelImage.setImageSize(maximumPhysicalSquareSize());

    // As Qt documentation says:
    //     “The widget will be erased and receive a paint event
    //      immediately after processing the resize event. No
    //      drawing need be (or should be) done inside this handler.”
}

/** @brief  Widget coordinates corresponding to the @ref currentColor property
 * @returns Widget coordinates corresponding to the @ref currentColor property.
 * This is the position of @ref currentColor in the gamut diagram, but measured
 * and expressed in widget coordinates.
 * @sa @ref ChromaHueMeasurement "Measurement details" */
QPointF ChromaHueDiagram ::ChromaHueDiagramPrivate ::widgetCoordinatesFromCurrentColor() const
{
    const qreal scaleFactor = (q_pointer->maximumWidgetSquareSize() - 2 * diagramBorder()) / static_cast<qreal>(2 * m_maxChroma);
    QPointF currentColor = PolarPointF(m_currentColor.c, m_currentColor.h).toCartesian();
    return QPointF(currentColor.x() * scaleFactor + diagramOffset(), diagramOffset() - currentColor.y() * scaleFactor);
}

/** @brief Converts widget pixel positions to Lab coordinates
 * @param position The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to
 * be within the actual displayed diagram or even the gamut itself. It
 * might even be negative.
 * @returns The Lab coordinates of the currently displayed gamut diagram
 * for the (center of the) given pixel position.
 * @sa @ref ChromaHueMeasurement "Measurement details" */
cmsCIELab ChromaHueDiagram ::ChromaHueDiagramPrivate ::fromWidgetPixelPositionToLab(const QPoint position) const
{
    const qreal scaleFactor = static_cast<qreal>(2 * m_maxChroma) / (q_pointer->maximumWidgetSquareSize() - 2 * diagramBorder());
    // The pixel at position 0 0 has its top left border at position 0 0
    // and its bottom right border at position 1 1 and its center at
    // position 0.5 0.5. Its the center of the pixel that is our reference
    // for conversion, therefore we have to ship by 0.5 widget pixels.
    constexpr qreal pixelValueShift = 0.5;
    cmsCIELab lab;
    lab.L = m_currentColor.l;
    lab.a = (position.x() + pixelValueShift - diagramOffset()) * scaleFactor;
    lab.b = (position.y() + pixelValueShift - diagramOffset()) * scaleFactor * (-1);
    return lab;
}

/** @brief Sets the @ref currentColor property corresponding to a given widget
 * pixel position.
 *
 * @param position The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to be within the
 * actual displayed diagram or even the gamut itself. It might even be
 * negative.
 *
 * @post If the <em>center</em> of the widget pixel is within the represented
 * gamut, then the @ref currentColor property is set correspondingly. If the
 * center of the widget pixel is outside the gamut, then the chroma value is
 * reduced (while the hue is maintained) until arriving at the outer shell
 * of the gamut; the @ref currentColor property is than set to this adapted
 * color.
 *
 * @note This function works independently of the actually displayed color
 * gamut diagram. So if parts of the gamut (the high chroma parts) are cut
 * off in the visible diagram, this does not influence this function.
 *
 * @sa @ref ChromaHueMeasurement "Measurement details"
 *
 * @todo What when the mouse goes outside the gray circle, but more
 * gamut is available outside (because @ref m_maxChroma was chosen too small)?
 * For consistency, the handle of the diagram should stay within the gray
 * circle, and this should be interpretat also actually as the value at
 * the position of the handle. */
void ChromaHueDiagram ::ChromaHueDiagramPrivate ::setColorFromWidgetPixelPosition(const QPoint position)
{
    cmsCIELab lab = fromWidgetPixelPositionToLab(position);
    q_pointer->setCurrentColor(m_rgbColorSpace->nearestInGamutSacrifyingChroma(m_rgbColorSpace->toLch(lab)));
}

/** @brief Tests if a wiget pixel positon is within the mouse sensible circle.
 *
 * The mouse sensible circle contains the inner gray circle (on which the
 * gamut diagram is painted).
 * @param position The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to be within the
 * actual displayed diagram or even the gamut itself. It might even be
 * negative.
 * @returns <tt>true</tt> if the (center of the) pixel at the given position
 * is within the circle, <tt>false</tt> otherwise. */
bool ChromaHueDiagram ::ChromaHueDiagramPrivate ::isWidgetPixelPositionWithinMouseSensibleCircle(const QPoint position) const
{
    const qreal radial = PolarPointF(
                             // Position relative to polar coordinate system center:
                             position -
                             diagramCenterInWidgetCoordinates()
                             // Apply the offset between a pixel position and a point in
                             // the middle of this very same pixel:
                             + QPointF(0.5, 0.5))
                             .radial();

    const qreal diagramCircleRadius = q_pointer->maximumWidgetSquareSize() / static_cast<qreal>(2) - diagramBorder();

    return (radial <= diagramCircleRadius);
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * @internal
 * - Paints the widget. Takes the existing
 *   @ref ChromaHueDiagramPrivate::m_chromaHueImage and
 *   @ref ChromaHueDiagramPrivate::m_wheelImage and paints them on the widget.
 *   If their cache is up-to-date, this operation is fast, otherwise
 *   considerably slower.
 * - Paints the handles.
 * - If the widget has focus, it also paints the focus indicator. As the
 *   widget is round, we cannot use <tt>QStyle::PE_FrameFocusRect</tt> for
 *   painting this, neither does <tt>QStyle</tt> provide build-in support
 *   for round widgets. Therefore, we draw the focus indicator ourself,
 *   which means its form is not controlled by <tt>QStyle</tt>.
 * @endinternal
 *
 * @param event the paint event
 *
 * @todo What when @ref ChromaHueDiagramPrivate::m_currentColor has a valid
 * in-gamut color, but this color is out of the <em>displayed</em> diagram?
 * How to handle that? */
void ChromaHueDiagram::paintEvent(QPaintEvent *event)
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
    QImage buffer(maximumPhysicalSquareSize(), maximumPhysicalSquareSize(), QImage::Format_ARGB32_Premultiplied);
    buffer.fill(Qt::transparent);
    buffer.setDevicePixelRatio(devicePixelRatioF());

    // Other initialization
    QPainter bufferPainter(&buffer);
    QPen pen;
    const QBrush transparentBrush {Qt::transparent};
    // Set color of the handle: Black or white, depending on the lightness of
    // the currently selected color.
    const QColor handleColor {handleColorFromBackgroundLightness(d_pointer->m_currentColor.l)};
    const QPointF widgetCoordinatesFromCurrentColor {d_pointer->widgetCoordinatesFromCurrentColor()};

    // Paint the gamut itself as available in the cache.
    bufferPainter.setRenderHint(QPainter::Antialiasing, false);
    d_pointer->m_chromaHueImage.setBorder(d_pointer->diagramBorder() * devicePixelRatioF());
    d_pointer->m_chromaHueImage.setImageSize(maximumPhysicalSquareSize());
    d_pointer->m_chromaHueImage.setChromaRange(d_pointer->m_maxChroma);
    d_pointer->m_chromaHueImage.setLightness(d_pointer->m_currentColor.l);
    d_pointer->m_chromaHueImage.setDevicePixelRatioF(devicePixelRatioF());
    bufferPainter.drawImage(QPoint(0, 0),                          // position of the image
                            d_pointer->m_chromaHueImage.getImage() // image
    );

    // Paint a color wheel around
    bufferPainter.setRenderHint(QPainter::Antialiasing, false);
    // As devicePixelRatioF() might have changed, we make sure everything
    // is update before painting.
    d_pointer->m_wheelImage.setBorder(spaceForFocusIndicator() * devicePixelRatioF());
    d_pointer->m_wheelImage.setDevicePixelRatioF(devicePixelRatioF());
    d_pointer->m_wheelImage.setImageSize(maximumPhysicalSquareSize());
    d_pointer->m_wheelImage.setWheelThickness(gradientThickness() * devicePixelRatioF());
    bufferPainter.drawImage(QPoint(0, 0),                      // position of the image
                            d_pointer->m_wheelImage.getImage() // the image itself
    );

    // Paint a handle on the color wheel (only if a mouse event is
    // currently active).
    if (d_pointer->m_isMouseEventActive) {
        // The radius of the outer border of the color wheel
        const qreal radius = maximumWidgetSquareSize() / static_cast<qreal>(2) - spaceForFocusIndicator();
        // Get widget coordinate point for the handle
        QPointF myHandleInner = PolarPointF(radius - gradientThickness(), d_pointer->m_currentColor.h).toCartesian();
        myHandleInner.ry() *= -1; // Transform to Widget coordinates
        myHandleInner += d_pointer->diagramCenterInWidgetCoordinates();
        QPointF myHandleOuter = PolarPointF(radius, d_pointer->m_currentColor.h).toCartesian();
        myHandleOuter.ry() *= -1; // Transform to Widget coordinates
        myHandleOuter += d_pointer->diagramCenterInWidgetCoordinates();
        // Draw the line
        pen = QPen();
        pen.setWidth(handleOutlineThickness());
        // TODO Instead of Qt::FlatCap, we could really paint a handle
        // that does match perfectly the round inner and outer border
        // of the wheel. But: Is it really worth the complexity?
        pen.setCapStyle(Qt::FlatCap);
        pen.setColor(handleColor);
        bufferPainter.setPen(pen);
        bufferPainter.setRenderHint(QPainter::Antialiasing, true);
        bufferPainter.drawLine(myHandleInner, myHandleOuter);
    }

    // Paint the handle within the gamut
    bufferPainter.setRenderHint(QPainter::Antialiasing, true);
    pen = QPen();
    pen.setWidth(handleOutlineThickness());
    pen.setColor(handleColor);
    pen.setCapStyle(Qt::RoundCap);
    bufferPainter.setPen(pen);
    bufferPainter.setBrush(transparentBrush);
    bufferPainter.drawEllipse(widgetCoordinatesFromCurrentColor, // center
                              handleRadius(),                    // x radius
                              handleRadius()                     // y radius
    );
    const QPointF diagramCartesianCoordinatesFromCurrentColor {widgetCoordinatesFromCurrentColor.x() - d_pointer->diagramOffset(), (widgetCoordinatesFromCurrentColor.y() - d_pointer->diagramOffset()) * (-1)};
    PolarPointF diagramPolarCoordinatesFromCurrentColor {diagramCartesianCoordinatesFromCurrentColor};
    // lineRadial will be a point at the middle of the line thickness
    // of the circular handle.
    qreal lineRadial = diagramPolarCoordinatesFromCurrentColor.radial() - handleRadius();
    if (lineRadial > 0) {
        QPointF lineEndWidgetCoordinates = PolarPointF(lineRadial, diagramPolarCoordinatesFromCurrentColor.angleDegree()).toCartesian();
        lineEndWidgetCoordinates.ry() *= (-1);
        lineEndWidgetCoordinates += d_pointer->diagramCenterInWidgetCoordinates();
        bufferPainter.drawLine(
            // point 1 (center of the diagram):
            d_pointer->diagramCenterInWidgetCoordinates(),
            // point 2:
            lineEndWidgetCoordinates);
    }

    // Paint a focus indicator.
    //
    // We could paint a focus indicator (round or rectangular) around the
    // handle. Depending on the currently selected hue for the diagram, it
    // looks ugly because the colors of focus indicator and diagram do not
    // harmonize, or it is mostly invisible if the colors are similar. So
    // this approach does not work well.
    //
    // It seems better to paint a focus indicator for the whole widget.
    // We could use the style primitives to paint a rectangular focus
    // indicator around the whole widget:
    //
    // style()->drawPrimitive(
    //     QStyle::PE_FrameFocusRect,
    //     &option,
    //     &painter,
    //     this
    // );
    //
    // However, this does not work well because this widget does not have a
    // rectangular form.
    //
    // Then we have to design the line that we want to display. It is better
    // to do that ourselves instead of relying on generic QStyle::PE_Frame or
    // similar solutions as their result seems to be quite unpredictable
    // across various styles. So we use handleOutlineThickness as line width
    // and paint it at the left-most possible position. As m_wheelBorder
    // accommodates also to handleRadius(), the distance of the focus line to
    // the real widget also does, which looks nice.
    if (hasFocus()) {
        bufferPainter.setRenderHint(QPainter::Antialiasing, true);
        pen = QPen();
        pen.setWidth(handleOutlineThickness());
        pen.setColor(focusIndicatorColor());
        bufferPainter.setPen(pen);
        bufferPainter.setBrush(transparentBrush);
        bufferPainter.drawEllipse(
            // center:
            d_pointer->diagramCenterInWidgetCoordinates(),
            // x radius:
            (maximumWidgetSquareSize() - handleOutlineThickness()) / 2.0,
            // y radius:
            (maximumWidgetSquareSize() - handleOutlineThickness()) / 2.0);
    }

    // Paint the buffer to the actual widget
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing, false);
    widgetPainter.drawImage(QPoint(0, 0), buffer);
}

/** @brief The border around the round diagram.
 *
 * Measured in widget coordinates.
 *
 * @returns The border. This is the space where the surrounding color wheel
 * and the focus indicator are painted. */
int ChromaHueDiagram::ChromaHueDiagramPrivate::diagramBorder() const
{
    return
        // The space outside the wheel:
        q_pointer->spaceForFocusIndicator()
        // Add space for the wheel itself:
        + q_pointer->gradientThickness()
        // Add extra space between wheel and diagram:
        + 2 * q_pointer->handleOutlineThickness();
}

} // namespace PerceptualColor
