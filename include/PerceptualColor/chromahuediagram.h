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

#ifndef CHROMAHUEDIAGRAM_H
#define CHROMAHUEDIAGRAM_H

#include <QImage>
#include <QPointer>

#include "PerceptualColor/abstractcirculardiagram.h"
#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief A widget for selecting chroma and hue in LCh color space
 * 
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radial and the angle of the
 * LCh color space respectively the a axis and the b axis of the
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * Lab color model</a>) at a given lightness.
 * 
 * The widget allows to select a color within the specified gamut at the
 * given lightness. It reacts on mouse events and on keyboard events (see
 * @ref keyPressEvent() for details).
 * 
 * The circle form of the widget and the selection marker that always
 * indicates the distance from the center of the diagram, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radial
 * and angle.
 * 
 * @note This widget <em>always</em> accepts focus by a mouse click within
 * the circle. This happens regardless of the <tt>QWidget::focusPolicy</tt>
 * property:
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that does not accept focus by mouse click, the focus
 *   will nevertheless be accepted for clicks within the actual circle.
 *   (This is the default behavior.)
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that accepts focus by mouse click, the focus will not only be
 *   accepted for clicks within the actual circle, but also for clicks
 *   within the surrounding rectangle.
 * 
 * @warning The image is wrong: The hue indicator is not at the correct
 * position (at least at 150% screen scaling).
 * 
 * @todo Declare a property for @ref color()
 * 
 * @todo Example code: How to create the widget at a given
 * lightness.
 * 
 * @todo The image cache should be updated asynchronously (in its own thread
 * or even various own threads in parallel), but this should only be
 * triggered when the widget is visible, and not while the widget is
 * hidden. While waiting for the result, an empty image could be used.
 * It might be useful to provide first a low-resolution version, and only
 * later-on a high-resolution version.
 * 
 * @todo High-dpi support.
 * 
 * @todo Automatically scale the thickness of the wheel (and maybe even the
 * marker) with varying widget size?
 * 
 * @todo Support additional mouse buttons. For example, “forward” and
 * “backward” could be used to increase or decrease the radial, while
 * the angel is controlled by the mouse wheel.
 *         
 * @todo Provide RESET functions for all properties around the library? */
class ChromaHueDiagram : public AbstractCircularDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     * 
     * The widget allows the user to change the LCh chroma and the LCh hue
     * values. However, the LCh lightness value cannot be changed by the
     * user, but only by the programmer through this property.
     * 
     * @sa READ @ref color()
     * @sa WRITE @ref setColor()
     * @sa NOTIFY @ref colorChanged() */
    Q_PROPERTY(FullColorDescription color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(
        PerceptualColor::RgbColorSpace *colorSpace,
        QWidget *parent = nullptr
    );
    /** @brief Destructor */
    virtual ~ChromaHueDiagram() override = default;
    /** @brief Getter for property @ref color
     *  @returns the property @ref color */
    FullColorDescription color() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setColor(const PerceptualColor::FullColorDescription &color);

Q_SIGNALS:
    /** @brief Notify signal for property @ref color().
     *  @param color the new color */
    void colorChanged(const PerceptualColor::FullColorDescription &color);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:

    Q_DISABLE_COPY(ChromaHueDiagram)
    
    /** @brief Only for unit tests. */
    friend class TestChromaHueDiagram;

    // Member variables
    /** @brief The border around the round diagram.
     * 
     * Measured in Device Independent Pixels: Pixels used by application
     * (user space), subject to scaling by the operating system or Qt. 
     * 
     * The border is the space where the surrounding color wheel and, where
     * applicable, the focus indicator is painted.
     * @sa @ref markerThickness */
    static constexpr qreal diagramBorder = 8 * markerThickness;
    /** @brief Internal storage of the @ref color() property */
    FullColorDescription m_color;
    /** Holds whether or not @ref m_diagramImage() is up-to-date.
     *  @sa @ref updateDiagramCache() */
    bool m_diagramCacheReady = false;
    /** @brief A cache for the diagram as QImage. Might be outdated.
     *  @sa @ref updateDiagramCache()
     *  @sa @ref m_diagramCacheReady */
    QImage m_diagramImage;
    /** @brief Position of the center of the diagram coordinate system
     * 
     * This value is measured in widget coordinates. */
    qreal m_diagramOffset = 0;
    /** @brief Diameter of the widget.
     * 
     * This is different from <tt>size()</tt>. It is the maximum possible
     * diameter that is available within the current <tt>size()</tt>. */
    int m_widgetDiameter = 0;
    /** @todo This should not be hard-coded sRGB. */
    qreal m_maxChroma = Helper::LchDefaults::maxSrgbChroma;
    /** @brief Holds if currently a mouse event is active or not.
     * 
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a @ref mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref mouseMoveEvent() will move the diagram’s color
     *   marker.
     * - Once a @ref mouseReleaseEvent() occurs, the value is set to
     *   <tt>false</tt>. Further mouse movements will not move the marker
     *   anymore. */
    bool m_isMouseEventActive = false;
    /** @brief Pointer to @ref RgbColorSpace object */
    QPointer<RgbColorSpace> m_rgbColorSpace;
    /** Holds whether or not @ref m_wheelImage() is up-to-date.
     *  @sa @ref updateWheelCache() */
    bool m_isWheelCacheReady = false;
    /** @brief A cache for the wheel as QImage. Might be outdated.
     *  @sa @ref updateWheelCache()
     *  @sa @ref m_isWheelCacheReady */
    QImage m_wheelImage;

    // Member functions
    bool areImageCoordinatesWithinDiagramSurface(
        const QPoint imageCoordinates
    );
    QPoint imageCoordinatesFromColor();
    QPointF fromImageCoordinatesToAB(const QPoint imageCoordinates);
    static QImage generateDiagramImage(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const qreal border,
        const QColor backgroundColor
    );
    static QImage generateDiagramImage2(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    static QImage generateDiagramImage3(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    void setColorFromImageCoordinates(const QPoint imageCoordinates);
    void updateDiagramCache();
    void updateWheelCache();

};

}

#endif // CHROMAHUEDIAGRAM_H
