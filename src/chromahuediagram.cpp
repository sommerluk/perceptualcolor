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

#include <math.h>

#include <QDebug>
#include <QBuffer>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

#include <lcms2.h>

#include <QElapsedTimer>

namespace PerceptualColor {

/** @brief The constructor.
 * @param colorSpace The colorSpace within this widget should operate
 * @param parent Passed to the QWidget base class constructor
 */
ChromaHueDiagram::ChromaHueDiagram(RgbColorSpace *colorSpace, QWidget *parent) : QWidget(parent)
{
    // Setup LittleCMS (must be first thing because other operations rely on working LittleCMS)
    m_rgbColorSpace = colorSpace;

    // Simple initializations
    // We don't use the reset methods as they rely on refreshDiagram
    // (and refreshDiagram relies itself on m_hue, m_markerRadius and
    // m_markerThickness)
    cmsCIELCh temp;
    temp.h = Helper::LchDefaults::defaultHue;
    temp.C = Helper::LchDefaults::versatileSrgbChroma;
    temp.L = Helper::LchDefaults::defaultLightness;
    m_color = FullColorDescription(
        m_rgbColorSpace,
        temp,
        FullColorDescription::outOfGamutBehaviour::sacrifyChroma
    );
    m_markerRadius = default_markerRadius;
    m_markerThickness = default_markerThickness;
    updateBorder();

    // Other initializations
    // Accept focus only by keyboard tabbing and not by mouse click
    // Focus by mouse click is handeled manually by mousePressEvent().
    setFocusPolicy(Qt::FocusPolicy::TabFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

// TODO The widget content should be centered horizontally and vertically. Attention: Then adopt ColorDialog so that this widget still is left-aligned (or right-aligned on RTL) to stay visually together with the lightness selector widget.

/** @brief Updates the border() property.
 * 
 * This function can be called after changes to markerRadius() or markerThickness() to
 * update the border() property.
 */
void ChromaHueDiagram::updateBorder()
{
    // Code
    m_border = 8 * m_markerThickness;
}

// TODO It might be better (here and in the other classes) that when cache is invalidated, the images are
// deleted to free memory. We could just set them to an invalid QImage(). The problem is this would be
// identical to a valid 0-size image, which could then lead to unexpected results (recursion...)...?

// TODO high-dpi support

// TODO automatically scale marker radius and thickness with widget size

// TODO reasonable boundary for markerWidth and markerRadius and minimumSizeHint: How to make sure the diagram has at least a few pixels? And if it's very low: For precision wouldn't it be better to internally calculate with a higher-resolution pixmap for more precision? Alternative: for the border() property: better quint16? No, that's not a good idea...

/** Sets the color values corresponding to image coordinates.
 * 
 * @param newImageCoordinates A coordinte pair within the image's coordinate
 * system. This does not necessarily need to intersect with the actual
 * displayed diagram or the gamut. It might even be negative or outside the
 * image or even outside widget.
 * 
 * \post If the coordinates are within the gamut diagram, then
 * the corresponding values are set. If the coordinates
 * are outside the gamut diagram, then the chroma value is sacrified and hue is
 * maintained.
 */
void ChromaHueDiagram::setWidgetCoordinates(const QPoint newImageCoordinates)
{
    updateDiagramCache();
    QPointF aB;
    cmsCIELab lab;
    if (newImageCoordinates != currentImageCoordinates()) {
        aB = fromImageCoordinatesToAB(newImageCoordinates);
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

// TODO Do not use nearest neigbour or other pixel based search algorithms, but work directly with LittleCMS, maybe with a limited, but well-defined, precision.

/** @brief React on a mouse press event.
 * 
 * Reimplemented from base class.
 *
 * Does not differenciate between left, middle and right mouse click.
 * If the mouse is clicked within the \em displayed gamut, than the marker is placed here and further
 * mouse movements are tracked.
 * 
 * @param event The corresponding mouse event
 */
void ChromaHueDiagram::mousePressEvent(QMouseEvent *event)
{
    if (imageCoordinatesInGamut(event->pos())) { // TODO also accept out-of-gamut clicks when they are covered by the current marker.
        // Mouse focus is handeled manually because so we can accept focus only
        // on mouse clicks within the displayed gamut, while rejecting focus
        // otherwise. In the constructor, therefore Qt::FocusPolicy::TabFocus
        // is specified, so that manual handling of mouse focus is up to
        // this code here.
        // TODO Find another solution that guarantees that setFocusPolicy() API
        // of this class behaves as expected, and not as a dirty hack that
        // accepts mouse focus even when set to Qt::FocusPolicy::TabFocus.
        setFocus(Qt::MouseFocusReason);
        m_mouseEventActive = true;
        setCursor(Qt::BlankCursor);
        setWidgetCoordinates(event->pos());
        update(); // make sure the wheel indicator is drawn when a mouse event is active
    } else {
        // Make sure default behaviour like drag-window in KDE's Breeze widget style works
        QWidget::mousePressEvent(event);
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * Reacts only on mouse move events if previously there had been a mouse press
 * event within the displayed gamut. If the mouse moves inside the \em displayed
 * gamut, the marker is displaced there. If the mouse moves outside the
 * \em display gamut, the marker is displaced to the nearest neigbbour pixel
 * within gamut.
 * 
 * If previously there had not been a mouse press event, the mouse move event is ignored.
 * 
 * @param event The corresponding mouse event
 */
void ChromaHueDiagram::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseEventActive) {
        if (imageCoordinatesInGamut(event->pos())) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        setWidgetCoordinates(event->pos());
    } else {
        // Make sure default behaviour like drag-window in KDE's Breeze widget style works
        QWidget::mousePressEvent(event);
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Does not differenciate between left, middle and right mouse click.
 *
 * If the mouse is inside the \em displayed gamut, the marker is displaced there. If the mouse is
 * outside the \em display gamut, the marker is displaced to the nearest neigbbour pixel within gamut.
 * 
 * @param event The corresponding mouse event
 */
void ChromaHueDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_mouseEventActive) {
        unsetCursor();
        m_mouseEventActive = false;
        setWidgetCoordinates(event->pos());
        update(); // make sure the wheel indicator is not drawn when no mouse event is active
    } else {
        // Make sure default behaviour like drag-window in KDE's Breeze widget style works
        QWidget::mousePressEvent(event);
    }
}

// TODO What when m_color has a valid in-gamut color, but this color is out of the _displayed_ diagram? How to handle that?

/** @brief Paint the widget.
 * 
 * Reimplemented from base class.
 * 
 * Paints the widget. Takes the existing m_diagramImage and m_diagramPixmap and paints
 * them on the widget. Paints, if approperiate, the focus indicator. Paints the marker.
 * Relies on that m_diagramImage and m_diagramPixmap are up to date.
 * 
 * @param event the paint event
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
        m_border,                   // x
        m_border,                   // y
        m_diameter - 2 * m_border,  // width
        m_diameter - 2 * m_border   // height
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
    if (m_mouseEventActive) {
        qreal radius =
            m_diameter / static_cast<qreal>(2) - 2 * m_markerThickness;
        // get widget coordinates for our marker
        QPointF myMarkerInner = PolarPointF(
                radius - 4 * m_markerThickness,
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
        pen.setWidth(m_markerThickness);
        pen.setCapStyle(Qt::FlatCap);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(myMarkerInner, myMarkerOuter);
    }
  
    /* Paint a focus indicator.
     * 
     * We could paint a focus indicator (round or rectangular) around the marker.
     * Depending on the currently selected hue for the diagram, it looks ugly
     * because the colors of focus indicator and diagram do not harmonize, or
     * it is mostly invisible the the colors are similar. So this apporach does
     * not work well.
     * 
     * It seems better to paint a focus indicator for the whole widget.
     * We could use the style primitives to paint a rectangular focus indicator
     * around the whole widget:
     * style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
     * However, this does not work well because the chroma-lightness diagram has
     * usually a triangular shape. The style primitive, however, often paints
     * just a line at the bottom of the widget. That does not look good. An
     * alternative approach is that we paint ourself a focus indicator only
     * on the left of the diagram (which is the place of black/grey/white,
     * so the won't be any problems with non-harmonic colors). We can
     * get the color from the current style:
     * QColor hightlightColor = palette().color(QPalette::Highlight);
     * QBrush highlightBrush = palette().highlight();
     * Then we have to design the line that we want to display. It is better to
     * do that ourself instead of relying on generic QStyle::PE_Frame or similar
     * solutions as their result seems to be quite unpredictible accross various
     * styles. So we use markerThickness as line width and paint it at the
     * left-most possible position. As the border() property accomodates also to
     * markerRadius, the distance of the focus line to the real diagram also
     * does, which looks nice.
     */
    if (hasFocus()) {
        painter.setRenderHint(QPainter::Antialiasing);
        pen = QPen();
        pen.setWidth(m_markerThickness);
        pen.setColor(
            palette().color(QPalette::Highlight)
        );
        painter.setPen(pen);
        brush = QBrush(Qt::transparent);
        painter.setBrush(brush);
        painter.drawEllipse(
            QPoint(m_diagramOffset + 1, m_diagramOffset + 1),   // center
            (m_diameter - m_markerThickness) / 2,   // x radius
            (m_diameter - m_markerThickness) / 2    // y radius
        );
    }

    // Paint the marker on-the-fly.
    painter.setRenderHint(QPainter::Antialiasing);
    QPoint imageCoordinates = currentImageCoordinates();
    pen = QPen();
    pen.setWidth(m_markerThickness);
    int markerBackgroundLightness = m_color.toLch().L; // range: 0..100
    if (markerBackgroundLightness >= 50) {
        pen.setColor(Qt::black);
    } else {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);
    brush = QBrush(Qt::transparent);
    painter.setBrush(brush);
    painter.drawEllipse(
        imageCoordinates.x() - m_markerRadius,
        imageCoordinates.y() - m_markerRadius,
        2 * m_markerRadius + 1,
        2 * m_markerRadius + 1
    );
    PolarPointF lineEndPolar = PolarPointF(
        QPointF(
            imageCoordinates.x() - m_diagramOffset,
            (imageCoordinates.y() - m_diagramOffset) * (-1)
        )
    );
    qreal newRadial = lineEndPolar.radial() - m_markerRadius - 1.5;
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
        (!m_mouseEventActive) &&
        /* Only react on wheel events when its in the appropriate place */
        // TODO Wouldn't it be better to accept wheel events in the whole gray circle, even if outside the gamut?
        imageCoordinatesInGamut(event->pos()) &&
        /* Only react on good old vertical wheels, and not on horizontal wheels */
        (event->angleDelta().y() != 0)
    ) {
        cmsCIELCh lch = m_color.toLch();
        lch.h += Helper::standardWheelSteps(event) * m_singleStepHue;
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
 * Reacts on key press events. When the arrow keys are pressed, it moves the marker by one pixel
 * into the desired direction if this is still within gamut. When @c Qt::Key_PageUp,
 * @c Qt::Key_PageDown, @c Qt::Key_Home or @c Qt::Key_End are pressed, it moves the marker as much
 * as possible into the desired direction as long as this is still in the gamut.
 * 
 * @param event the paint event */
void ChromaHueDiagram::keyPressEvent(QKeyEvent *event)
{
    // TODO The choise of keys does not seem to be very intuitive
    cmsCIELCh lch = m_color.toLch();
    switch (event->key()) {
        case Qt::Key_Up:
            lch.C += m_singleStepChroma;
            break;
        case Qt::Key_Down:
            lch.C -= m_singleStepChroma;
            break;
        case Qt::Key_Left:
            lch.h += m_singleStepHue;
            break;
        case Qt::Key_Right:
            lch.h -= m_singleStepHue;
            break;
        case Qt::Key_PageUp:
            lch.C += m_pageStepChroma;
            break;
        case Qt::Key_PageDown:
            lch.C -= m_pageStepChroma;
            break;
        case Qt::Key_Home:
            lch.h += m_pageStepHue;
            break;
        case Qt::Key_End:
            lch.h -= m_pageStepHue;
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
QPointF ChromaHueDiagram::fromImageCoordinatesToAB(const QPoint imageCoordinates)
{
    const qreal scaleFactor = static_cast<qreal>(2 * m_maxChroma) / (m_diameter - 2 * m_border);
    return QPointF(
        (imageCoordinates.x() - m_diagramOffset) * scaleFactor,
        (imageCoordinates.y() - m_diagramOffset) * scaleFactor * (-1)
    );
}

/**
 * @returns the coordinates for m_color()
 */
QPoint ChromaHueDiagram::currentImageCoordinates()
{
    const qreal scaleFactor = (m_diameter - 2 * m_border) / static_cast<qreal>(2 * m_maxChroma);
    return QPoint(
        qRound((m_color.toLab().a * scaleFactor + m_diagramOffset)),
        qRound((m_color.toLab().b * scaleFactor + m_diagramOffset) * (-1) + 2 * m_diagramOffset)
    );
}

/** @brief Tests if image coordinates are in gamut.
 *  @returns @c true if the image coordinates are within the displayed gamut. Otherwise @c false.
 */
bool ChromaHueDiagram::imageCoordinatesInGamut(const QPoint imageCoordinates)
{
    // variables
    bool temp;
    QColor diagramPixelColor;

    // code
    updateDiagramCache();
    temp = false;
    if (m_diagramImage.valid(imageCoordinates)) {
        diagramPixelColor = m_diagramImage.pixelColor(imageCoordinates);
        temp = ((diagramPixelColor.alpha() != 0));
    }
    return temp;
}

qreal ChromaHueDiagram::lightness() const
{
    return m_color.toLch().L;
}

/** @brief Set the lightness
 * 
 * convenience function
 * 
 *  @param newLightness The new lightness value to set.
 */
void ChromaHueDiagram::setLightness(const qreal newLightness)
{
    if (newLightness == m_color.toLch().L) {
        return;
    }
    cmsCIELCh lch;
    lch.h = m_color.toLch().h;
    lch.C = m_color.toLch().C;
    lch.L = newLightness;
    setColor(
        FullColorDescription(
            m_rgbColorSpace,
            lch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
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

    // schedule a paint event
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
        m_wheelCacheReady = false;
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

int ChromaHueDiagram::markerRadius() const
{
    return m_markerRadius;
}

/** @brief Setter for the markerRadius() property.
 * 
 * @param newMarkerRadius the new marker radius
 */
void ChromaHueDiagram::setMarkerRadius(const int newMarkerRadius)
{
// TODO bound markerRadius and markerThickness to maximum 10 px (or something like that) to make sure the minimal size hint of the widget still allows a diagram to be displayed. (And border property does not overflow.)
    int temp = qMax(newMarkerRadius, 0);
    if (m_markerRadius != temp) {
        m_markerRadius = temp;
        updateBorder();
        m_diagramCacheReady = false; // because the border has changed, so the size of the pixmap will change.
        update();
    }
}

/** @brief Reset the markerRadius() property.
 */
void ChromaHueDiagram::resetMarkerRadius()
{
    setMarkerRadius(default_markerRadius);
}

int ChromaHueDiagram::markerThickness() const
{
    return m_markerThickness;
}

/** @brief Setter for the markerThickness() property.
 * 
 * @param newMarkerThickness the new marker thickness
 */
void ChromaHueDiagram::setMarkerThickness(const int newMarkerThickness)
{
    int temp = qMax(newMarkerThickness, 0);
    if (m_markerThickness != temp) {
        m_markerThickness = temp;
        updateBorder();
        m_diagramCacheReady = false; // because the border has changed, so the size of the pixmap will change.
        m_wheelCacheReady = false;
        update();
    }
}

/** @brief Reset the markerThickness() property. */
void ChromaHueDiagram::resetMarkerThickness()
{
    setMarkerThickness(default_markerThickness);
}

int ChromaHueDiagram::border() const
{
    return m_border;
}

/** @brief Color of the current selection
 * 
 * @returns color of the current selection
 */
FullColorDescription ChromaHueDiagram::color() const
{
    return m_color;
}

QImage ChromaHueDiagram::generateDiagramImage(
    const RgbColorSpace *colorSpace,
    const int imageSize,
    const qreal maxChroma,
    const qreal lightness,
    const int border
)
{
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
        TYPE_Lab_FLT,                               // InputFormat
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
    const qreal scaleFactor = static_cast<qreal>(2 * maxChroma) / (imageSize - 2 * border);

    int myBufferSize = imageSize * imageSize * 3;
    float *myBuffer = new float[myBufferSize];
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
    const int outputBufferArrayLength = imageSize * imageSize * 4;
    quint8 *outputBuffer = new quint8[outputBufferArrayLength];
    for (int i = 0 ; i < outputBufferArrayLength; ++i) {
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
QImage ChromaHueDiagram::generateDiagramImage2(
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
 * manually @c update() to schedule a re-paint of the widget, if you wish so.
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
        m_border
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
 * manually @c update() to schedule a re-paint of the widget, if you wish so.
 */
void ChromaHueDiagram::updateWheelCache()
{
    if (m_wheelCacheReady) {
        return;
    }

    // Update QImage
    m_wheelImage = SimpleColorWheel::generateWheelImage(
        m_rgbColorSpace,
        m_diameter,                                 // diameter
        2 * m_markerThickness,                      // border
        4 * m_markerThickness,                      // thickness
        Helper::LchDefaults::defaultLightness,      // lightness
        Helper::LchDefaults::versatileSrgbChroma    // chroma
    );

    // Mark cache as ready
    m_wheelCacheReady = true;
}

}
