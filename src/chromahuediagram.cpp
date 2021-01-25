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
#include "PerceptualColor/chromahuediagram.h"
// Second, the private implementation.
#include "chromahuediagram_p.h"

#include "helper.h"
#include "lchvalues.h"
#include "PerceptualColor/polarpointf.h"
#include <PerceptualColor/simplecolorwheel.h>

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QWidget>

#include <lcms2.h>

namespace PerceptualColor {

/** @brief The constructor.
 * @param colorSpace The color spaces within this widget should operate.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the QWidget base class constructor. */
ChromaHueDiagram::ChromaHueDiagram(
    const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
    QWidget *parent
) : AbstractDiagram(parent),
    d_pointer(new ChromaHueDiagramPrivate(this, colorSpace))
{
    // Setup LittleCMS. This is the first thing to do, because other
    // operations rely on a working LittleCMS.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Initialize the color
    cmsCIELCh initialColorLch;
    initialColorLch.h = LchValues::defaultHue;
    initialColorLch.C = LchValues::srgbVersatileChroma;
    initialColorLch.L = LchValues::defaultLightness;
    d_pointer->m_color = FullColorDescription(
        d_pointer->m_rgbColorSpace,
        initialColorLch,
        FullColorDescription::outOfGamutBehaviour::sacrifyChroma
    );

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
ChromaHueDiagram::~ChromaHueDiagram() noexcept
{
}

/** @brief Constructor
 * 
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation.
 * @param colorSpace The color spaces within this widget should operate. */
ChromaHueDiagram::ChromaHueDiagramPrivate::ChromaHueDiagramPrivate(
    ChromaHueDiagram *backLink,
    const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
) :
    m_chromaHueImage(colorSpace),
    m_wheelImage(colorSpace),
    q_pointer(backLink)
{
}
    
/** @brief Sets the @ref color property corresponding to given widget
 * coordinates.
 * 
 * @param imageCoordinates A coordinate pair within the widget’s coordinate
 * system. This does not necessarily need to be within the actual displayed
 * diagram or even the gamut itself. It might even be negative.
 * 
 * @post If the widget coordinates are within the represented gamut, then
 * the @ref color property is set correspondingly. If the coordinates are
 * outside the gamut, then the chroma value is reduced (while the hue is
 * maintained) until arriving at the outer shell of the gamut; this adapted
 * color is than used for the @ref color property.
 * 
 * @note This function works independently of the actually display color
 * gamut diagram. So if parts of the gamut are cut off in the diagram,
 * this does not influence this function. */
void ChromaHueDiagram::ChromaHueDiagramPrivate::setColorFromImageCoordinates(
    const QPoint imageCoordinates
)
{
    QPointF aB;
    cmsCIELab lab;
    if (imageCoordinates != imageCoordinatesFromColor()) {
        aB = fromImageCoordinatesToAB(imageCoordinates);
        lab.L = m_color.toLch().L;
        lab.a = aB.x();
        lab.b = aB.y();
        q_pointer->setColor(
            FullColorDescription(
                m_rgbColorSpace,
                lab,
                FullColorDescription::outOfGamutBehaviour::sacrifyChroma
            )
        );
    }
}

/** @brief React on a mouse press event.
 * 
 * Reimplemented from base class.
 *
 * @post
 * - If the mouse is clicked with the circular diagram (inside or
 *   outside of the visible gamut), than this widget gets the focus
 *   and and @ref ChromaHueDiagramPrivate::m_isMouseEventActive is
 *   set to <tt>true</tt> to track mouse movements from now on.
 *   Reacts on all clicks (left, middle, right). If the mouse was
 *   within the gamut, the diagram’s marker is displaced there. If
 *   the mouse was outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is scarified while the hue value is
 *   retained.
 * - Any other type of mouse event is handled with the default QWidget
 *   implementation of this function.
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mousePressEvent(QMouseEvent *event)
{
    // TODO Also accept out-of-gamut clicks when they are covered by the
    // current marker.
    if (d_pointer->areImageCoordinatesWithinDiagramSurface(event->pos())) {
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
        // cursor is made invisible. Its function is taken over by the value
        // marker within the displayed gamut.
        setCursor(Qt::BlankCursor);
        // Set the color property
        d_pointer->setColorFromImageCoordinates(event->pos());
        // Schedule a paint event, so that the wheel marker will show. It’s
        // not enough to hope setColorFromWidgetCoordinates() would do this,
        // because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color marker.
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
 * Reacts only on mouse move events if
 * @ref ChromaHueDiagramPrivate::m_isMouseEventActive is <tt>true</tt>.
 * - If the mouse moves within the gamut, the diagram’s marker is displaced
 *   there. The mouse cursor is invisible; only the diagram’ marker is
 *   visible.
 * - If the mouse moves outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is scarified while the hue value is
 *   retained. Both, the diagram’s marker <em>and</em> the mouse cursor are
 *   visible.
 * 
 * If @ref ChromaHueDiagramPrivate::m_isMouseEventActive is <tt>false</tt>,
 * it simply falls back to QWidget’s default implementation.
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseMoveEvent(QMouseEvent *event)
{
    if (d_pointer->m_isMouseEventActive) {
        event->accept();
        cmsCIELab lab;
        QPointF aB = d_pointer->fromImageCoordinatesToAB(event->pos());
        lab.L = d_pointer->m_color.toLch().L;
        lab.a = aB.x();
        lab.b = aB.y();
        if (
            d_pointer->areImageCoordinatesWithinDiagramSurface(event->pos()) &&
            d_pointer->m_rgbColorSpace->inGamut(lab)
            
        ) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        d_pointer->setColorFromImageCoordinates(event->pos());
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
 * If @ref ChromaHueDiagramPrivate::m_isMouseEventActive is <tt>true</tt>
 * then:
 * - If the mouse is within the gamut, the diagram’s marker is displaced
 *   there.
 * - If the mouse moves outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is scarified while the hue value is
 *   retained.
 * - The mouse cursor is made visible (if he wasn’t yet visible anyway).
 * - @ref ChromaHueDiagramPrivate::m_isMouseEventActive is set
 *   to <tt>false</tt>.
 * 
 * If @ref ChromaHueDiagramPrivate::m_isMouseEventActive is <tt>false</tt>,
 * it simply falls back to QWidget’s default implementation.
 * 
 * @todo What if the widget displays a gamut that has no L*=0.1 because its
 * blackpoint is lighter.? Scarifying chroma alone does not help? How to
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
        d_pointer->setColorFromImageCoordinates(event->pos());
        // Schedule a paint event, so that the wheel marker will be hidden.
        // It’s not enough to hope setColorFromWidgetCoordinates() would do
        // this, because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color marker.
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
void ChromaHueDiagram::wheelEvent(QWheelEvent* event)
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
        d_pointer->areImageCoordinatesWithinDiagramSurface(event->pos())
    ) {
        event->accept();
        cmsCIELCh lch = d_pointer->m_color.toLch();
        // Calculate the new hue.
        // This may result in a hue smaller then 0° or bigger then 360°.
        // This is no problem because the constructor of FullColorDescription
        // will normalize the hue.
        lch.h += standardWheelSteps(event) * singleStepHue;
        setColor(
            FullColorDescription(
                d_pointer->m_rgbColorSpace,
                lch,
                FullColorDescription::outOfGamutBehaviour::sacrifyChroma
            )
        );
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
 * For chroma changes: Moves the marker as much as possible into the
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
 * @param event the paint event
 * 
 * @todo Is this behavior really a good user experience? Or is it confusing
 * that left, right, up and down don’t do what was expected? What could be
 * more intuitive keys for changing radial and angle? At least the arrow keys
 * are likely that the user tries them out by trial-and-error. */
void ChromaHueDiagram::keyPressEvent(QKeyEvent *event)
{
    cmsCIELCh lch = d_pointer->m_color.toLch();
    switch (event->key()) {
        case Qt::Key_Up:
            lch.C += singleStepChroma;
            break;
        case Qt::Key_Down:
            lch.C -= singleStepChroma;
            break;
        case Qt::Key_Left:
            lch.h += singleStepHue;
            break;
        case Qt::Key_Right:
            lch.h -= singleStepHue;
            break;
        case Qt::Key_PageUp:
            lch.C += pageStepChroma;
            break;
        case Qt::Key_PageDown:
            lch.C -= pageStepChroma;
            break;
        case Qt::Key_Home:
            lch.h += pageStepHue;
            break;
        case Qt::Key_End:
            lch.h -= pageStepHue;
            break;
        default:
            /* Quote from Qt documentation:
             * 
             *     “If you reimplement this handler, it is very important that
             *      you call the base class implementation if you do not act
             *      upon the key.
             * 
             *      The default implementation closes popup widgets if the
             *      user presses the key sequence for QKeySequence::Cancel
             *      (typically the Escape key). Otherwise the event is
             *      ignored, so that the widget's parent can interpret it.“ */
            QWidget::keyPressEvent(event);
            return;
    }
    // Here we reach only if the key has been recognized. If not, in the
    // default branch of the switch statement, we would have passed the
    // keyPressEvent yet to the parent and returned.
    if (lch.C < 0) {
        // Do not allow negative chroma values.
        // (Doing so would be counter-intuitive.)
        lch.C = 0;
    }
    setColor(
        FullColorDescription(
            d_pointer->m_rgbColorSpace,
            lch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
    );
}

/** @brief Provide the size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the size hint
 * 
 * @sa @ref minimumSizeHint() */
QSize ChromaHueDiagram::sizeHint() const
{
    return QSize(300, 300);
}

/** @brief Provide the minimum size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the minimum size hint
 * 
 * @sa @ref sizeHint() */
QSize ChromaHueDiagram::minimumSizeHint() const
{
    return QSize(100, 100);
}

// No documentation here (documentation of properties
// and its getters are in the header)
FullColorDescription ChromaHueDiagram::color() const
{
    return d_pointer->m_color;
}

/** @brief Setter for the @ref color() property.
 * 
 * @param color the new color
 * @post If <em>color</em> is valid, it is set. Otherwise, it is ignored.
 * @todo ColorDialog (like QColorDialog) sets the color to black when an
 * invalid value is set. On the one hand, it might be better if this
 * function does the same and so get consistent behavior all over the
 * library; on the other hand substituting by black feels strange and
 * wrong… */
void ChromaHueDiagram::setColor(const FullColorDescription &color)
{
    if (!color.isValid()) {
        return;
    }

    if (color == d_pointer->m_color) {
        return;
    }

    FullColorDescription oldColor = d_pointer->m_color;

    d_pointer->m_color = color;

    // Update, if necessary, the diagram.
    if (d_pointer->m_color.toLch().L != oldColor.toLch().L) {
        d_pointer->m_chromaHueImage.setLightness(
            d_pointer->m_color.toLch().L
        );
    }

    // Schedule a paint event:
    update();

    // Emit notify signal
    Q_EMIT colorChanged(color);
}

// TODO xxx Revision starting here

// xxx // TODO document and test the internationalization features of
       // MultiSpinBox.
// xxx // TODO own class for image caches also for chroma-hue-image

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 * 
 * @param event The corresponding resize event */
void ChromaHueDiagram::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    // As Qt’s documentation says: 
    //
    //     “When resizeEvent() is called, the widget already has its new
    //      geometry.”
    //
    // So we can rely on QWidget.size() to get the new size.

    // Find the smaller one of width and height
    int newWidgetDiameter = qMin(
        size().width(),
        size().height()
    );
    if (newWidgetDiameter < 0) {
        newWidgetDiameter = 0;
    }

    // Update the widget
    if (newWidgetDiameter != d_pointer->m_widgetDiameter) {
        d_pointer->m_widgetDiameter = newWidgetDiameter;
        d_pointer->m_diagramOffset =
            (d_pointer->m_widgetDiameter - 1) / static_cast<qreal>(2);
        d_pointer->m_chromaHueImage.setImageSize(
            // Rounding down by using static_cast<int>
            static_cast<int>(d_pointer->m_widgetDiameter * devicePixelRatioF())
        );
        d_pointer->m_wheelImage.setImageSize(
            // Consciously rounding down by using static_cast<int>
            static_cast<int>(d_pointer->m_widgetDiameter * devicePixelRatioF())
        );
        // As Qt documentation says:
        //     “The widget will be erased and receive a paint event
        //      immediately after processing the resize event. No
        //      drawing need be (or should be) done inside this handler.”
    }
}

// TODO xxx What happens when maxChroma is low and parts of the gamut
// are out-of-display?

/**
 * @param imageCoordinates the image coordinates
 * @returns the diagram (a-b) value for given image coordinates
 */
QPointF ChromaHueDiagram::ChromaHueDiagramPrivate::fromImageCoordinatesToAB(
    const QPoint imageCoordinates
)
{
    const qreal scaleFactor =
        static_cast<qreal>(2 * m_maxChroma)
            / (m_widgetDiameter - 2 * diagramBorder);
    return QPointF(
        (imageCoordinates.x() - m_diagramOffset) * scaleFactor,
        (imageCoordinates.y() - m_diagramOffset) * scaleFactor * (-1)
    );
}

/**
 * @returns the logical coordinates that correspond to @ref m_color()
 */
QPoint ChromaHueDiagram::ChromaHueDiagramPrivate::imageCoordinatesFromColor()
{
    const qreal scaleFactor =
        (m_widgetDiameter - 2 * diagramBorder)
            / static_cast<qreal>(2 * m_maxChroma);
    return QPoint(
        qRound((m_color.toLab().a * scaleFactor + m_diagramOffset)),
        qRound(
            (m_color.toLab().b * scaleFactor + m_diagramOffset) * (-1)
                + 2 * m_diagramOffset
        )
    );
}

/** @brief Tests if image coordinates are within the diagram surface.
 * 
 * The diagram surface is the gray circle on which the gamut diagram is
 * painted.
 * @param imageCoordinates the image coordinates to test
 * @returns <tt>true</tt> if the given image coordinates are within this
 * circle, <tt>false</tt> otherwise. */
bool ChromaHueDiagram
    ::ChromaHueDiagramPrivate
    ::areImageCoordinatesWithinDiagramSurface
(
    const QPoint imageCoordinates
)
{
    const qreal radialPixel = PerceptualColor::PolarPointF(
        imageCoordinates - QPointF(m_diagramOffset, m_diagramOffset)
    ).radial();
    return (
        radialPixel <= ( (m_widgetDiameter - 1) / static_cast<qreal>(2) )
    );
}

// TODO how to treat empty images because the color profile does not
// work or the resolution is too small?

// TODO what to do if a gamut allows lightness < 0 or lightness > 100 ???

/** @brief Paint the widget.
 * 
 * Reimplemented from base class.
 *
 * - Paints the widget. Takes the existing
 *   @ref ChromaHueDiagramPrivate::m_chromaHueImage and
 *   @ref ChromaHueDiagramPrivate::m_wheelImage and paints them on the widget.
 * - Paints the markers.
 * - If the widget has focus, it also paints the focus indicator.
 * 
 * @param event the paint event
 * 
 * @todo What when @ref ChromaHueDiagramPrivate::m_color has a valid
 * in-gamut color, but this color is out of the <em>displayed</em> diagram?
 * How to handle that?
 * 
 * @todo Better design on small widget sizes for the whole library. */
void ChromaHueDiagram::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    int imageSize = qMin(
            physicalPixelSize().width(),
            physicalPixelSize().height()
    );
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
    QImage paintBuffer(
        imageSize,
        imageSize,
        QImage::Format_ARGB32_Premultiplied
    );
    paintBuffer.fill(Qt::transparent);
    paintBuffer.setDevicePixelRatio(devicePixelRatioF());
    
    // Other initialization
    QPainter painter(&paintBuffer);
    QPen pen;
    QBrush brush;

    // Paint the gamut itself as available in the cache.
    painter.setRenderHint(QPainter::Antialiasing, false);
    d_pointer->m_chromaHueImage.setBorder(
        d_pointer->diagramBorder * devicePixelRatioF()
    );
    d_pointer->m_chromaHueImage.setImageSize(
        // Rounding down by using static_cast<int>
        static_cast<int>(d_pointer->m_widgetDiameter * devicePixelRatioF())
    );
    d_pointer->m_chromaHueImage.setChromaRange(
        d_pointer->m_maxChroma
    );
    d_pointer->m_chromaHueImage.setLightness(
        d_pointer->m_color.toLch().L
    );
    d_pointer->m_chromaHueImage.setDevicePixelRatioF(
        devicePixelRatioF()
    );
    painter.drawImage(
        QPoint(0, 0),                          // position of the image
        d_pointer->m_chromaHueImage.getImage() // image
    );

    // Paint a color wheel around
    painter.setRenderHint(QPainter::Antialiasing, false);
    // As devicePixelRatioF() might have changed, we make sure everything
    // is update before painting.
    d_pointer->m_wheelImage.setBorder(
        2 * markerThickness * devicePixelRatioF()
    );
    d_pointer->m_wheelImage.setDevicePixelRatioF(devicePixelRatioF());
    d_pointer->m_wheelImage.setImageSize(
        // Consciously rounding down by using static_cast<int>
        static_cast<int>(d_pointer->m_widgetDiameter * devicePixelRatioF())
    );
    d_pointer->m_wheelImage.setWheelThickness(4 * markerThickness);
    painter.drawImage(
        QPoint(0, 0),                       // position of the image
        d_pointer->m_wheelImage.getImage()  // the image itself
    );

    // Paint a marker on the color wheel
    // only if a mouse event is currently active.
    if (d_pointer->m_isMouseEventActive) {
        // TODO Apparently the position is not exact.
        qreal radius =
            d_pointer->m_widgetDiameter
                / static_cast<qreal>(2) - 2 * markerThickness;
        // Get widget coordinates for our marker
        QPointF myMarkerInner = PolarPointF(
            radius - 4 * markerThickness,
            d_pointer->m_color.toLch().h
        ).toCartesian();
        QPointF myMarkerOuter = PolarPointF(
            radius,
            d_pointer->m_color.toLch().h
        ).toCartesian();
        myMarkerInner.ry() *= -1;
        myMarkerOuter.ry() *= -1;
        myMarkerInner += QPointF(
            d_pointer->m_diagramOffset,
            d_pointer->m_diagramOffset
        );
        myMarkerOuter += QPointF(
            d_pointer->m_diagramOffset,
            d_pointer->m_diagramOffset
        );
        // Draw the line
        pen = QPen();
        pen.setWidth(markerThickness);
        pen.setCapStyle(Qt::FlatCap);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(myMarkerInner, myMarkerOuter);
    }
  
    /* Paint a focus indicator.
     * 
     * We could paint a focus indicator (round or rectangular) around the
     * marker. Depending on the currently selected hue for the diagram, it
     * looks ugly because the colors of focus indicator and diagram do not
     * harmonize, or it is mostly invisible the the colors are similar. So
     * this approach does not work well.
     * 
     * It seems better to paint a focus indicator for the whole widget.
     * We could use the style primitives to paint a rectangular focus
     * indicator around the whole widget:
     * 
     * style()->drawPrimitive(
     *     QStyle::PE_FrameFocusRect,
     *     &option,
     *     &painter,
     *     this
     * );
     * 
     * However, this does not work well because this widget does not have a
     * rectangular form.
     * 
     * Then we have to design the line that we want to display. It is better
     * to do that ourselves instead of relying on generic QStyle::PE_Frame or
     * similar solutions as their result seems to be quite unpredictable
     * across various styles. So we use markerThickness as line width and
     * paint it at the left-most possible position. As the diagramBorder
     * accommodates also to markerRadius, the distance of the focus line to
     * the real diagram also does, which looks nice. */
    if (hasFocus()) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        pen = QPen();
        pen.setWidth(markerThickness);
        pen.setColor(focusIndicatorColor());
        painter.setPen(pen);
        brush = QBrush(Qt::transparent);
        painter.setBrush(brush);
        painter.drawEllipse(
            QPointF(
                d_pointer->m_diagramOffset + 1,
                d_pointer->m_diagramOffset + 1
            ),                                                      // center
            (d_pointer->m_widgetDiameter - markerThickness) / 2,    // x radius
            (d_pointer->m_widgetDiameter - markerThickness) / 2     // y radius
        );
    }

