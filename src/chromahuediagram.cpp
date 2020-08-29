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
#include "PerceptualColor/chromahuediagram.h"

#include "PerceptualColor/helper.h"
#include "PerceptualColor/polarpointf.h"
#include <PerceptualColor/simplecolorwheel.h>

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QWidget>

#include <lcms2.h>

// TODO Remove these headers that are only for development purposes:
#include <QElapsedTimer>

namespace PerceptualColor {

/** @brief The constructor.
 * @param colorSpace The color spaces within this widget should operate.
 * @param parent The widget’s parent widget. This paramenter will be passed
 * to the QWidget base class constructor. */
ChromaHueDiagram::ChromaHueDiagram(
    RgbColorSpace *colorSpace,
    QWidget *parent
) : CircularDiagram(parent)
{
    // Setup LittleCMS. This is the first thing to do, because other
    // operations rely on a working LittleCMS.
    m_rgbColorSpace = colorSpace;

    // Initialize the color
    cmsCIELCh initialColorLch;
    initialColorLch.h = Helper::LchDefaults::defaultHue;
    initialColorLch.C = Helper::LchDefaults::versatileSrgbChroma;
    initialColorLch.L = Helper::LchDefaults::defaultLightness;
    m_color = FullColorDescription(
        m_rgbColorSpace,
        initialColorLch,
        FullColorDescription::outOfGamutBehaviour::sacrifyChroma
    );

    // Accept focus only by keyboard tabbing and not by mouse click
    // This is because the focus by mouse click is handeled manually
    // by mousePressEvent().
    setFocusPolicy(Qt::FocusPolicy::TabFocus);

    // Define the size policy of this widget.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/** @brief Sets the @ref color property corresponding to given widget
 * coordinates.
 * 
 * @param imageCoordinates A coordinte pair within the widget’s coordinate
 * system. This does not necessarily need to be within the actual displayed
 * diagram or even the gamut itself. It might even be negative.
 * 
 * \post If the widget coordinates are within the represented gamut, then the
 * @ref color property is set correcpondingly. If the coordinates are outside
 * the gamut, then the chroma value is reduced (while the hue is maintained)
 * until arriving at the outer shell of the gamut; this adapted color is than
 * used for the @ref color property.
 * 
 * @note This function works independently of the actually display color
 * gamut diagram. So if parts of the gamut are cut off in the diagram,
 * this does not influence this function. */
void ChromaHueDiagram::setColorFromImageCoordinates(
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
        setColor(
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
 * - If the mouse is clicked <em>within</em> the visible gamut, than the
 *   marker is placed there and @ref m_isMouseEventActive is set to
 *   <tt>true</tt> to track mouse movements from now on. Reacts on all clicks
 *   (left, middle, right).
 * - Any other type of mouse event is handled with the default QWidget
 *   implementation of this function.
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mousePressEvent(QMouseEvent *event)
{
    // TODO Also accept out-of-gamut clicks when they are covered by the
    // current marker.
    if (areImageCoordinatesWithinDiagramSurface(event->pos())) {
        // Mouse focus is handeled manually because so we can accept
        // focus only on mouse clicks within the displayed gamut, while
        // rejecting focus otherwise. In the constructor, therefore
        // Qt::FocusPolicy::TabFocus is specified, so that manual handling
        // of mouse focus is up to this code here.
        // TODO Find another solution that guarantees that setFocusPolicy()
        // API of this class behaves as expected, and not as a dirty hack that
        // accepts mouse focus even when set to Qt::FocusPolicy::TabFocus.
        setFocus(Qt::MouseFocusReason);
        // Enable mouse tracking from now on:
        m_isMouseEventActive = true;
        // As clicks are only accepted within the visible gamut, the mouse
        // cursor is made invisible. Its function is taken over by the value
        // marker within the displayed gamut.
        setCursor(Qt::BlankCursor);
        // Set the color property
        setColorFromImageCoordinates(event->pos());
        // Schedule a paint event, so that the wheel marker will show. It’s
        // not enough to hope setColorFromWidgetCoordinates() would do this,
        // because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color marker.
        update();
    } else {
        // Make sure default behaviour like drag-window in KDE’s
        // “Breeze” widget style works if this widget does not
        // actually react itself on a mouse event.
        QWidget::mousePressEvent(event);
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * Reacts only on mouse move events @ref m_isMouseEventActive is <tt>true</tt>.
 * - If the mouse moves within the gamut, the diagram’s marker is displaced
 *   there. The mouse cursor is invisible; only the diagram’ marker is visible.
 * - If the mouse moves outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is sacrified while the hue value is
 *   retained. Both, the diagram’s marker <em>and</em> the mouse cursor are
 *   visible.
 * 
 * If @ref m_isMouseEventActive is <tt>false</tt>, it simply falls back to
 * QWidget’s default implementation.
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isMouseEventActive) {
        cmsCIELab lab;
        QPointF aB = fromImageCoordinatesToAB(event->pos());
        lab.L = m_color.toLch().L;
        lab.a = aB.x();
        lab.b = aB.y();
        if (
            areImageCoordinatesWithinDiagramSurface(event->pos()) &&
            m_rgbColorSpace->inGamut(lab)
            
        ) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        setColorFromImageCoordinates(event->pos());
    } else {
        // Make sure default behaviour like drag-window in KDE’s
        // Breeze widget style works.
        QWidget::mousePressEvent(event);
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Reacts on all clicks (left, middle, right).
 *
 * If @ref m_isMouseEventActive is <tt>true</tt> then:
 * - If the mouse is within the gamut, the diagram’s marker is displaced
 *   there.
 * - If the mouse moves outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is sacrified while the hue value is
 *   retained.
 * - The mouse curser is made visible (if he wasn’t yet visible anyway).
 * - @ref m_isMouseEventActive is set to <tt>false</tt>.
 * 
 * If @ref m_isMouseEventActive is <tt>false</tt>, it simply falls back to
 * QWidget’s default implementation.
 * 
 * @todo What if the widget displays a gamut that has no L*=0.1 because its
 * blackpoint is lighter.? Sacrifying chroma alone does not help? How to
 * react (for mouse input, keyboard input, but also API functions like
 * setColor()?
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isMouseEventActive) {
        unsetCursor();
        m_isMouseEventActive = false;
        setColorFromImageCoordinates(event->pos());
        // Schedule a paint event, so that the wheel marker will be hidden.
        // It’s not enough to hope setColorFromWidgetCoordinates() would do
        // this, because setColorFromWidgetCoordinates() would not update the
        // widget if the mouse click was done at the same position as the
        // current color marker.
        update();
    } else {
        // Make sure default behaviour like drag-window in KDE's
        // Breeze widget style works
        QWidget::mousePressEvent(event);
    }
}

// xxx

/** @brief Paint the widget.
 * 
 * Reimplemented from base class.
 * 
 * Paints the widget. Takes the existing m_diagramImage and m_diagramPixmap
 * and paints them on the widget. Paints, if approperiate, the focus
 * indicator. Paints the marker. Relies on that m_diagramImage and
 * m_diagramPixmap are up to date.
 * 
 * @param event the paint event
 * 
 * @todo What when m_color has a valid in-gamut color, but this color is out
 * of the <em>displayed</em> diagram? How to handle that?
 * 
 * @todo Make the wheel to be drawn horizontally and vertically aligned.
 * 
 * @todo Better design on small widget sizes
 */
void ChromaHueDiagram::paintEvent(QPaintEvent* event)
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
    // (at difference to QPixmap and a QWidget) is independant of native
    // platform rendering, it guarantees identical anti-aliasing results on
    // all platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should
    //       use the platform independent QImage as paint device; i.e. using
    //       QImage will ensure that the result has an identical pixel
    //       representation on any platform.”
    QImage paintBuffer(m_diameter, m_diameter, QImage::Format_ARGB32);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);

    QPen pen;

    // Paint the background.
    cmsCIELCh backgroundColor;
    backgroundColor.L = 50;
    backgroundColor.C = 0;
    backgroundColor.h = 0;
    QBrush brush(m_rgbColorSpace->colorRgbBound(backgroundColor));
    painter.setBrush(brush);
    pen.setStyle(Qt::PenStyle::NoPen);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(
        border,                   // x
        border,                   // y
        m_diameter - 2 * border,  // width
        m_diameter - 2 * border   // height
    );
    painter.setRenderHint(QPainter::Antialiasing, false);
    // Paint the diagram itself as available in the cache.
    updateDiagramCache();
    painter.drawImage(0, 0, m_diagramImage);

    // Paint a thin color wheel for better orientation
    updateWheelCache();
    painter.drawImage(
        0,
        0,
        m_wheelImage
    );

    // paint also an additional marker indicating the hue
    if (m_isMouseEventActive) {
        qreal radius =
            m_diameter / static_cast<qreal>(2) - 2 * markerThickness;
        // get widget coordinates for our marker
        QPointF myMarkerInner = PolarPointF(
                radius - 4 * markerThickness,
                m_color.toLch().h
        ).toCartesian();
        QPointF myMarkerOuter = PolarPointF(
                radius,
                m_color.toLch().h
            ).toCartesian();
        myMarkerInner.ry() *= -1;
        myMarkerOuter.ry() *= -1;
        myMarkerInner += QPointF(m_diagramOffset, m_diagramOffset);
        myMarkerOuter += QPointF(m_diagramOffset, m_diagramOffset);
        // draw the line
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
     * this apporach does not work well.
     * 
     * It seems better to paint a focus indicator for the whole widget.
     * We could use the style primitives to paint a rectangular focus
     * indicator around the whole widget:
     * style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
     * However, this does not work well because the chroma-lightness diagram
     * has usually a triangular shape. The style primitive, however, often
     * paints just a line at the bottom of the widget. That does not look
     * good. An alternative approach is that we paint ourself a focus
     * indicator only on the left of the diagram (which is the place of
     * black/grey/white, so the won't be any problems with non-harmonic
     * colors). We can get the color from the current style:
     * QColor hightlightColor = palette().color(QPalette::Highlight);
     * QBrush highlightBrush = palette().highlight();
     * Then we have to design the line that we want to display. It is better
     * to do that ourself instead of relying on generic QStyle::PE_Frame or
     * similar solutions as their result seems to be quite unpredictible
     * accross various styles. So we use markerThickness as line width and
     * paint it at the left-most possible position. As the border() property
     * accomodates also to markerRadius, the distance of the focus line to
     * the real diagram also does, which looks nice.
     */
    if (hasFocus()) {
        painter.setRenderHint(QPainter::Antialiasing);
        pen = QPen();
        pen.setWidth(markerThickness);
        pen.setColor(
            palette().color(QPalette::Highlight)
        );
        painter.setPen(pen);
        brush = QBrush(Qt::transparent);
        painter.setBrush(brush);
        painter.drawEllipse(
            QPoint(m_diagramOffset + 1, m_diagramOffset + 1),   // center
            (m_diameter - markerThickness) / 2,   // x radius
            (m_diameter - markerThickness) / 2    // y radius
        );
    }

    // Paint the marker on-the-fly.
    painter.setRenderHint(QPainter::Antialiasing);
    QPoint imageCoordinates = imageCoordinatesFromColor();
    pen = QPen();
    pen.setWidth(markerThickness);
    // Set color of the marker: Black or white, depending on the lightness of
    // the currently selected color.
    if (m_color.toLch().L >= 50) { // range: 0..100
        pen.setColor(Qt::black);
    } else {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);
    brush = QBrush(Qt::transparent);
    painter.setBrush(brush);
    painter.drawEllipse(
        imageCoordinates.x() - markerRadius,
        imageCoordinates.y() - markerRadius,
        2 * markerRadius + 1,
        2 * markerRadius + 1
    );
    PolarPointF lineEndPolar = PolarPointF(
        QPointF(
            imageCoordinates.x() - m_diagramOffset,
            (imageCoordinates.y() - m_diagramOffset) * (-1)
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
        lineEndCartesian += QPointF(m_diagramOffset, m_diagramOffset);
        painter.drawLine(
            QPointF(m_diagramOffset, m_diagramOffset),
            lineEndCartesian
        );
    }

    // Paint the buffer to the actual widget
    QPainter(this).drawImage(0, 0, paintBuffer);
}

void ChromaHueDiagram::wheelEvent(QWheelEvent* event)
{
    if (
        /* Do nothing while mouse mouvement is tracked anyway. This would be confusing. */
        (!m_isMouseEventActive) &&
        /* Only react on wheel events when its in the appropriate place */
        // TODO Wouldn't it be better to accept wheel events in the whole gray circle, even if outside the gamut?
        areImageCoordinatesWithinDiagramSurface(event->pos()) &&
        /* Only react on good old vertical wheels, and not on horizontal wheels */
        (event->angleDelta().y() != 0)
    ) {
        cmsCIELCh lch = m_color.toLch();
        lch.h += Helper::standardWheelSteps(event) * singleStepHue;
        setColor(
            FullColorDescription(
                m_rgbColorSpace,
                lch,
                FullColorDescription::outOfGamutBehaviour::sacrifyChroma
            )
        );
    } else {
        event->ignore();
    }
}

// TODO xxx What happens when maxChroma is low and parts of the gamut are out-of-display?
// TODO xxx Melt together the displayed gamut and the gray circle to avoid strange alpha-blending
// at the anti-aliased parts

/** @brief React on key press events.
 * 
 * Reimplemented from base class.
 * 
 * Reacts on key press events. When the arrow keys are pressed, it moves the
 * marker by one pixel into the desired direction if this is still within
 * gamut. When <tt>Qt::Key_PageUp</tt>, <tt>Qt::Key_PageDown</tt>,
 * <tt>Qt::Key_Home</tt> or <tt>Qt::Key_End</tt> are pressed, it moves the
 * marker as much as possible into the desired direction as long as this is
 * still in the gamut.
 * 
 * @param event the paint event */
void ChromaHueDiagram::keyPressEvent(QKeyEvent *event)
{
    // TODO The choise of keys does not seem to be very intuitive
    cmsCIELCh lch = m_color.toLch();
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
             * If you reimplement this handler, it is very important that you call the base class
             * implementation if you do not act upon the key.
             * 
             * The default implementation closes popup widgets if the user presses the key sequence
             * for QKeySequence::Cancel (typically the Escape key). Otherwise the event is ignored,
             * so that the widget's parent can interpret it.
             */
            QWidget::keyPressEvent(event);
            return;
    }
    // Here we reach only if the key has been recognized. If not, in the default branch of the
    // switch statement, we would have passed the keyPressEvent yet to the parent and returned.
    if (lch.C < 0) {
        lch.C = 0;
    }
    setColor(
        FullColorDescription(
            m_rgbColorSpace,
            lch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
    );
}

/**
 * @param imageCoordinates the image coordiantes
 * @returns the diagram (a-b) value for given image coordinates
 */
QPointF ChromaHueDiagram::fromImageCoordinatesToAB(
    const QPoint imageCoordinates
)
{
    const qreal scaleFactor =
        static_cast<qreal>(2 * m_maxChroma) / (m_diameter - 2 * border);
    return QPointF(
        (imageCoordinates.x() - m_diagramOffset) * scaleFactor,
        (imageCoordinates.y() - m_diagramOffset) * scaleFactor * (-1)
    );
}

/**
 * @returns the coordinates for m_color()
 */
QPoint ChromaHueDiagram::imageCoordinatesFromColor()
{
    const qreal scaleFactor =
        (m_diameter - 2 * border) / static_cast<qreal>(2 * m_maxChroma);
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
 * @param imageCoordinates the image coordiantes to test
 * @returns <tt>true</tt> if the given image coordiantes are within this
 * circle, <tt>false</tt> otherwise. */
bool ChromaHueDiagram::areImageCoordinatesWithinDiagramSurface(
    const QPoint imageCoordinates
)
{
    qreal radialPixel = PerceptualColor::PolarPointF(
        imageCoordinates - QPoint(m_diagramOffset, m_diagramOffset)
    ).radial();
    return (
        radialPixel <= ( (m_diameter - 1) / static_cast<qreal>(2) )
    );
}

/** @brief Setter for the color() property */
void ChromaHueDiagram::setColor(const FullColorDescription &newColor)
{
    if (newColor == m_color) {
        return;
    }

    FullColorDescription oldColor = m_color;
    m_color = newColor;

    // update if necessary, the diagram
    if (m_color.toLch().L != oldColor.toLch().L) {
        m_diagramCacheReady = false;
    }

    // Schedule a paint event:
    update();
    Q_EMIT colorChanged(newColor);
}

/** @brief React on a resive event.
 *
 * Reimplemented from base class.
 * 
 * @param event The corresponding resize event
 */
void ChromaHueDiagram::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    // Find the smaller one of width and height
    int newDiameter = qMin(size().width(), size().height());
    // Make sure the diameter is an odd integer (so that center of the
    // coordinate system is exactly at a pixel, and not between two
    // pixels.
    if ((newDiameter % 2) == 0) {
        newDiameter -= 1;
    }
    if (newDiameter < 0) {
        newDiameter = 0;
    }
    // Update the widget
    if (newDiameter != m_diameter) {
        m_diameter = newDiameter;
        m_diagramOffset = (m_diameter - 1) / 2;
        m_diagramCacheReady = false;
        m_isWheelCacheReady = false;
        // As by Qt documentation: The widget will be erased and receive a
        // paint event immediately after processing the resize event. No
        // drawing need be (or should be) done inside this handler.
    }
}

// TODO how to treat empty images because the color profile does not work or the resolution is too small?

/** @brief Provide the size hint.
 *
 * Reimplemented from base class.
 * 
 * @returns the size hint
 * 
 * @sa minimumSizeHint()
 */
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
 * @sa sizeHint()
 */
QSize ChromaHueDiagram::minimumSizeHint() const
{
    return QSize(100, 100);
}

// TODO rework all "throw" statements (also these in comments) and the qDebug() statements

// TODO what to do if a gamut allows lightness < 0 or lightness > 100 ???

// TODO allow imaginary colors?

// No documentation here (documentation of properties
// and its getters are in the header)
FullColorDescription ChromaHueDiagram::color() const
{
    return m_color;
}

// How to make sure the diagram has at least a few pixels?
QImage ChromaHueDiagram::generateDiagramImage2(
    const RgbColorSpace *colorSpace,
    const int imageSize,
    const qreal maxChroma,
    const qreal lightness,
    const int border
)
{
    Q_UNUSED(colorSpace);
/*
Re: [Lcms-user] Out-of-gamut colours?
From: Marti <marti@li...> - 2005-07-29 08:23:37

Hi,

There is a special built-in profile called the null profile. This
profile operates on gray scale and returns always zero. Since the gamut
warning bypasses the output profile, you can use the null profile to do
a quick gamut check: You will obtain 0 for in-gamut and 255 (or whatever
gamut alarm you set) for out of gamut.

Example:

You want to check gamut colors on cmyk.icc, then:

    cmsSetAlarmCodes(255, 255, 255)     // This would turn gray alarm code to 255
    hNULL =  cmsCreateNULLProfile();
    cmsCreateProofingTransform(
        hXYZ,
        TYPE_XYZ_DBL,
        hNULL, TYPE_GRAY_8,
        hCMYK,
        INTENT_RELATIVE_COLORIMETRIC,
        INTENT_RELATIVE_COLORIMETRIC,
        (cmsFLAGS_GAMUTCHECK|cmsFLAGS_SOFTPROOFING)
    );

Then, for checking a single color

    cmsCIEXYZ XYZ;
    BYTE gamut;

... put value to check into XYZ ...

    cmsDoTransform(xform, &XYZ, &gamut, 1);
*/
    QElapsedTimer myTimer;
    myTimer.start();

    /* We do not call cmsSetAlarmCodes(alarmCodes); because this seems to be
     * not able to change the alpa channel. And apparently, with gamut
     * checking, transparency seems to be the default for out-of-gamut
     * colors anyway, so no need to do anything. Anyway, here is some
     * example code snipped that could change the alarm code. */
    // cmsUInt16Number alarmCodes[cmsMAXCHANNELS];
    // cmsUInt16Number alarmCode = std::numeric_limits<cmsUInt16Number>::max();
    // for (int i = 0; i < cmsMAXCHANNELS; ++i) {
    //     alarmCodes[i] = alarmCode;
    // }
    // cmsSetAlarmCodes(alarmCodes);

    // There is a special built-in profile called the null profile. This
    // profile operates on gray scale and returns always zero. Since the gamut
    // warning bypasses the output profile, you can use the null profile to do
    // a quick gamut check: You will obtain 0 for in-gamut and 255 (or
    // whatever gamut alarm you set) for out of gamut. Currently we do not use
    // this technique.
    // cmsHPROFILE hNULL = cmsCreateNULLProfile();
    cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(NULL);
    cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();

    cmsHTRANSFORM xform = cmsCreateProofingTransform(
        labProfileHandle,                           // Input profile handle
        TYPE_Lab_DBL,                               // InputFormat
        rgbProfileHandle,                           // Output
        TYPE_BGRA_8,                                // OutputFormat TODO endianess?
        rgbProfileHandle,                           // Proofing
        INTENT_ABSOLUTE_COLORIMETRIC,               // Intent
        INTENT_ABSOLUTE_COLORIMETRIC,               // ProofingIntent
        (cmsFLAGS_SOFTPROOFING|cmsFLAGS_GAMUTCHECK) // dwFlags // TODO cmsFLAGS_SOFTPROOFING slown down. And cmsFLAGS_GAMUTCHECK slows down a lot and is not exact.
    );

    qDebug()
        << "Generating the lcms transform took"
        << myTimer.restart()
        << "ms.";

    const int maxIndex = imageSize - 1;
    // Test if image size is too small.
    if (maxIndex < 1) {
        // maxIndex must be at least >= 1 for our algorithm. If they are 0, this would crash (division by 0). // TODO how to solve this?
        return QImage();
    }

    // Setup
    cmsCIELab lab; // uses cmsFloat64Number internally
    int x;
    int y;
    const qreal scaleFactor =
        static_cast<qreal>(2 * maxChroma) / (imageSize - 2 * border);

    int myBufferSize = imageSize * imageSize * 3;
    double *myBuffer = new double[myBufferSize];

    // Paint the gamut.
    lab.L = lightness;
    int index = 0;
    for (y = 0; y <= maxIndex; ++y) {
        lab.b = maxChroma - (y - border) * scaleFactor;
        for (x = 0; x <= maxIndex; ++x) {
            lab.a = (x - border) * scaleFactor - maxChroma;
//             if ((qPow(lab.a, 2) + qPow(lab.b, 2)) <= (qPow(maxChroma, 2))) {
//                 tempColor = colorSpace->colorRgb(lab);
//                 if (tempColor.isValid()) {
//                     // The pixel is within the gamut
//                     tempImage.setPixelColor(x, y, tempColor);
//                 }
//             }
            // Buffer for 3 channels à 1 bytes
            myBuffer[index] = lab.L;
            ++index;
            myBuffer[index] = lab.a;
            ++index;
            myBuffer[index] = lab.b;
            ++index;
        }
    }

    static_assert(
        sizeof(uchar) == sizeof(quint8),
        "LittleCMS expects uchar as buffer type. We are using however quint8 "
            "to be sure it is actually 8 bit (important for the rest of the "
            "image processing code. Therefore uchar and quint8 must have the "
            "same size to make the code reliable."
    );
    const std::size_t outputBufferArrayLength = imageSize * imageSize * 4;
    quint8 *outputBuffer = new quint8[outputBufferArrayLength];
    for (std::size_t i = 0 ; i < outputBufferArrayLength; ++i) {
        outputBuffer[i] = 250;
    }

    cmsDoTransform(
        xform,
        myBuffer,
        outputBuffer,
        imageSize * imageSize
    );

    QImage tempImage = QImage(
        outputBuffer,
        imageSize,
        imageSize,
        QImage::Format_ARGB32
    );
    
    qDebug()
        << "Generating chroma-hue gamut image took"
        << myTimer.restart()
        << "ms.";

    QImage result = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32
    );

    result.fill(Qt::transparent);

    // Cut of everything outside the circle
    QPainter myPainter(&result);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::NoPen));
    myPainter.setBrush(QBrush(tempImage));
    myPainter.drawEllipse(
        border,                 // x
        border,                 // y
        imageSize - 2 * border, // width
        imageSize - 2 * border  // height
    );

    delete[] myBuffer;

    // Delete the original content of tempImage before deleting outputBuffer,
    // because tempImage relies on the presence of outputBuffer.
    tempImage = QImage();
    delete[] outputBuffer;
    qDebug()
        << "Generating chroma-hue gamut image took cleanup"
        << myTimer.restart() << "ms.";
    return result;
}

/** @brief in image of a-b plane of the color space at a given lightness */
// How to make sure the diagram has at least a few pixels?
QImage ChromaHueDiagram::generateDiagramImage3(
    const RgbColorSpace *colorSpace,
    const int imageSize,
    const qreal maxChroma,
    const qreal lightness,
    const int border
)
{
    cmsContext ContextID = cmsCreateContext(NULL, NULL);
    cmsHANDLE h_gamutBoundaryDescriptor = cmsGBDAlloc(ContextID);
    qreal h;
    qreal s;
    qreal v;
    constexpr qreal step = 0.1;
    cmsCIELab lab; //     cmsCIELab lab; 
    QColor color;
    for (h = 0; h < 360; h += step) {
        s = 255;
        for (v = 0; v <= 255; v += step) {
            lab = colorSpace->colorLab(QColor::fromHsv(h, s, v));
            cmsGDBAddPoint(h_gamutBoundaryDescriptor, &lab);
        }
        v = 255;
        for (s = 0; s <= 255; s += step) {
            lab = colorSpace->colorLab(QColor::fromHsv(h, s, v));
            cmsGDBAddPoint(h_gamutBoundaryDescriptor, &lab);
        }
    }
    cmsGDBCompute(
        h_gamutBoundaryDescriptor,
        0 // As of LittleCMS documentation: dwFlags: reserved (unused). Set it to 0
    );


    const int maxIndex = imageSize - 1;
    // Test if image size is too small.
    if (maxIndex < 1) {
        // maxIndex must be at least >= 1 for our algorithm. If they are 0,
        // this would crash (division by 0). TODO how to solve this?
        return QImage();
    }
    
    // Setup
    int x;
    int y;
    QColor tempColor;
    QImage tempImage = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32
    );
    tempImage.fill(Qt::transparent); // Initialize the image with transparency
    const qreal scaleFactor =
        static_cast<qreal>(2 * maxChroma) / (imageSize - 2 * border);
 
    QElapsedTimer myTimer; // TODO remove me!
    myTimer.start(); // TODO remove me!
    // Paint the gamut.
    lab.L = lightness;
    for (y = border; y <= maxIndex - border; ++y) {
        lab.b = maxChroma - (y - border) * scaleFactor;
        for (x = border; x <= maxIndex - border; ++x) {
            lab.a = (x - border) * scaleFactor - maxChroma;
            if ((qPow(lab.a, 2) + qPow(lab.b, 2)) <= (qPow(maxChroma, 2))) {
                tempColor = colorSpace->colorRgbBound(lab);
                if (cmsGDBCheckPoint(h_gamutBoundaryDescriptor, &lab)) {
                    // The pixel is within the gamut
                    tempImage.setPixelColor(x, y, tempColor);
                }
            }
        }
    }
    qDebug() << "Generating chroma-hue gamut image took" << myTimer.restart() << "ms.";  // TODO remove me!

    QImage result = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32
    );
    result.fill(Qt::transparent);
    // Cut of everything outside the circle
    QPainter myPainter(&result);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::NoPen));
    myPainter.setBrush(QBrush(tempImage));
    myPainter.drawEllipse(
        border,                 // x
        border,                 // y
        imageSize - 2 * border, // width
        imageSize - 2 * border  // height
    );

    cmsGBDFree(h_gamutBoundaryDescriptor);

    return result;
}

