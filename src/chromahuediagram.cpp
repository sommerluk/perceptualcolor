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
 * @param parent The widget’s parent widget. This parameter will be passed
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
 * - If the mouse is clicked with the circular diagram (inside or outside
 *   of the visible gamut), than this widget gets the focus and and
 *   @ref m_isMouseEventActive is set to <tt>true</tt> to track mouse
 *   movements from now on. Reacts on all clicks (left, middle, right). If the
 *   mouse was within the gamut, the diagram’s marker is displaced there. If
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
    if (areImageCoordinatesWithinDiagramSurface(event->pos())) {
        event->accept();
        // Mouse focus is handled manually because so we can accept
        // focus only on mouse clicks within the displayed gamut, while
        // rejecting focus otherwise. In the constructor, therefore
        // Qt::FocusPolicy::TabFocus is specified, so that manual handling
        // of mouse focus is up to this code here.
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
 * Reacts only on mouse move events @ref m_isMouseEventActive is <tt>true</tt>.
 * - If the mouse moves within the gamut, the diagram’s marker is displaced
 *   there. The mouse cursor is invisible; only the diagram’ marker is visible.
 * - If the mouse moves outside the gamut, the diagram’s marker always stays
 *   within the gamut: The chroma value is scarified while the hue value is
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
        event->accept();
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
        // Make sure default behavior like drag-window in KDE’s
        // Breeze widget style works.
        event->ignore();
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
 *   within the gamut: The chroma value is scarified while the hue value is
 *   retained.
 * - The mouse cursor is made visible (if he wasn’t yet visible anyway).
 * - @ref m_isMouseEventActive is set to <tt>false</tt>.
 * 
 * If @ref m_isMouseEventActive is <tt>false</tt>, it simply falls back to
 * QWidget’s default implementation.
 * 
 * @todo What if the widget displays a gamut that has no L*=0.1 because its
 * blackpoint is lighter.? Scarifying chroma alone does not help? How to
 * react (for mouse input, keyboard input, but also API functions like
 * setColor()?
 * 
 * @param event The corresponding mouse event */
void ChromaHueDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isMouseEventActive) {
        event->accept();
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
        (!m_isMouseEventActive) &&
        // Only react on good old vertical wheels,
        // and not on horizontal wheels.
        (event->angleDelta().y() != 0) &&
        // Only react on wheel events when then happen in the appropriate
        // area.a
        areImageCoordinatesWithinDiagramSurface(event->pos())
    ) {
        event->accept();
        cmsCIELCh lch = m_color.toLch();
        // Calculate the new hue.
        // This may result in a hue smaller then 0° or bigger then 360°.
        // This is no problem because the constructor of FullColorDescription
        // will normalize the hue.
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
            m_rgbColorSpace,
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
    return m_color;
}

/** @brief Setter for the @ref color() property.
 * 
 * @param color the new color */
void ChromaHueDiagram::setColor(const FullColorDescription &color)
{
    if (color == m_color) {
        return;
    }

    FullColorDescription oldColor = m_color;

    m_color = color;

    // Update, if necessary, the diagram.
    if (m_color.toLch().L != oldColor.toLch().L) {
        m_diagramCacheReady = false;
    }

    // Schedule a paint event:
    update();

    // Emit notify signal
    Q_EMIT colorChanged(color);
}

// TODO xxx Revision starting here


/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 * 
 * @param event The corresponding resize event
 */
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
    if (newWidgetDiameter != m_widgetDiameter) {
        m_widgetDiameter = newWidgetDiameter;
        m_diagramOffset = (m_widgetDiameter - 1) / static_cast<qreal>(2);
        m_diagramCacheReady = false;
        m_isWheelCacheReady = false;
        // As by Qt documentation: The widget will be erased and receive a
        // paint event immediately after processing the resize event. No
        // drawing need be (or should be) done inside this handler.
    }
}

// TODO xxx What happens when maxChroma is low and parts of the gamut are out-of-display?

/**
 * @param imageCoordinates the image coordinates
 * @returns the diagram (a-b) value for given image coordinates
 */
QPointF ChromaHueDiagram::fromImageCoordinatesToAB(
    const QPoint imageCoordinates
)
{
    const qreal scaleFactor =
        static_cast<qreal>(2 * m_maxChroma) / (m_widgetDiameter - 2 * diagramBorder);
    return QPointF(
        (imageCoordinates.x() - m_diagramOffset) * scaleFactor,
        (imageCoordinates.y() - m_diagramOffset) * scaleFactor * (-1)
    );
}

