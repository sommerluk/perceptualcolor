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
#include <QWidget>

#include <lcms2.h>

#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {
    
/** @brief A widget that displays the plan of chroma and hue
 * 
 * A widget that displays the plan of chroma and hue, that means an a*-b*
 * diagram (the a* axis and the b* axis of the L*a*b* color model).
 *
 * The lightness (L* axis in Lab/LCh color model) can be controled by a
 * property.
 * 
 * The widget reacts on mouse events and on keyboard events (see
 * keyPressEvent() for details).
 * 
 * @todo When the mouse is within the circle, but above the gray background
 * and outside the gamut, then the widget should continue to accept clicks,
 * but it should now also show the mouse curser. */
class ChromaHueDiagram : public QWidget
{
    Q_OBJECT

    /** @brief The border between the widget outer border and the diagram itself.
     * 
     * The diagram is not painted on the whole extend of the widget. A border is
     * left to allow that the selection marker can be painted completly even when
     * a pixel on the border of the diagram is selected. The border is
     * determined automatically, its value depends on markerRadius() and
     * markerThickness().
     * 
     * @sa border()
     * @sa updateBorder()
     */
    Q_PROPERTY(int border READ border STORED false)

    /** @brief the radius of the marker
     * 
     * @sa markerRadius()
     * @sa setMarkerRadius()
     * @sa resetMarkerRadius()
     * @sa default_markerRadius
     */
    Q_PROPERTY(int markerRadius READ markerRadius WRITE setMarkerRadius RESET resetMarkerRadius)

    /** @brief the thickness of the marker
     * 
     * @sa markerThickness()
     * @sa setMarkerThickness()
     * @sa resetMarkerThickness()
     * @sa default_markerThickness
     */
    Q_PROPERTY(int markerThickness READ markerThickness WRITE setMarkerThickness RESET resetMarkerThickness)

    /** @brief Currently selected color
     * 
     * @sa color()
     * @sa setColor()
     * @sa colorChanged()
     */
    Q_PROPERTY(FullColorDescription color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    explicit ChromaHueDiagram(RgbColorSpace *colorSpace, QWidget *parent = nullptr);
    virtual ~ChromaHueDiagram() override = default;
    int border() const;
    FullColorDescription color() const;
    qreal lightness() const;
    int markerRadius() const;
    int markerThickness() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void resetMarkerRadius();
    void resetMarkerThickness();
    void setColor(const PerceptualColor::FullColorDescription &newColor);
    void setLightness(const qreal newLightness);
    void setMarkerRadius(const int newMarkerRadius);
    void setMarkerThickness(const int newMarkerThickness);

Q_SIGNALS:
    /** @brief Signal for color() property. */
    void colorChanged(const PerceptualColor::FullColorDescription &newColor);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

private:

    Q_DISABLE_COPY(ChromaHueDiagram)

    /** @brief Default value for markerRadius() property. */
    static constexpr int default_markerRadius = 4;
    /** @brief Default value for markerThickness() property. */
    static constexpr int default_markerThickness = 2;

    /** @brief Internal storage of the border() property */
    int m_border;
    /** @brief Internal storage of the chromaLightness() property */
//     QPointF m_chromaLightness;
    /** @brief Internal storage of the color() property */
    FullColorDescription m_color;
    /** @brief A cache for the diagram as QImage. Might be outdated.
     *  @sa updateDiagramCache()
     *  @sa m_diagramCacheReady */
    QImage m_diagramImage;
    /** Holds wether or not m_diagramImage() is up-to-date.
     *  @sa updateDiagramCache() */
    bool m_diagramCacheReady = false;
    /** @brief A cache for the wheel as QImage. Might be outdated.
     *  @sa updateWheelCache()
     *  @sa m_wheelCacheReady */
    QImage m_wheelImage;
    /** Holds wether or not m_wheelImage() is up-to-date.
     *  @sa updateWheelCache() */
    bool m_wheelCacheReady = false;
    /** @brief Internal storage of the markerRadius() property */
    int m_markerRadius;
    /** @brief Internal storage of the markerThickness() property */
    int m_markerThickness;
    /** @brief If a mouse event is active
     * 
     * Holds if currently a mouse event is active or not.
     * @sa mousePressEvent()
     * @sa mouseMoveEvent()
     * @sa mouseReleaseEvent()
     */
    bool m_mouseEventActive = false;
    /** @brief Pointer to RgbColorSpace() object */
    RgbColorSpace *m_rgbColorSpace;
    /** @brief diameter of the diagram */
    int m_diameter = 0;
    /** position (measured in widget coordinates) of the center of the diagram coordinate system */
    int m_diagramOffset = 0;
    qreal m_maxChroma = Helper::LchDefaults::maxSrgbChroma;
    static constexpr qreal m_singleStepChroma = 1;
    /* TODO What would be a good value for this? Its effect depends on chroma: On higher chroma,
     * the same step in hue means a bigger visual color differente. We could even calculate that,
     * but it does not seem to be very intuitive if the reaction on mouse wheel events are different
     * depending on chroma - that would not be easy to unserstand for the user. And it might be
     * better that the user this way also notices intuitively that hue changes are not linear
     * accross chroma. Anyway: What would be a sensible default step? for m_singleStepHue? */
    static constexpr qreal m_singleStepHue = 1;
    static constexpr qreal m_pageStepChroma = 10 * m_singleStepChroma;
    static constexpr qreal m_pageStepHue = 10 * m_singleStepHue;

    static QImage generateDiagramImage(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    static QImage generateDiagramImage2(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    QPoint currentImageCoordinates();
    QPointF fromImageCoordinatesToAB(const QPoint imageCoordinates);
    bool imageCoordinatesInGamut(const QPoint imageCoordinates);
    void updateWheelCache();
    void updateDiagramCache();
    void setWidgetCoordinates(const QPoint newImageCoordinates);
    void updateBorder();
};

}

#endif // CHROMAHUEDIAGRAM_H