    // Paint the marker on-the-fly.
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPoint imageCoordinates = d_pointer->imageCoordinatesFromColor();
    pen = QPen();
    pen.setWidth(markerThickness);
    // Set color of the marker: Black or white, depending on the lightness of
    // the currently selected color.
    if (d_pointer->m_color.toLch().L >= 50) { // range: 0..100
        pen.setColor(Qt::black);
    } else {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);
    brush = QBrush(Qt::transparent);
    painter.setBrush(brush);
    painter.drawEllipse( // TODO Switch to floating point!
        imageCoordinates.x() - markerRadius,
        imageCoordinates.y() - markerRadius,
        2 * markerRadius + 1,
        2 * markerRadius + 1
    );
    PolarPointF lineEndPolar = PolarPointF(
        QPointF(
            imageCoordinates.x() - d_pointer->m_diagramOffset,
            (imageCoordinates.y() - d_pointer->m_diagramOffset) * (-1)
        )
    );
    qreal newRadial = lineEndPolar.radial() - markerRadius - 1.5;
    QPointF lineEndCartesian;
    if (newRadial > 0) {
        lineEndCartesian = PolarPointF(
            newRadial,
            lineEndPolar.angleDegree()
        ).toCartesian();
        lineEndCartesian.setY(lineEndCartesian.y() * (-1));
        lineEndCartesian += QPointF(
            d_pointer->m_diagramOffset,
            d_pointer->m_diagramOffset
        );
        painter.drawLine(
            QPointF(
                d_pointer->m_diagramOffset,
                d_pointer->m_diagramOffset
            ),
            lineEndCartesian
        );
    }

    // Paint the buffer to the actual widget
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(
        QPainter::Antialiasing,
        false
    );
    widgetPainter.drawImage(
        QPoint(0, 0),
        paintBuffer
    );
}

}