/**
 * @returns the logical coordinates that correspond to @ref m_color()
 */
QPoint ChromaHueDiagram::imageCoordinatesFromColor()
{
    const qreal scaleFactor =
        (m_widgetDiameter - 2 * diagramBorder) / static_cast<qreal>(2 * m_maxChroma);
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
bool ChromaHueDiagram::areImageCoordinatesWithinDiagramSurface(
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

// TODO how to treat empty images because the color profile does not work or the resolution is too small?

// TODO what to do if a gamut allows lightness < 0 or lightness > 100 ???

/** @brief Beta implementation based on LittleCMS Gamut Boundary Description.
 * 
 * LittleCMS out-of-gamut marking might fail on some profiles! (Without
 * warning…?) So what could be an alternative?
 * 
 * This function implements based on Gamut Boundary Descriptions.
 */

/** @brief in image of a-b plane of the color space at a given lightness */
// TODO How to make sure the diagram has at least a few pixels?
QImage ChromaHueDiagram::generateDiagramImage3(
    const RgbColorSpace *colorSpace,
    const int imageSize,
    const qreal maxChroma,
    const qreal lightness,
    const int border
)
{
    cmsContext ContextID = cmsCreateContext(nullptr, nullptr);
    cmsHANDLE h_gamutBoundaryDescriptor = cmsGBDAlloc(ContextID);
    qreal h;
    qreal s;
    qreal v;
    constexpr qreal step = 1;
    cmsCIELab lab; //     cmsCIELab lab;
    for (h = 0; h < 359; h += step) {
        s = 255;
        for (v = 1; v <= 254; v += step) {
            lab = colorSpace->colorLab(QColor::fromHsvF(h, s, v));
            cmsGDBAddPoint(h_gamutBoundaryDescriptor, &lab);
        }
        v = 255;
        for (s = 1; s <= 254; s += step) {
            lab = colorSpace->colorLab(QColor::fromHsvF(h, s, v));
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
        QImage::Format_ARGB32_Premultiplied
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
                    // The  is within the gamut
                    tempImage.setPixelColor(x, y, tempColor);
                }
            }
        }
    }
    qDebug() << "Generating chroma-hue gamut image took" << myTimer.restart() << "ms.";  // TODO remove me!

    QImage result = QImage(
        QSize(imageSize, imageSize),
        QImage::Format_ARGB32_Premultiplied
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
    const qreal border,
    const QColor backgroundColor
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
        QImage::Format_ARGB32_Premultiplied
    );
    tempImage.fill(backgroundColor); // Initialize the image background
    const qreal scaleFactor =
        static_cast<qreal>(2 * maxChroma) / (imageSize - 2 * border);
 
    QElapsedTimer myTimer; // TODO remove me!
    myTimer.start(); // TODO remove me!
    // Paint the gamut.
    lab.L = lightness;
    for (y = 0; y <= maxIndex; ++y) {
        lab.b = maxChroma - (y - border) * scaleFactor;
        for (x = 0; x <= maxIndex; ++x) {
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
        QImage::Format_ARGB32_Premultiplied
    );
    result.fill(Qt::transparent);
    // Cut of everything outside the circle
    QPainter myPainter(&result);
    myPainter.setRenderHint(QPainter::Antialiasing, true);
    myPainter.setPen(QPen(Qt::NoPen));
    myPainter.setBrush(QBrush(tempImage));
    myPainter.drawEllipse(
        QPointF(
            static_cast<qreal>(imageSize) / 2,
            static_cast<qreal>(imageSize) / 2
        ),                                      // center
        (imageSize - 2 * border) / 2,           // width
        (imageSize - 2 * border) / 2            // height
    );

    return result;
}


/** @brief Refresh the diagram and associated data
 * 
 * This class has a cache of for the diagram: @ref m_diagramImage. This data
 * is cached because it is often needed and it would be expensive to calculate
 * it again and again on the fly.
 * 
 * Calling this function updates this cached data. This is always necessary
 * when the data has changed, that the diagram relies on. Call this function
 * always before using the cached data!
 * 
 * This function does not repaint the widget! After calling this function,
 * you have to call manually <tt>update()</tt> to schedule a re-paint of the
 * widget, if you wish so. */
void ChromaHueDiagram::updateDiagramCache()
{
    if (m_diagramCacheReady) {
        return;
    }

    // Update QImage
    // Free memory of the old image before allocating
    // new memory for the new image
    m_diagramImage = QImage();
    // Get the new image
    constexpr cmsCIELCh backgroundColor = Helper::LchDefaults::neutralGray;
    m_diagramImage = generateDiagramImage(
        m_rgbColorSpace,
        physicalPixelWidgetDiameter(),
        m_maxChroma,
        m_color.toLch().L,
        diagramBorder * devicePixelRatioF(),
        m_rgbColorSpace->colorRgbBound(backgroundColor)
    );
    m_diagramImage.setDevicePixelRatio(devicePixelRatioF());

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
    // Free memory of the old image before allocating
    // new memory for the new image
    m_wheelImage = QImage();
    m_wheelImage = SimpleColorWheel::generateWheelImage(
        m_rgbColorSpace,
        physicalPixelWidgetDiameter(),                                 // diameter
        2 * markerThickness * devicePixelRatioF(),                      // border
        4 * markerThickness * devicePixelRatioF(),                      // thickness
        Helper::LchDefaults::defaultLightness,      // lightness
        Helper::LchDefaults::versatileSrgbChroma    // chroma
    );
    m_wheelImage.setDevicePixelRatio(devicePixelRatioF());

    // Mark cache as ready
    m_isWheelCacheReady = true;
}


/** @brief Paint the widget.
 * 
 * Reimplemented from base class.
 *
 * - Paints the widget. Takes the existing @ref m_diagramImage and
 *   @ref m_wheelImage and paints them on the widget.
 * - Paints the markers.
 * - If the widget has focus, it also paints the focus indicator.
 * 
 * @param event the paint event
 * 
 * @todo What when @ref m_color has a valid in-gamut color, but this color is
 * out of the <em>displayed</em> diagram? How to handle that?
 * 
 * @todo Better design on small widget sizes for the whole library. */
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
    // (at difference to QPixmap and a QWidget) is independent of native
    // platform rendering, it guarantees identical anti-aliasing results on
    // all platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should
    //       use the platform independent QImage as paint device; i.e. using
    //       QImage will ensure that the result has an identical pixel
    //       representation on any platform.”
    QImage paintBuffer(
        physicalPixelWidgetDiameter(),
        physicalPixelWidgetDiameter(),
        QImage::Format_ARGB32_Premultiplied
    );
    paintBuffer.fill(Qt::transparent);
    paintBuffer.fill(Qt::white); // TODO Remove this line!
    paintBuffer.setDevicePixelRatio(devicePixelRatioF());

    
    // Other initialization
    QPainter painter(&paintBuffer);
    QPen pen;
    QBrush brush;

    // Paint the gamut itself as available in the cache.
    updateDiagramCache();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(
        QPoint(0, 0),   // position of the image
        m_diagramImage  // image
    );

    // Paint a color wheel around
    updateWheelCache();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(
        QPoint(0, 0),   // position of the image
        m_wheelImage    // image
    );

    // Paint a marker on the color wheel
    // only if a mouse event is currently active.
    if (m_isMouseEventActive) {
        // TODO Apparently the position is not exact.
        qreal radius =
            m_widgetDiameter / static_cast<qreal>(2) - 2 * markerThickness;
        // Get widget coordinates for our marker
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
     * style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
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
            QPointF(m_diagramOffset + 1, m_diagramOffset + 1),   // center
            (m_widgetDiameter - markerThickness) / 2,   // x radius
            (m_widgetDiameter - markerThickness) / 2    // y radius
        );
    }

    // Paint the marker on-the-fly.
    painter.setRenderHint(QPainter::Antialiasing, true);
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
    painter.drawEllipse( // TODO Switch to floating point!
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
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(
        QPainter::Antialiasing,
        false
    );
    widgetPainter.drawImage(
        QPoint(0, 0),
        paintBuffer
    );
    qDebug() // TODO Remove me!
        << size()
        << "×"
        << devicePixelRatioF()
        << "="
        << size().width() * devicePixelRatioF()
        << ","
        << size().height() * devicePixelRatioF();
}

}
