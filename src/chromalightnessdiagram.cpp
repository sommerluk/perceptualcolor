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
#include "polarpointf.h"
#include "rgbcolorspace.h"

#include <math.h>

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

#include <lcms2.h>

namespace PerceptualColor
{
/** @brief The constructor.
 *
 * @param colorSpace The color space within which the widget should operate
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * @param parent Passed to the QWidget base class constructor */
ChromaLightnessDiagram::ChromaLightnessDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ChromaLightnessDiagramPrivate(this))
{
    // Setup the color space must be the first thing to do because
    // other operations rely on a working color space.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Initialization
    d_pointer->m_currentColor = LchValues::srgbVersatileInitialColor;
    // Accept focus only by keyboard tabbing and not by mouse click
    // Focus by mouse click is handled manually by mousePressEvent().
    setFocusPolicy(Qt::FocusPolicy::TabFocus);
    // Define the size policy of this widget.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/** @brief Default destructor */
ChromaLightnessDiagram::~ChromaLightnessDiagram() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink)
    : q_pointer(backLink)
{
}

// TODO xxx Review starts here

// TODO Get rid of m_diagramCacheReady and m_diagramImage (who’s declaration
// including the documentation is still not reviewd) and of
// updateDiagramCache(). Use a cached image class instead, like for the
// other widgets.

// TODO Control all usage of m_defaultBorder (here, and also in friend
// classes) and change the code (if necessary) to use (also) the new
// m_leftBorder.

// TODO high-dpi support

// TODO Unit tests that make sure there is no crash also on sizes like
// 0×0, 0×1, 1×0, 1×1, 2×2… (There should be code for this yet in the unit
// test of one of the other widgets.) And do this also for ColorWheel
// and ChromaHueDiagram.

/** Updates @ref currentColor corresponding to the given image coordinates.
 *
 * @param newImageCoordinates A coordinate point within the image’s coordinate
 * system. This does not necessarily need to intersect with the actual
 * displayed diagram or the gamut. It might even be negative or outside the
 * image or even outside widget.
 *
 * @post If the coordinate point is within the gamut, then
 * the corresponding @ref currentColor is set. If the coordinate point
 * is outside the gamut, than a nearest-neighbor-search is done, searching
 * a nearby in-gamut color (hue is preverved, chroma and lightness are
 * adjusted) that is less far from the indicates coordinate point. */
void ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::setCurrentColorFromImageCoordinates(const QPoint newImageCoordinates)
{
    updateDiagramCache();
    QPoint correctedImageCoordinates = nearestNeighborSearch(newImageCoordinates, m_diagramImage);
    QPointF chromaLightness;
    LchDouble lch;
    if (correctedImageCoordinates != currentImageCoordinates()) {
        chromaLightness = fromImageCoordinatesToChromaLightness(correctedImageCoordinates);
        lch.c = chromaLightness.x();
        lch.l = chromaLightness.y();
        lch.h = m_currentColor.h;
        q_pointer->setCurrentColor(lch);
    }
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * Does not differentiate between left, middle and right mouse click.
 * If the mouse is clicked within the <em>displayed</em> gamut, than the
 * handle is placed here and further
 * mouse movements are tracked.
 *
 * @param event The corresponding mouse event
 */
void ChromaLightnessDiagram::mousePressEvent(QMouseEvent *event)
{
    QPoint imageCoordinates = d_pointer->fromWidgetCoordinatesToImageCoordinates(event->pos());
    // TODO In the following “if” condition, also accept out-of-gamut clicks
    // when they are covered by the current handle.
    if (d_pointer->imageCoordinatesInGamut(imageCoordinates)) {
        // Mouse focus is handled manually because so we can accept focus only
        // on mouse clicks within the displayed gamut, while rejecting focus
        // otherwise. In the constructor, therefore Qt::FocusPolicy::TabFocus
        // is specified, so that manual handling of mouse focus is up to
        // this code here.
        // TODO Find another solution that guarantees that setFocusPolicy() API
        // of this class behaves as expected, and not as a dirty hack that
        // accepts mouse focus even when set to Qt::FocusPolicy::TabFocus.
        setFocus(Qt::MouseFocusReason);
        d_pointer->m_isMouseEventActive = true;
        setCursor(Qt::BlankCursor);
        d_pointer->setCurrentColorFromImageCoordinates(imageCoordinates);
    } else {
        // Make sure default behavior like drag-window in KDE’s
        // Breeze widget style works.
        event->ignore();
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * Reacts only on mouse move events if previously there had been a mouse
 * press event within the displayed gamut. If the mouse moves inside the
 * <em>displayed</em> gamut, the handle is displaced there. If the mouse
 * moves outside the <em>displayed</em> gamut, the handle is displaced to
 * the nearest neighbor pixel within gamut.
 *
 * If previously there had not been a mouse press event, the mouse move
 * event is ignored.
 *
 * @param event The corresponding mouse event
 */
void ChromaLightnessDiagram::mouseMoveEvent(QMouseEvent *event)
{
    QPoint imageCoordinates = d_pointer->fromWidgetCoordinatesToImageCoordinates(event->pos());
    if (d_pointer->m_isMouseEventActive) {
        if (d_pointer->imageCoordinatesInGamut(imageCoordinates)) {
            setCursor(Qt::BlankCursor);
        } else {
            unsetCursor();
        }
        d_pointer->setCurrentColorFromImageCoordinates(imageCoordinates);
    } else {
        // Make sure default behavior like drag-window in KDE’s
        // Breeze widget style works.
        event->ignore();
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Does not differentiate between left,
 * middle and right mouse click.
 *
 * If the mouse is inside the <em>displayed</em> gamut, the handle is
 * displaced there. If the mouse is outside the <em>displayed</em> gamut,
 * the handle is displaced to the nearest neighbor pixel within gamut.
 *
 * @param event The corresponding mouse event
 */
void ChromaLightnessDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_pointer->m_isMouseEventActive) {
        d_pointer->setCurrentColorFromImageCoordinates(d_pointer->fromWidgetCoordinatesToImageCoordinates(event->pos()));
        unsetCursor();
        d_pointer->m_isMouseEventActive = false;
    } else {
        // Make sure default behavior like drag-window in KDE’s
        // Breeze widget style works.
        event->ignore();
    }
}

// TODO What when @ref m_color has a valid in-gamut color, but this color
// is out of the _displayed_ diagram? How to handle that?

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * Paints the widget. Takes the existing m_diagramImage and m_diagramPixmap
 * and paints them on the widget. Paints, if appropriate, the focus indicator.
 * Paints the handle. Relies on that m_diagramImage and m_diagramPixmap are
 * up to date.
 *
 * @param event the paint event
 */
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
    QImage paintBuffer(size(), QImage::Format_ARGB32_Premultiplied);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);

    QPen pen;

    // Paint the diagram itself as available in the cache.
    d_pointer->updateDiagramCache();
    painter.drawImage(d_pointer->m_defaultBorder, // x position (top-left)
                      d_pointer->m_defaultBorder, // y position (top-left)
                      d_pointer->m_diagramImage   // image
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
    // paint it at the left-most possible position. As the border() property
    // accommodates also to handleRadius, the distance of the focus line
    // to the real diagram also does, which looks nice.
    // TODO This should be styled correctly. Find a solution with
    // QStyle::PE_Frame. But: Some styles like dlight have invisible frames
    // (all, even raised and sunken).
    if (hasFocus()) {
        pen.setWidth(handleOutlineThickness());
        pen.setColor(focusIndicatorColor());
        painter.setPen(pen);
        painter.drawLine(
            // x1:
            handleOutlineThickness() / 2, // 0.5 is rounded down to 0.0
            // y1:
            0 + d_pointer->m_defaultBorder,
            // x2
            handleOutlineThickness() / 2, // 0.5 is rounded down to 0.0
            // y2:
            size().height() - d_pointer->m_defaultBorder);
    }

    // Paint the handle on-the-fly.
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
    painter.setRenderHint(QPainter::Antialiasing);
    QPoint imageCoordinates = d_pointer->currentImageCoordinates();
    pen.setWidth(handleOutlineThickness());
    pen.setColor(handleColorFromBackgroundLightness(d_pointer->m_currentColor.l));
    painter.setPen(pen);
    painter.drawEllipse(
        // Within this rectangle:
        QRectF(
            // x:
            imageCoordinates.x() + d_pointer->m_defaultBorder - handleRadius(),
            // y:
            imageCoordinates.y() + d_pointer->m_defaultBorder - handleRadius(),
            // width:
            2 * handleRadius() + 1,
            // height:
            2 * handleRadius() + 1));

    // Paint the buffer to the actual widget
    QPainter(this).drawImage(0, 0, paintBuffer);
}

/** @brief Transforms from widget to image coordinates
 *
 * @param widgetCoordinates a coordinate pair within the widget’s coordinate
 * system
 * @returns the corresponding coordinate pair within m_diagramImage’s
 * coordinate system
 */
QPoint ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::fromWidgetCoordinatesToImageCoordinates(const QPoint widgetCoordinates) const
{
    return widgetCoordinates - QPoint(m_defaultBorder, m_defaultBorder);
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * Reacts on key press events. When the arrow keys are pressed, it moves the
 * handle by one pixel into the desired direction if this is still within
 * gamut. When <tt>Qt::Key_PageUp</tt>, <tt>Qt::Key_PageDown</tt>,
 * <tt>Qt::Key_Home</tt> or <tt>Qt::Key_End</tt> are pressed, it moves the
 * handle as much as possible into the desired direction as long as this is
 * still in the gamut.
 *
 * @param event the paint event
 *
 * @internal
 *
 * @todo “it moves the handle by one pixel into the desired direction”. This
 * is bad. Behaviour should not be pixel-density-dependant. Use a fixed value
 * for this!
 *
 * @todo WARNING This function might have an infinite loop if called when the
 * currently selected color has no non-transparent pixel on its row or line.
 * This is a problem because it is well possible this will arrive
 * because of possible rounding errors!
 *
 * @todo Still the darkest color is far from RGB zero on usual widget size.
 * This has to get better to allow choosing RGB(0, 0, 0)! */
void ChromaLightnessDiagram::keyPressEvent(QKeyEvent *event)
{
    // TODO singleStep & pageStep for ALL graphical widgets expressed in LCh
    // values, not in pixel. Use values as inherited from base class!

    QPoint newImageCoordinates = d_pointer->currentImageCoordinates();
    d_pointer->updateDiagramCache();
    switch (event->key()) {
    case Qt::Key_Up:
        if (d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(0, -1))) {
            newImageCoordinates += QPoint(0, -1);
        }
        break;
    case Qt::Key_Down:
        if (d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(0, 1))) {
            newImageCoordinates += QPoint(0, 1);
        }
        break;
    case Qt::Key_Left:
        if (d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(-1, 0))) {
            newImageCoordinates += QPoint(-1, 0);
        }
        break;
    case Qt::Key_Right:
        if (d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(1, 0))) {
            newImageCoordinates += QPoint(1, 0);
        }
        break;
    case Qt::Key_PageUp:
        newImageCoordinates.setY(0);
        while (!d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(0, 1))) {
            newImageCoordinates += QPoint(0, 1);
        }
        break;
    case Qt::Key_PageDown:
        newImageCoordinates.setY(d_pointer->m_diagramImage.height() - 1);
        while (!d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(0, -1))) {
            newImageCoordinates += QPoint(0, -1);
        }
        break;
    case Qt::Key_Home:
        newImageCoordinates.setX(0);
        while (!d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(1, 0))) {
            newImageCoordinates += QPoint(1, 0);
        }
        break;
    case Qt::Key_End:
        newImageCoordinates.setX(d_pointer->m_diagramImage.width() - 1);
        while (!d_pointer->imageCoordinatesInGamut(newImageCoordinates + QPoint(-1, 0))) {
            newImageCoordinates += QPoint(-1, 0);
        }
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
    d_pointer->setCurrentColorFromImageCoordinates(newImageCoordinates);
}

