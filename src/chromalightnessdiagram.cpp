// // SPDX-License-Identifier: MIT
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
#include "chromalightnessdiagram.h"
// Second, the private implementation.
#include "chromalightnessdiagram_p.h"

#include "helper.h"
#include "lchvalues.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QtMath>

namespace PerceptualColor
{
/** @brief The constructor.
 *
 * @param colorSpace The color space within which the widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * @param parent Passed to the QWidget base class constructor */
ChromaLightnessDiagram::ChromaLightnessDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ChromaLightnessDiagramPrivate(this, colorSpace))
{
    // Setup the color space must be the first thing to do because
    // other operations rely on a working color space.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Initialization
    d_pointer->m_currentColor = LchValues::srgbVersatileInitialColor;
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d_pointer->m_chromaLightnessImage.setImageSize(
        // Update the image size will free memory of the cache inmediatly.
        d_pointer->calculateImageSizePhysical());
}

/** @brief Default destructor */
ChromaLightnessDiagram::~ChromaLightnessDiagram() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation.
 * @param colorSpace The color space within which this widget should operate. */
ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
    : m_chromaLightnessImage(colorSpace)
    , q_pointer(backLink)
{
}

/** Updates @ref currentColor corresponding to the given widget pixel position.
 *
 * @param widgetPixelPosition The position of a pixel within the widget’s
 * coordinate system. This does not necessarily need to intersect with the
 * actually displayed diagram or the gamut. It might even be negative or
 * outside the widget.
 *
 * @post If the pixel position is within the gamut, then
 * the corresponding @ref currentColor is set. If the pixel position
 * is outside the gamut, than a nearby in-gamut color is set (hue is
 * preverved, chroma and lightness are adjusted). Exception: If the
 * widget is so small that no diagram is displayed, nothing will happen. */
void ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::setCurrentColorFromWidgetPixelPosition(const QPoint widgetPixelPosition)
{
    const LchDouble color = fromWidgetPixelPositionToColor(widgetPixelPosition);
    q_pointer->setCurrentColor(
        // Search for the nearest color without changing the hue:
        m_rgbColorSpace->nearestInGamutColorByAdjustingChromaLightness(color));
}

/** @brief The border between the widget outer top, right and bottom
 * border and the diagram itself.
 *
 * The diagram is not painted on the whole extend of the widget.
 * A border is left to allow that the selection handle can be painted
 * completely even when a pixel on the border of the diagram is
 * selected.
 *
 * This is the value for the top, right and bottom border. For the left
 * border, see @ref leftBorderPhysical() instead.
 *
 * Measured in <em>physical pixels</em>. */
int ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::defaultBorderPhysical() const
{
    const qreal border = q_pointer->handleRadius() + q_pointer->handleOutlineThickness() / 2.0;
    return qCeil(border * q_pointer->devicePixelRatioF());
}

/** @brief The left border between the widget outer left border and the
 * diagram itself.
 *
 * The diagram is not painted on the whole extend of the widget.
 * A border is left to allow that the selection handle can be painted
 * completely even when a pixel on the border of the diagram is
 * selected. Also, there is space left for the focus indicator.
 *
 * This is the value for the left border. For the other three borders,
 * see @ref defaultBorderPhysical() instead.
 *
 * Measured in <em>physical pixels</em>. */
int ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::leftBorderPhysical() const
{
    const int focusIndicatorThickness = qCeil(q_pointer->handleOutlineThickness() * q_pointer->devicePixelRatioF());

    // Candidate 1:
    const int candidateOne = defaultBorderPhysical() + focusIndicatorThickness;

    // Candidate 2: Generally recommended value for focus indicator:
    const int candidateTwo = qCeil(q_pointer->spaceForFocusIndicator() * q_pointer->devicePixelRatioF());

    return qMax(candidateOne, candidateTwo);
}

/** @brief Calculate a size for @ref m_chromaLightnessImage that corresponds
 * to the current widget size.
 *
 * @returns The size for @ref m_chromaLightnessImage that corresponds
 * to the current widget size. Measured in <em>physical pixels</em>. */