/** @brief in image of a-b plane of the color space at a given lightness */
// How to make sure the diagram has at least a few pixels?
QImage ChromaHueDiagram::generateDiagramImage(
    const RgbColorSpace *colorSpace,
    const int imageSize,
    const qreal maxChroma,
    const qreal lightness,
    const int border
)
{
    const int maxIndex = imageSize - 1;
    // Test if image size is too small.
    if (maxIndex < 1) {
        // maxIndex must be at least >= 1 for our algorithm. If they are 0,
        // this would crash (division by 0). TODO how to solve this?
        return QImage();
    }
    
    // Setup
    cmsCIELab lab; // uses cmsFloat64Number internally
    int x;
    int y;
    QColor tempColor;
    QImage tempImage = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32
    );
    tempImage.fill(Qt::transparent); // Initialize the image with transparency
    const qreal scaleFactor =
        static_cast<qreal>(2 * maxChroma) / (imageSize - 2 * border);
 
    QElapsedTimer myTimer; // TODO remove me!
    myTimer.start(); // TODO remove me!
    // Paint the gamut.
    lab.L = lightness;
    for (y = border; y <= maxIndex - border; ++y) {
        lab.b = maxChroma - (y - border) * scaleFactor;
        for (x = border; x <= maxIndex - border; ++x) {
            lab.a = (x - border) * scaleFactor - maxChroma;
            if ((qPow(lab.a, 2) + qPow(lab.b, 2)) <= (qPow(maxChroma, 2))) {
                tempColor = colorSpace->colorRgb(lab);
                if (tempColor.isValid()) {
                    // The pixel is within the gamut
                    tempImage.setPixelColor(x, y, tempColor);
                }
            }
        }
    }
    qDebug() << "Generating chroma-hue gamut image took" << myTimer.restart() << "ms.";  // TODO remove me!

    QImage result = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32
    );
    result.fill(Qt::transparent);
    // Cut of everything outside the circle
    QPainter myPainter(&result);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::NoPen));
    myPainter.setBrush(QBrush(tempImage));
    myPainter.drawEllipse(
        border,                 // x
        border,                 // y
        imageSize - 2 * border, // width
        imageSize - 2 * border  // height
    );

    return result;
}