/**
 * @param imageCoordinates the image coordinates
 * @returns the chroma-lightness value for given image coordinates
 *
 * @todo It would be great if this function could be <tt>const</tt>.
 */
QPointF ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::fromImageCoordinatesToChromaLightness(const QPoint imageCoordinates)
{
    updateDiagramCache();
    return QPointF(
        // x:
        static_cast<qreal>(imageCoordinates.x()) * 100 / (m_diagramImage.height() - 1),
        // y:
        static_cast<qreal>(imageCoordinates.y()) * 100 / (m_diagramImage.height() - 1) * (-1) + 100);
}

/**
 * @returns the coordinates for m_color
 *
 * @todo It would be great if this function could be <tt>const</tt>.
 */
QPoint ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::currentImageCoordinates()
{
    updateDiagramCache();
    return QPoint(
        // x:
        qRound(m_currentColor.c * (m_diagramImage.height() - 1) / 100),
        // y:
        qRound(m_currentColor.l * (m_diagramImage.height() - 1) / 100 * (-1) + (m_diagramImage.height() - 1)));
}

/** @brief Tests if image coordinates are in gamut.
 *  @returns <tt>true</tt> if the image coordinates are within the
 *  displayed gamut. Otherwise <tt>false</tt>.
 *
 * @todo It would be great if this function could be <tt>const</tt>.
 */
