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

#ifndef CHROMALIGHTNESSDIAGRAM_H
#define CHROMALIGHTNESSDIAGRAM_H

#include <QImage>
#include <QPointer>
#include <QWidget>

#include <lcms2.h>

#include "PerceptualColor/abstractdiagram.h"
#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {
    
/** @brief A widget that displays a chroma-lightness diagram.
 *
 * This widget displays a chroma-lightness diagram in the LCh color model for a given hue, in a
 * Cartesian coordinate system.
 * 
 * The widget shows the chroma-lightness diagram at the whole widget extend. At the
 * y axis the diagram always shows the lightness values of 0 at the bottom pixel
 * and the lightness value of 100 at the top pixel. For the x (chroma) axis is
 * automatically used the same scale as for the y axis. So if the widget size
 * is a square, both x range and y range are from 0 to 100. If the widget
 * width is larger than the widget height, the x range goes beyond 100.
 * 
 * The widget reacts on mouse events and on keyboard events (see keyPressEvent() for details).
 * 
 * @note This widget <em>always</em> accepts focus by a mouse click within
 * the displayed gamut. This happens regardless of the <tt>QWidget::focusPolicy</tt>
 * property. If you set the <tt>QWidget::focusPolicy</tt> property to a
 * value that accepts focus by mouse click, the focus will not only be
 * accepted for clicks within the actual display gamut, but also for clicks
 * within the surrounding rectangle.
 * 
 * @todo Declare a property for @ref hue()? If not, at least Q_INVOKABLE
 */
class ChromaLightnessDiagram : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     * 
     * @sa color()
     * @sa setColor()
     * @sa colorChanged()
     */
    Q_PROPERTY(FullColorDescription color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    Q_INVOKABLE explicit ChromaLightnessDiagram(
        PerceptualColor::RgbColorSpace *colorSpace,
        QWidget *parent = nullptr
    );
    virtual ~ChromaLightnessDiagram() override = default;
    FullColorDescription color() const;
    qreal hue() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setColor(const PerceptualColor::FullColorDescription &newColor);
    void setHue(const qreal newHue);

Q_SIGNALS:
    /** @brief Signal for color() property. */
    void colorChanged(const PerceptualColor::FullColorDescription &newColor);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

private:

    Q_DISABLE_COPY(ChromaLightnessDiagram)

    /** @brief The border between the widget outer border and the diagram itself.
     * 
     * The diagram is not painted on the whole extend of the widget. A border is
     * left to allow that the selection marker can be painted completely even when
     * a pixel on the border of the diagram is selected. The border is
     * determined automatically, its value depends on @ref markerRadius and
     * @ref markerThickness.
     * 
     * @sa updateBorder()
     */
    int m_border;
    /** @brief Internal storage of the chromaLightness() property */
//     QPointF m_chromaLightness;
    /** @brief Internal storage of the color() property */
    FullColorDescription m_color;
    /** @brief A cache for the diagram as QImage. @sa updateDiagramCache() */
    QImage m_diagramImage;
    /** True if the m_diagramImage cache is up-to-date. False otherwise.
     * @sa m_diagramImage
     * @sa updateDiagramCache */
    bool m_diagramCacheReady = false;
    /** @brief If a mouse event is active
     * 
     * Holds if currently a mouse event is active or not.
     * @sa mousePressEvent()
     * @sa mouseMoveEvent()
     * @sa mouseReleaseEvent()
     */
    bool m_mouseEventActive;
    /** @brief Pointer to RgbColorSpace() object */
    QPointer<RgbColorSpace> m_rgbColorSpace;

    QImage generateDiagramImage(
        const qreal imageHue,
        const QSize imageSize) const;
    QPoint currentImageCoordinates();
    QPointF fromImageCoordinatesToChromaLightness(const QPoint imageCoordinates);
    QPoint fromWidgetCoordinatesToImageCoordinates(const QPoint widgetCoordinates) const;
    bool imageCoordinatesInGamut(const QPoint imageCoordinates);
    static QPoint nearestNeighborSearch(const QPoint originalPoint, const QImage &image);
    void updateDiagramCache();
    void setImageCoordinates(const QPoint newImageCoordinates);
    void updateBorder();
};

}

#endif // CHROMALIGHTNESSDIAGRAM_H