/** @brief Refresh the diagram and associated data
 * 
 * This class has a cache of various data related to the diagram
 * - @ref m_diagramImage
 * 
 * This data is cached because it is often needed and it would be expensive to calculate it
 * again and again on the fly.
 * 
 * Calling this function updates this cached data. This is always necessary if the data
 * the diagram relies on, has changed. Call this function always before using the cached data!
 * 
 * This function does not repaint the widget! After calling this function, you have to call
 * manually <tt>update()</tt> to schedule a re-paint of the widget, if you wish so.
 */
void ChromaHueDiagram::updateDiagramCache()
{
    if (m_diagramCacheReady) {
        return;
    }

    // Update QImage
    m_diagramImage = generateDiagramImage(
        m_rgbColorSpace,
        m_diameter,
        m_maxChroma,
        m_color.toLch().L,
        border
    );

    // Mark cache as ready
    m_diagramCacheReady = true;
}

/** @brief Refresh the wheel and associated data
 * 
 * This class has a cache of various data related to the diagram
 * - @ref m_wheelImage
 * 
 * This data is cached because it is often needed and it would be expensive to calculate it
 * again and again on the fly.
 * 
 * Calling this function updates this cached data. This is always necessary if the data
 * the diagram relies on, has changed. Call this function always before using the cached data!
 * 
 * This function does not repaint the widget! After calling this function, you have to call
 * manually <tt>update()</tt> to schedule a re-paint of the widget, if you wish so.
 */
void ChromaHueDiagram::updateWheelCache()
{
    if (m_isWheelCacheReady) {
        return;
    }

    // Update QImage
    m_wheelImage = SimpleColorWheel::generateWheelImage(
        m_rgbColorSpace,
        m_diameter,                                 // diameter
        2 * markerThickness,                      // border
        4 * markerThickness,                      // thickness
        Helper::LchDefaults::defaultLightness,      // lightness
        Helper::LchDefaults::versatileSrgbChroma    // chroma
    );

    // Mark cache as ready
    m_isWheelCacheReady = true;
}

}