bool ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::imageCoordinatesInGamut(const QPoint imageCoordinates)
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

/** @brief Setter for the @ref currentColor() property.
 *
 * @param newCurrentColor the new @ref currentColor */
void ChromaLightnessDiagram::setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor)
{
    if (newCurrentColor.hasSameCoordinates(d_pointer->m_currentColor)) {
        return;
    }

    LchDouble oldColor = d_pointer->m_currentColor;
    d_pointer->m_currentColor = newCurrentColor;

    // update if necessary, the diagram
    if (d_pointer->m_currentColor.h != oldColor.h) {
        d_pointer->m_diagramCacheReady = false;
        ;
    }

    // schedule a paint event
    update();
    Q_EMIT currentColorChanged(newCurrentColor);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event
 */
void ChromaLightnessDiagram::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_pointer->m_diagramCacheReady = false;
    // As by Qt documentation:
    //     “The widget will be erased and receive a paint event
    //      immediately after processing the resize event. No drawing
    //      need be (or should be) done inside this handler.”
}

// TODO how to treat empty images because the color profile does not
// work or the resolution is too small?

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
QSize ChromaLightnessDiagram::minimumSizeHint() const
{
    const int minimum = 2 * d_pointer->m_defaultBorder + gradientMinimumLength();
    // Expand to the global minimum size for GUI elements
    return QSize(minimum, minimum).expandedTo(QApplication::globalStrut());
}

// TODO what to do if a gamut allows lightness < 0 or lightness > 100 ???

// TODO what if a part of the gamut at the right is not displayed?

/** @brief Color of the current selection
 *
 * @returns color of the current selection
 */
LchDouble PerceptualColor::ChromaLightnessDiagram::currentColor() const
{
    return d_pointer->m_currentColor;
}

