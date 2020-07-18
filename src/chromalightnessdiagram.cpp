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
#include "PerceptualColor/chromalightnessdiagram.h"

#include "PerceptualColor/helper.h"
#include "PerceptualColor/polarpointf.h"

#include <math.h>

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

#include <lcms2.h>

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent Passed to the QWidget base class constructor
 * @param f Passed to the QWidget base class constructor
 */
ChromaLightnessDiagram::ChromaLightnessDiagram(RgbColorSpace *colorSpace, QWidget *parent) : QWidget(parent)
{
    // Setup LittleCMS (must be first thing because other operations rely on working LittleCMS)
    m_rgbColorSpace = colorSpace;

    // Simple initializations
    // We don't use the reset methods as they rely on refreshDiagram
    // (and refreshDiagram relies itself on m_hue, m_markerRadius and
    // m_markerThickness)
    cmsCIELCh temp;
    temp.h = Helper::LchBoundaries::defaultHue;
    temp.C = Helper::LchBoundaries::versatileSrgbChroma;
    temp.L = Helper::LchBoundaries::defaultLightness;
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

/** @brief Updates the border() property.
 * 
 * This function can be called after changes to markerRadius() or markerThickness() to
 * update the border() property.
 */
void ChromaLightnessDiagram::updateBorder()
{
    // Code
    m_border = qRound(m_markerRadius + (m_markerThickness / static_cast<qreal>(2)));
}

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
 * are outside the gamut diagram, then a nearest-neigbbour-search is done,
 * searching for the pixel that is less far from the cursor.
 */
void ChromaLightnessDiagram::setImageCoordinates(const QPoint newImageCoordinates)
{
    updateDiagramCache();
    QPoint correctedImageCoordinates = Helper::nearestNeighborSearch(newImageCoordinates, m_diagramImage);
    QPointF chromaLightness;
    cmsCIELCh lch;
    if (correctedImageCoordinates != currentImageCoordinates()) {
        chromaLightness = fromImageCoordinatesToChromaLightness(correctedImageCoordinates);
        lch.C = chromaLightness.x();
        lch.L = chromaLightness.y();
        lch.h = m_color.toLch().h;
        setColor(
            FullColorDescription(
                m_rgbColorSpace,
                lch,
                FullColorDescription::outOfGamutBehaviour::preserve
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
void ChromaLightnessDiagram::mousePressEvent(QMouseEvent *event)
{
    QPoint imageCoordinates = fromWidgetCoordinatesToImageCoordinates(
        event->pos()
    );
    if (imageCoordinatesInGamut(imageCoordinates)) { // TODO also accept out-of-gamut clicks when they are covered by the current marker.
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
        setImageCoordinates(imageCoordinates);
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
void ChromaLightnessDiagram::mouseMoveEvent(QMouseEvent *event)
{
    QPoint imageCoordinates = fromWidgetCoordinatesToImageCoordinates(
        event->pos()
    );
    if (m_mouseEventActive) {
        if (imageCoordinatesInGamut(imageCoordinates)) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        setImageCoordinates(imageCoordinates);
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
void ChromaLightnessDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_mouseEventActive) {
        setImageCoordinates(
            fromWidgetCoordinatesToImageCoordinates(event->pos())
        );
        unsetCursor();
        m_mouseEventActive = false;
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
 */
void ChromaLightnessDiagram::paintEvent(QPaintEvent* event)
{
    // We do not paint directly on the widget, but on a QImage buffer first:
    // Render anti-aliased looks better. But as Qt documentation says:
    //
    //      “Renderhints are used to specify flags to QPainter that may or
    //       may not be respected by any given engine.”
    //
    // Painting here directly on the widget might lead to different
    // anti-aliasing results depending on the underlying window system. This is
    // especially problematic as anti-aliasing might shift or not a pixel to the
    // left or to the right. So we paint on a QImage first. As QImage (at
    // difference to QPixmap and a QWidget) is independant of native platform
    // rendering, it guarantees identical anti-aliasing results on all
    // platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should use
    //       the platform independent QImage as paint device; i.e. using QImage
    //       will ensure that the result has an identical pixel representation
    //       on any platform.”
    QImage paintBuffer(size(), QImage::Format_ARGB32);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);

    QPen pen;

    // Paint the diagram itself as available in the cache.
    updateDiagramCache();
    painter.drawImage(m_border, m_border, m_diagramImage);

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
        pen.setWidth(m_markerThickness);
        pen.setColor(
            palette().color(QPalette::Highlight)
        );
        painter.setPen(pen);
        painter.drawLine(
            m_markerThickness / 2, // 0.5 is rounded down to 0.0
            0 + m_border,
            m_markerThickness / 2, // 0.5 is rounded down to 0.0
            size().height() - m_border
        );
    }

    // Paint the marker on-the-fly.
    // Render anti-aliased looks better. But as Qt documentation says: “Renderhints are used to
    // specify flags to QPainter that may or may not be respected by any given engine.” Now, we
    // are painting here directly on the widget, which might lead to different anti-aliasing
    // results depending on the underlying window system. An alternative approach might be to
    // do the rendering on a QImage first. As QImage (at difference to QPixmap and widgets) is
    // independant of native platform rendering, it would garantee identical results on all
    // platforms. But it seems a litte overkill, so we don't do that here. Anyway here the
    // quote from QPainter class documentation:
    //
    // To get the optimal rendering result using QPainter, you should use the platform independent
    // QImage as paint device; i.e. using QImage will ensure that the result has an identical pixel
    // representation on any platform.
    painter.setRenderHint(QPainter::Antialiasing);
    QPoint imageCoordinates = currentImageCoordinates();
    pen.setWidth(m_markerThickness);
    int markerBackgroundLightness = m_color.toLch().L; // range: 0..100
    if (markerBackgroundLightness >= 50) {
        pen.setColor(Qt::black);
    } else {
        pen.setColor(Qt::white);
    }
    painter.setPen(pen);
    painter.drawEllipse(
        imageCoordinates.x() + m_border - m_markerRadius,
        imageCoordinates.y() + m_border - m_markerRadius,
        2 * m_markerRadius + 1,
        2 * m_markerRadius + 1
    );

    // Paint the buffer to the actual widget
    QPainter(this).drawImage(0, 0, paintBuffer);
}

/** @brief Transforms from widget to image coordinates
 * 
 * @param widgetCoordinates a coordinate pair within the widget's coordinate system
 * @returns the corresponding coordinate pair within m_diagramImage's coordinate system
 */
QPoint ChromaLightnessDiagram::fromWidgetCoordinatesToImageCoordinates(const QPoint widgetCoordinates) const
{
    return widgetCoordinates - QPoint(m_border, m_border);
}

/** @brief React on key press events.
 * 
 * Reimplemented from base class.
 * 
 * Reacts on key press events. When the arrow keys are pressed, it moves the marker by one pixel
 * into the desired direction if this is still within gamut. When @c Qt::Key_PageUp,
 * @c Qt::Key_PageDown, @c Qt::Key_Home or @c Qt::Key_End are pressed, it moves the marker as much
 * as possible into the desired direction as long as this is still in the gamut.
 * 
 * @param event the paint event
 * 
 * @warning This function might have an infinite loop if called when the currently selected
 * color has no non-transparent pixel on its row or line. TODO This is a problem because it
 * is well possibe this will arrive because of possible rounding errors!
 * 
 * // TODO Still the darkest color is far from RGB zero on usual widget size. This has to get better to allow choosing RGB 0, 0, 0!!!
 */
void ChromaLightnessDiagram::keyPressEvent(QKeyEvent *event)
{
    // TODO singleStep & pageStep for ALL graphical widgets expressed in LCh
    // values, not in pixel. And the same values accross all widgets!
    
    QPoint newImageCoordinates = currentImageCoordinates();
    updateDiagramCache();
    switch (event->key()) {
        case Qt::Key_Up: 
            if (imageCoordinatesInGamut(newImageCoordinates + QPoint(0, -1))) {
                newImageCoordinates += QPoint(0, -1);
            }
            break;
        case Qt::Key_Down:
            if (imageCoordinatesInGamut(newImageCoordinates + QPoint(0, 1))) {
                newImageCoordinates += QPoint(0, 1);
            }
            break;
        case Qt::Key_Left:
            if (imageCoordinatesInGamut(newImageCoordinates + QPoint(-1, 0))) {
                newImageCoordinates += QPoint(-1, 0);
            }
            break;
        case Qt::Key_Right:
            if (imageCoordinatesInGamut(newImageCoordinates + QPoint(1, 0))) {
                newImageCoordinates += QPoint(1, 0);
            }
            break;
        case Qt::Key_PageUp:
            newImageCoordinates.setY(0);
            while (!imageCoordinatesInGamut(newImageCoordinates + QPoint(0, 1))) {
                newImageCoordinates += QPoint(0, 1);
            }
            break;
        case Qt::Key_PageDown:
            newImageCoordinates.setY(m_diagramImage.height() - 1);
            while (!imageCoordinatesInGamut(newImageCoordinates + QPoint(0, -1))) {
                newImageCoordinates += QPoint(0, -1);
            }
            break;
        case Qt::Key_Home:
            newImageCoordinates.setX(0);
            while (!imageCoordinatesInGamut(newImageCoordinates + QPoint(1, 0))) {
                newImageCoordinates += QPoint(1, 0);
            }
            break;
        case Qt::Key_End:
            newImageCoordinates.setX(m_diagramImage.width() - 1);
            while (!imageCoordinatesInGamut(newImageCoordinates + QPoint(-1, 0))) {
                newImageCoordinates += QPoint(-1, 0);
            }
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
    // Set the new image coordinates (only takes effect when image coordinates are indeed different)
    setImageCoordinates(newImageCoordinates);
}

/**
 * @param imageCoordinates the image coordiantes
 * @returns the chroma-lightness value for given image coordinates
 */
QPointF ChromaLightnessDiagram::fromImageCoordinatesToChromaLightness(const QPoint imageCoordinates)
{
    updateDiagramCache();
    return QPointF(
        static_cast<qreal>(imageCoordinates.x()) * 100 / (m_diagramImage.height() - 1),
        static_cast<qreal>(imageCoordinates.y()) * 100 / (m_diagramImage.height() - 1) * (-1) + 100
    );
}

/**
 * @returns the coordinates for m_color
 */
QPoint ChromaLightnessDiagram::currentImageCoordinates()
{
    updateDiagramCache();
    return QPoint(
        qRound(m_color.toLch().C * (m_diagramImage.height() - 1) / 100),
        qRound(m_color.toLch().L * (m_diagramImage.height() - 1) / 100 * (-1) + (m_diagramImage.height() - 1))
    );
}

/** @brief Tests if image coordinates are in gamut.
 *  @returns @c true if the image coordinates are within the displayed gamut. Otherwise @c false.
 */
bool ChromaLightnessDiagram::imageCoordinatesInGamut(const QPoint imageCoordinates)
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

qreal ChromaLightnessDiagram::hue() const
{
    return m_color.toLch().h;
}

/** @brief Set the hue
 * 
 * convenience function
 * 
 *  @param newHue The new hue value to set.
 */
void ChromaLightnessDiagram::setHue(const qreal newHue)
{
    if (newHue == m_color.toLch().h) {
        return;
    }
    cmsCIELCh lch;
    lch.h = newHue;
    lch.C = m_color.toLch().C;
    lch.L = m_color.toLch().L;
    setColor(
        FullColorDescription(
            m_rgbColorSpace,
            lch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
    );
}

/** @brief Setter for the color() property */
void ChromaLightnessDiagram::setColor(const FullColorDescription &newColor)
{
    if (newColor == m_color) {
        return;
    }

    FullColorDescription oldColor = m_color;
    m_color = newColor;

    // update if necessary, the diagram
    if (m_color.toLch().h != oldColor.toLch().h) {
        m_diagramCacheReady = false;;
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
void ChromaLightnessDiagram::resizeEvent(QResizeEvent* event)
{
    m_diagramCacheReady = false;
    // As by Qt documentation: The widget will be erased and receive a paint event immediately after processing the resize event. No drawing need be (or should be) done inside this handler.
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
QSize ChromaLightnessDiagram::sizeHint() const
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
QSize ChromaLightnessDiagram::minimumSizeHint() const
{
    return QSize(100, 100);
}

// TODO rework all "throw" statements (also these in comments) and the qDebug() statements

// TODO what to do if a gamut allows lightness < 0 or lightness > 100 ???

// TODO what if a part of the gammut at the right is not displayed?

// TODO allow imaginary colors?

int ChromaLightnessDiagram::markerRadius() const
{
    return m_markerRadius;
}

/** @brief Setter for the markerRadius() property.
 * 
 * @param newMarkerRadius the new marker radius
 */
void ChromaLightnessDiagram::setMarkerRadius(const int newMarkerRadius)
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
void ChromaLightnessDiagram::resetMarkerRadius()
{
    setMarkerRadius(default_markerRadius);
}

int ChromaLightnessDiagram::markerThickness() const
{
    return m_markerThickness;
}

/** @brief Setter for the markerThickness() property.
 * 
 * @param newMarkerThickness the new marker thickness
 */
void ChromaLightnessDiagram::setMarkerThickness(const int newMarkerThickness)
{
    int temp = qMax(newMarkerThickness, 0);
    if (m_markerThickness != temp) {
        m_markerThickness = temp;
        updateBorder();
        m_diagramCacheReady = false; // because the border has changed, so the size of the pixmap will change.
        update();
    }
}

/** @brief Reset the markerThickness() property. */
void ChromaLightnessDiagram::resetMarkerThickness()
{
    setMarkerThickness(default_markerThickness);
}

int ChromaLightnessDiagram::border() const
{
    return m_border;
}

/** @brief Color of the current selection
 * 
 * @returns color of the current selection
 */
FullColorDescription ChromaLightnessDiagram::color() const
{
    return m_color;
}

/** @brief Generates an image of a chroma-lightness diagram.
 * 
 * This function generates images of chroma-lightness diagrams in the Lch color space.
 * This function should be thread-save as long as you do not use the same LittleCMS
 * transform from different threads. (Also, out of the Qt library, it uses only QImage,
 * and not QPixmap, to make sure the result can be passed around between threads.)
 * 
 * @param imageHue the (Lch) hue of the image
 * @param imageSize the size of the requested image
 * @param colorTransform the LittleCMS color transform to use. The input profile must be based on
 * cmsCreateLab4ProfileTHR() or cmsCreateLab4Profile(). The output profile must be an RGB
 * profile.
 * @returns A chroma-lightness diagram for the given hue. For the y axis, its heigth covers
 * the lightness range 0..100. [Pixel (0) corresponds to value 100. Pixel (height-1) corresponds
 * to value 0.] Its x axis uses always the same scale as the y axis. So if the size
 * is a square, both @c x range and @c y range are from @c 0 to @c 100. If the 
 * width is larger than the height, the @c x range goes beyond @c 100. The image paints all
 * the Lch values that are within the gamut of the RGB profile. All other values are
 * Qt::transparent. Intentionally there is no anti-aliasing.
 * 
 * @todo Would anti-aliasing be possible? As there is no mathematical
 * description of the shape of the color solid, the only way to get
 * anti-aliasing would be to render at a higher resolution (say two
 * times higher, which would yet mean four times more data), and then
 * downscale it to the final resolution. Would this be an absolute
 * performance killer? Even if not: Is it really worth the performance
 * loss?
 */
QImage ChromaLightnessDiagram::diagramImage(
        const qreal imageHue,
        const QSize imageSize) const
{
    cmsCIELCh LCh; // uses cmsFloat64Number internally
    QColor rgbColor;
    int x;
    int y;
    QImage temp_image = QImage(imageSize, QImage::Format_ARGB32);
    const int maxHeight = imageSize.height() - 1;
    const int maxWidth = imageSize.width() - 1;
    
    // Test if image size is too small.
    if ((maxHeight < 1) || (maxWidth < 1)) {
        // TODO How to react correctly here? Exception?
        // maxHeight and maxWidth must be at least >= 1 for our algorithm. If they are 0, this would crash (division by 0).
        return temp_image;
    }

    // Initialize the image with transparency.
    temp_image.fill(Qt::transparent);

    // Paint the gamut.
    LCh.h = PolarPointF::normalizedAngleDegree(imageHue);
    for (y = 0; y <= maxHeight; ++y) {
        LCh.L = y * static_cast<cmsFloat64Number>(100) / maxHeight; // floating point division thanks to 100 which is a "cmsFloat64Number"
        for (x = 0; x <= maxWidth; ++x) {
            // Using the same scale as on the y axis. floating point
            // division thanks to 100 which is a "cmsFloat64Number"
            LCh.C = x * static_cast<cmsFloat64Number>(100) / maxHeight;
            rgbColor = m_rgbColorSpace->colorRgb(LCh);
            if (rgbColor.isValid()) {
                // The pixel is within the gamut
                temp_image.setPixelColor(
                    x,
                    maxHeight - y,
                    rgbColor
                );
                /* If color is out-of-gamut: We have chroma on the x axis and
                * lightness on the y axis. We are drawing the pixmap line per
                * line, so we go for given lightness from low chroma to high
                * chroma. Because of the nature of most gamuts, if once in a
                * line we have an out-of-gamut value, all other pixels that
                * are more at the right will be out-of-gamut also. So we
                * could optimize our code and break here. But as we are not
                * sure about this (we do not know the gamut at compile time)
                * for the moment we do not optimize the code.
                */
            }
        }
    }

    return temp_image;
}

/** @brief Refresh the diagram and associated data
 * 
 * This class has a cache of various data related to the diagram
 * - @ref m_diagramImage
 * - @ref m_diagramPixmap
 * - @ref m_maxY
 * - @ref m_minY
 * 
 * This data is cached because it is often needed and it would be expensive to calculate it
 * again and again on the fly.
 * 
 * Calling this function updates this cached data. This is always necessary if the data
 * the diagram relies on, has changed. For example, if the hue() property or the widget size
 * have changed, this function has to be called.
 * 
 * This function does not repaint the widget! After calling this function, you have to call
 * manually @c update() to schedule a re-paint of the widget, if you wish so.
 */
void ChromaLightnessDiagram::updateDiagramCache()
{
    if (m_diagramCacheReady) {
        return;
    }

    // Update QImage
    m_diagramImage = diagramImage(
        m_color.toLch().h,
        QSize(size().width() - 2 * m_border, size().height() - 2 * m_border)
    );

    // Mark cache as ready
    m_diagramCacheReady = true;
}

}