QSize ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::calculateImageSizePhysical() const
{
    const QSize borderSizePhysical(
        // Borders:
        leftBorderPhysical() + defaultBorderPhysical(), // left + right
        2 * defaultBorderPhysical()                     // top + bottom
    );
    return q_pointer->physicalPixelSize() - borderSizePhysical;
}

/** @brief Converts widget pixel positions to color.
 *
 * @param widgetPixelPosition The position of a pixel of the widget
 * coordinate system. The given value  does not necessarily need to
 * be within the actual displayed widget. It might even be negative.
 *
 * @returns The corresponding color for the (center of the) given
 * widget pixel position. (The value is not normalized. It might have
 * a negative C value if the position is on the left of the diagram,
 * or an L value smaller than 0 or bigger than 100…) Exception: If
 * the widget is too small to show a diagram, a default color is
 * returned.
 *
 * @sa @ref measurementdetails */
LchDouble ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::fromWidgetPixelPositionToColor(const QPoint widgetPixelPosition) const
{
    const QPointF offset(leftBorderPhysical(), defaultBorderPhysical());
    const QPointF imageCoordinatePoint = widgetPixelPosition
        // Offset to pass from widget reference system to image reference system:
        - offset / q_pointer->devicePixelRatioF()
        // Offset to pass from pixel positions to coordinate points:
        + QPointF(0.5, 0.5);
    LchDouble color;
    color.h = m_currentColor.h;
    const qreal diagramHeight = calculateImageSizePhysical().height() / q_pointer->devicePixelRatioF();
    if (diagramHeight > 0) {
        color.l = imageCoordinatePoint.y() * 100.0 / diagramHeight * (-1.0) + 100.0;
        color.c = imageCoordinatePoint.x() * 100.0 / diagramHeight;
    } else {
        color.l = 50;
        color.c = 0;
    }
    return color;
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * Does not differentiate between left, middle and right mouse click.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event
 *
 * @internal
 *
 * @todo This widget reacts on mouse press events also when they occur
 * within the border. It might be nice if it would not. On the other
 * hand: The border is small. Would it really be worth the pain to
 * implement this? */
void ChromaLightnessDiagram::mousePressEvent(QMouseEvent *event)
{
    d_pointer->m_isMouseEventActive = true;
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    if (d_pointer->isWidgetPixelPositionInGamut(event->pos())) {
        setCursor(Qt::BlankCursor);
    } else {
        unsetCursor();
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event */
void ChromaLightnessDiagram::mouseMoveEvent(QMouseEvent *event)
{
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    if (d_pointer->isWidgetPixelPositionInGamut(event->pos())) {
        setCursor(Qt::BlankCursor);
    } else {
        unsetCursor();
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Does not differentiate between left,
 * middle and right mouse click.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event */
void ChromaLightnessDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    unsetCursor();
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * @param event the paint event */
void ChromaLightnessDiagram::paintEvent(QPaintEvent *event)
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
    QImage paintBuffer(physicalPixelSize(), QImage::Format_ARGB32_Premultiplied);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);
    QPen pen;

    // Paint the diagram itself as available in the cache.
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(
        // Operating in physical pixels:
        d_pointer->leftBorderPhysical(),             // x position (top-left)
        d_pointer->defaultBorderPhysical(),          // y position (top-left)
        d_pointer->m_chromaLightnessImage.getImage() // image
    );

    // Paint a focus indicator.
    //
    // We could paint a focus indicator (round or rectangular) around the
    // handle. Depending on the currently selected hue for the diagram,
    // it looks ugly because the colors of focus indicator and diagram
    // do not harmonize, or it is mostly invisible the the colors are
    // similar. So this approach does not work well.
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
    // However, this does not work well because the chroma-lightness
    // diagram has usually a triangular shape. The style primitive, however,
    // often paints just a line at the bottom of the widget. That does not
    // look good. An alternative approach is that we paint ourselves a focus
    // indicator only on the left of the diagram (which is the place of
    // black/gray/white, so the won't be any problems with non-harmonic
    // colors).
    //
    // Then we have to design the line that we want to display. It is better
    // to do that ourselves instead of relying on generic QStyle::PE_Frame or
    // similar solutions as their result seems to be quite unpredictable across
    // various styles. So we use handleOutlineThickness as line width and
    // paint it at the left-most possible position.
    if (hasFocus()) {
        pen = QPen();
        pen.setWidthF(handleOutlineThickness() * devicePixelRatioF());
        pen.setColor(focusIndicatorColor());
        pen.setCapStyle(Qt::PenCapStyle::FlatCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, true);
        const QPointF pointOne(
            // x:
            handleOutlineThickness() * devicePixelRatioF() / 2.0,
            // y:
            0 + d_pointer->defaultBorderPhysical());
        const QPointF pointTwo(
            // x:
            handleOutlineThickness() * devicePixelRatioF() / 2.0,
            // y:
            physicalPixelSize().height() - d_pointer->defaultBorderPhysical());
        painter.drawLine(pointOne, pointTwo);
    }

    // Paint the handle on-the-fly.
    const int diagramHeight = d_pointer->calculateImageSizePhysical().height();
    QPointF colorCoordinatePoint = QPointF(
        // x:
        d_pointer->m_currentColor.c * diagramHeight / 100.0,
        // y:
        d_pointer->m_currentColor.l * diagramHeight / 100.0 * (-1) + diagramHeight);
    colorCoordinatePoint += QPointF(d_pointer->leftBorderPhysical(),   // horizontal offset
                                    d_pointer->defaultBorderPhysical() // vertical offset
    );
    pen = QPen();
    pen.setWidthF(handleOutlineThickness() * devicePixelRatioF());
    pen.setColor(handleColorFromBackgroundLightness(d_pointer->m_currentColor.l));
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(colorCoordinatePoint,                 // center
                        handleRadius() * devicePixelRatioF(), // x radius
                        handleRadius() * devicePixelRatioF()  // y radius
    );

    // Paint the buffer to the actual widget
    paintBuffer.setDevicePixelRatio(devicePixelRatioF());
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing, true);
    widgetPainter.drawImage(0, 0, paintBuffer);
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 *  When the arrow keys are pressed, it moves the
 * handle a small step into the desired direction.
 * When <tt>Qt::Key_PageUp</tt>, <tt>Qt::Key_PageDown</tt>,
 * <tt>Qt::Key_Home</tt> or <tt>Qt::Key_End</tt> are pressed, it moves the
 * handle a big step into the desired direction.
 *
 * Other key events are forwarded to the base class.
 *
 * @param event the event
 *
 * @internal
 *
 * @todo Is the current behaviour (when pressing right arrow while yet
 * at the right border of the gamut, also the lightness is adjusted to
 * allow moving actually to the right) really a good idea? Andway, it
 * has a bug, and arrow-down does not work on blue hues because the
 * gamut has some sort of corner, and there, the curser blocks. */
void ChromaLightnessDiagram::keyPressEvent(QKeyEvent *event)
{
    LchDouble temp = d_pointer->m_currentColor;
    switch (event->key()) {
    case Qt::Key_Up:
        temp.l += singleStepLightness;
        break;
    case Qt::Key_Down:
        temp.l -= singleStepLightness;
        break;
    case Qt::Key_Left:
        temp.c -= singleStepChroma;
        break;
    case Qt::Key_Right:
        temp.c += singleStepChroma;
        break;
    case Qt::Key_PageUp:
        temp.l += pageStepLightness;
        break;
    case Qt::Key_PageDown:
        temp.l -= pageStepLightness;
        break;
    case Qt::Key_Home:
        temp.c += pageStepChroma;
        break;
    case Qt::Key_End:
        temp.c -= pageStepChroma;
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

    // Set the new image coordinates (only takes effect when image
    // coordinates are indeed different)
    setCurrentColor(temp);
}

/** @brief Tests if a given widget pixel position is within
 * the <em>displayed</em> gamut.
 *
 * @param widgetPixelPosition The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to be within the
 * actual displayed diagram or even the gamut itself. It might even be
 * negative.
 *
 * @returns <tt>true</tt> if the widget pixel position is within the
 * <em>currently displayed gamut</em>. Otherwise <tt>false</tt>.
 *
 * @internal
 *
 * @todo How does isInGamut() react? Does it also control valid chroma
 * and lightness ranges? */
bool ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::isWidgetPixelPositionInGamut(const QPoint widgetPixelPosition) const
{
    if (calculateImageSizePhysical().isEmpty()) {
        // If there is no displayed gamut, the anser must be false.
        // But fromWidgetPixelPositionToColor() would return an in-gamut
        // fallback color nevertheless. Therefore, we have to catch
        // the special case with an empty diagram here manually.
        return false;
    }

    const LchDouble color = fromWidgetPixelPositionToColor(widgetPixelPosition);
    return (
        // Test if C is in range. This is important because a negative C value
        // can be in-gamut, but is not in the _displayed_ gamut.
        inRange<qreal>(0, color.c, m_rgbColorSpace->maximumChroma())
        // Test for out-of-range lightness (mainly for performance reasons)
        && inRange<qreal>(0, color.l, 100)
        // Test actually for in-gamut color
        && m_rgbColorSpace->isInGamut(color));
}

/** @brief Setter for the @ref currentColor() property.
 *
 * @param newCurrentColor the new @ref currentColor
 *
 * @todo When an out-of-gamut color is given, both lightness and chroma
 * are adjusted. But does this really make sense? In @ref WheelColorPicker,
 * when using the hue wheel, also <em>both</em>, lightness <em>and</em> chroma
 * will change. Isn’t that confusing? */
void ChromaLightnessDiagram::setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor)
{
    const LchDouble newColorInGamut =
        // Move newCurrentColor into the gamut (if necessary) – while preserving the hue:
        d_pointer->m_rgbColorSpace->nearestInGamutColorByAdjustingChromaLightness(newCurrentColor);
    if (newColorInGamut.hasSameCoordinates(d_pointer->m_currentColor)) {
        return;
    }

    double oldHue = d_pointer->m_currentColor.h;
    d_pointer->m_currentColor = newColorInGamut;
    if (d_pointer->m_currentColor.h != oldHue) {
        // Update the diagram (only if the hue has changed):
        d_pointer->m_chromaLightnessImage.setHue(d_pointer->m_currentColor.h);
    }
    update(); // Schedule a paint event
    Q_EMIT currentColorChanged(newColorInGamut);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding event */
void ChromaLightnessDiagram::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_pointer->m_chromaLightnessImage.setImageSize(
        // Update the image size will free memory of the cache inmediatly.
        d_pointer->calculateImageSizePhysical());
    // As by Qt documentation:
    //     “The widget will be erased and receive a paint event
    //      immediately after processing the resize event. No drawing
    //      need be (or should be) done inside this handler.”
}

/** @brief Recommmended size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize ChromaLightnessDiagram::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

/** @brief Recommended minimum size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref sizeHint() */
QSize ChromaLightnessDiagram::minimumSizeHint() const
{
    const int minimumHeight = qRound(
        // Top border and bottom border:
        2.0 * d_pointer->defaultBorderPhysical() / devicePixelRatioF()
        // Add the height for the diagram:
        + gradientMinimumLength());
    const int minimumWidth = qRound(
        // Left border and right border:
        (d_pointer->leftBorderPhysical() + d_pointer->defaultBorderPhysical()) / devicePixelRatioF()
        // Add the gradient minimum length from y axis, multiplied with
        // the factor to allow at correct scaling showing up the whole
        // chroma range of the gamut.
        + gradientMinimumLength() * d_pointer->m_rgbColorSpace->maximumChroma() / 100.0);
    // Expand to the global minimum size for GUI elements
    return QSize(minimumWidth, minimumHeight).expandedTo(QApplication::globalStrut());
}

// No documentation here (documentation of properties
// and its getters are in the header)
LchDouble PerceptualColor::ChromaLightnessDiagram::currentColor() const
{
    return d_pointer->m_currentColor;
}

} // namespace PerceptualColor