/** @brief Generates an image of a chroma-lightness diagram.
 *
 * (Also, out of the Qt library, it uses only QImage, and not QPixmap,
 * to make sure the result can be passed around between threads.)
 *
 * @param imageHue the (LCh) hue of the image
 * @param imageSize the size of the requested image
 * @returns A chroma-lightness diagram for the given hue. For the y axis,
 * its height covers the lightness range 0..100. [Pixel (0) corresponds
 * to value 100. Pixel (height-1) corresponds to value 0.] Its x axis
 * uses always the same scale as the y axis. So if the size
 * is a square, both x range and y range are from 0 to 100. If the
 * width is larger than the height, the x range goes beyond 100. The
 * image paints all the LCh values that are within the gamut of the
 * RGB profile. All other values are Qt::transparent. Intentionally
 * there is no anti-aliasing.
 *
 * @todo Would anti-aliasing be possible? As there is no mathematical
 * description of the shape of the color solid, the only way to get
 * anti-aliasing would be to render at a higher resolution (say two
 * times higher, which would yet mean four times more data), and then
 * downscale it to the final resolution. Would this be an absolute
 * performance killer? Even if not: Is it really worth the performance
 * loss?
 *
 * @todo Should this function be made static? Or the opposite: Stay a normal
 * function (but maybe even remove all arguments like imageHue and imageSize
 * because we can get them directly from underlying object data)?
 *
 * @todo Possible unit test for this function:

void testChromaLightnessDiagramm() {
    QImage mImage;

    // Testing extremely small images
    mImage = PerceptualColor::ChromaLightnessDiagram::generateDiagramImage(
        0,
        QSize(0, 0)
    );
    QCOMPARE(mImage.size(), QSize(0, 0));
    mImage = PerceptualColor::ChromaLightnessDiagram::generateDiagramImage(
        0,
        QSize(1, 1)
    );
    QCOMPARE(mImage.size(), QSize(1, 1));
    mImage = PerceptualColor::ChromaLightnessDiagram::generateDiagramImage(
        0,
        QSize(2, 2)
    );
    QCOMPARE(mImage.size(), QSize(2, 2));
    mImage = PerceptualColor::ChromaLightnessDiagram::generateDiagramImage(
        0,
        QSize(-1, -1)
    );
    QCOMPARE(mImage.size(), QSize(0, 0));

    // Start testing for a normal size image
    mImage = PerceptualColor::ChromaLightnessDiagram::generateDiagramImage(
        0,
        QSize(201, 101)
    );
    QCOMPARE(mImage.height(), 101);
    QCOMPARE(mImage.width(), 201);
    // Test if position within the QImage is valid:
    QCOMPARE(mImage.pixelColor(0, 0).isValid(), true);
    // Test if position within the QImage is valid:
    QCOMPARE(mImage.pixelColor(0, 100).isValid(), true);
    QTest::ignoreMessage(
        QtWarningMsg,
        "QImage::pixelColor: coordinate (0,101) out of range"
    );
    // Test if position within the QImage is invalid:
    QCOMPARE(mImage.pixelColor(0, 101).isValid(), false);
}
*/
QImage ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::generateDiagramImage(const qreal imageHue, const QSize imageSize) const
{
    LchDouble LCh;
    QColor rgbColor;
    int x;
    int y;
    QImage temp_image = QImage(imageSize, QImage::Format_ARGB32_Premultiplied);
    const int maxHeight = imageSize.height() - 1;
    const int maxWidth = imageSize.width() - 1;

    // Test if image size is too small.
    if ((maxHeight < 1) || (maxWidth < 1)) {
        // TODO How to react correctly here? Exception?
        // maxHeight and maxWidth must be at least >= 1 for our
        // algorithm. If they are 0, this would crash (division by 0).
        return temp_image;
    }

    // Initialize the image with transparency.
    temp_image.fill(Qt::transparent);

    // Paint the gamut.
    LCh.h = PolarPointF::normalizedAngleDegree(imageHue);
    for (y = 0; y <= maxHeight; ++y) {
        LCh.l = y * static_cast<cmsFloat64Number>(100) / maxHeight;
        for (x = 0; x <= maxWidth; ++x) {
            // Using the same scale as on the y axis. floating point
            // division thanks to 100 which is a "cmsFloat64Number"
            LCh.c = x * static_cast<cmsFloat64Number>(100) / maxHeight;
            rgbColor = m_rgbColorSpace->colorRgb(LCh);
            if (rgbColor.isValid()) {
                // The pixel is within the gamut
                temp_image.setPixelColor(x, maxHeight - y, rgbColor);
                // If color is out-of-gamut: We have chroma on the x axis and
                // lightness on the y axis. We are drawing the pixmap line per
                // line, so we go for given lightness from low chroma to high
                // chroma. Because of the nature of most gamuts, if once in a
                // line we have an out-of-gamut value, all other pixels that
                // are more at the right will be out-of-gamut also. So we
                // could optimize our code and break here. But as we are not
                // sure about this (we do not know the gamut at compile time)
                // for the moment we do not optimize the code.
            }
        }
    }

    return temp_image;
}

/** @brief Refresh the diagram and associated data
 *
 * This class has a cache of various data related to the diagram.
 * This data is cached because it is often needed and it would be
 * expensive to calculate it again and again on the fly.
 *
 * Calling this function updates this cached data. This is always
 * necessary if the data the diagram relies on, has changed. For example,
 * if the hue() property or the widget size have changed, this function
 * has to be called.
 *
 * This function does not repaint the widget! After calling this function,
 * you have to call manually <tt>update()</tt> to schedule a re-paint of
 * the widget, if you wish so. */
void ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::updateDiagramCache()
{
    if (m_diagramCacheReady) {
        return;
    }

    // Free the memory used by the old image.
    m_diagramImage = QImage();
    // Generate an update QImage
    m_diagramImage = generateDiagramImage(
        // hue:
        m_currentColor.h,
        // image size:
        QSize(q_pointer->size().width() - 2 * m_defaultBorder, // x
              q_pointer->size().height() - 2 * m_defaultBorder // y
              ));

    // Mark cache as ready
    m_diagramCacheReady = true;
}

/** @brief Search the nearest non-transparent neighbor pixel
 *
 * @note This code is an inefficient implementation
 * of a <em>nearest-neighbor-searchy</em>. See
 * <a href="https://stackoverflow.com/questions/307445/finding-closest-non-black-pixel-in-an-image-fast">here</a>
 * for a better approach.
 *
 * @param originalPoint The point for which you search the nearest neighbor,
 * expressed in the coordinate system of the image. This point may be within
 * or outside the image.
 * @param image The image in which the nearest neighbor is searched.
 * Must contain at least one pixel with an alpha value that is fully opaque.
 * @returns
 * \li If originalPoint itself is within the image and a
 *     non-transparent pixel, it returns originalPoint.
 * \li Else, if there is are non-transparent pixels in the image, the nearest
 *     non-transparent pixel is returned. (If there are various nearest
 *     neighbors at the same distance, it is undefined which one is returned.)
 * \li Else there are no non-transparent pixels, and simply the point
 *     <tt>0, 0</tt> is returned, but this is a very slow case.
 *
 * TODO Do not use nearest neighbor or other pixel based search algorithms,
 * but work directly with LittleCMS, maybe with a limited, but well-defined,
 * precision… */
QPoint ChromaLightnessDiagram::ChromaLightnessDiagramPrivate::nearestNeighborSearch(const QPoint originalPoint, const QImage &image)
{
    // Test for special case:
    // originalPoint itself is within the image and non-transparent
    if (image.valid(originalPoint)) {
        if (image.pixelColor(originalPoint).alpha() == 255) {
            return originalPoint;
        }
    }

    // No special case. So we have to actually perform
    // a nearest-neighbor-search.
    int x;
    int y;
    int currentBestX = 0; // 0 is the fallback value
    int currentBestY = 0; // 0 is the fallback value
    int currentBestDistanceSquare = std::numeric_limits<int>::max();
    int x_distance;
    int y_distance;
    int temp;
    for (x = 0; x < image.width(); x++) {
        for (y = 0; y < image.height(); y++) {
            if (image.pixelColor(x, y).alpha() == 255) {
                x_distance = originalPoint.x() - x;
                y_distance = originalPoint.y() - y;
                temp = x_distance * x_distance + y_distance * y_distance;
                if (temp < currentBestDistanceSquare) {
                    currentBestX = x;
                    currentBestY = y;
                    currentBestDistanceSquare = temp;
                }
            }
        }
    }
    return QPoint(currentBestX, currentBestY);
}

} // namespace PerceptualColor
