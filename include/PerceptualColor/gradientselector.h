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

#ifndef GRADIENTSELECTOR_H
#define GRADIENTSELECTOR_H

#include <PerceptualColor/abstractdiagram.h>
#include <PerceptualColor/fullcolordescription.h>
#include <PerceptualColor/rgbcolorspace.h>

#include <QPointer>
#include <QWidget>

namespace PerceptualColor {

/** @brief A slider displaying a gradient
 * 
 * A slider that displays a gradient between two LCh colors. The gradient
 * is calculated by equal steps in the LCh color space. Concerning the h
 * value (hue), which is circular, always the shorter side of the circle
 * is chosen. Examples:
 * @li If the first hue is 182° and the second hue is 1°, than
 *     the hue will increase from 182° to 360° than 1°.
 * @li If the first hue is 169° and the second hue is 359°, than
 *     the hue will decrease from 169° to 0°, than 359°.
 * 
 * This widget also renders the alpha channel, using a background
 * of gray squares for colors that are not fully opaque.
 * 
 * Note that due to this mathematical model, there might be out-of-gamut
 * colors within the slider even if both, the first and the second color are
 * in-gamut colors. Out-of-gamut colors are not rendered, so you might see
 * a hole in the gradient.
 */
class GradientSelector : public AbstractDiagram
{
    Q_OBJECT


    /** @brief Orientation of the widget
     * 
     * @sa setOrientation()
     * @sa m_orientation()
     */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

    /** @brief The value of the gradient
     * 
     * Ranges from 0 to 1.
     * 
     * - 0 means: totally firstColor()
     * - 1 means: totally secondColor()
     * 
     * @sa setFraction()
     * @sa m_fraction()
     */
    Q_PROPERTY(qreal fraction READ fraction WRITE setFraction NOTIFY fractionChanged USER true)

    /** @brief This property holds the page step.
     * 
     * The larger of two natural steps this widget provides.
     * Corresponds to the user pressing PageUp or PageDown.
     * 
     * @sa setPageStep()
     * @sa m_pageStep()
     */
    Q_PROPERTY(qreal pageStep READ pageStep WRITE setPageStep)

    /** @brief This property holds the single step.
     * 
     * The smaller of two natural steps this widget provides.
     * Corresponds to the user pressing an arrow key.
     * 
     * @sa setSingleStep()
     * @sa m_singleStep()
     */
    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep)

public:
    explicit GradientSelector(RgbColorSpace *colorSpace, QWidget *parent = nullptr);
    explicit GradientSelector(RgbColorSpace *colorSpace, Qt::Orientation orientation, QWidget *parent = nullptr);

    virtual QSize sizeHint() const override;

    virtual QSize minimumSizeHint() const override;
    Qt::Orientation	orientation() const;
    qreal fraction();
    qreal singleStep();
    qreal pageStep();

Q_SIGNALS:
    /** @brief Signal for fraction() property. */
    void fractionChanged(const qreal newFraction);

public Q_SLOTS:
    void setOrientation(const Qt::Orientation orientation);
    void setColors(const PerceptualColor::FullColorDescription &col1, const PerceptualColor::FullColorDescription &col2);
    void setFirstColor(const PerceptualColor::FullColorDescription &col);
    void setSecondColor(const PerceptualColor::FullColorDescription &col);
    void setFraction(const qreal newFraction);
    void setSingleStep(const qreal newSingleStep);
    void setPageStep(const qreal newPageStep);

protected:

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(GradientSelector)
    int m_gradientThickness = 20;
    int m_gradientMinimumLength = 84;
    Qt::Orientation m_orientation;
    void initialize(RgbColorSpace* colorSpace, Qt::Orientation orientation);
    FullColorDescription m_firstColor;
    FullColorDescription m_secondColor;
    QPointer<RgbColorSpace> m_rgbColorSpace;
    void updateGradientImage();
    QPair<cmsCIELCh, qreal> intermediateColor(const cmsCIELCh &firstColor, const cmsCIELCh &secondColor, qreal fraction);
    /** @brief Cache for the gradient image
     * 
     * Holds the current gradient image (without the selection cursor). Always
     * at the left is the first color, always at the right is the second color.
     * So when painting, it might be necessary to rotate the image.
     * 
     * This is a cache. Before using it, check if it's up-to-date with
     * m_gradientImageReady(). If not, use updateGradientImage() to
     * update it.
     * 
     * If something in the widget makes a new m_gradienImage() necessary,
     * do not directly call updateGradientImage() but just set
     * m_gradientImageReady to @e false. So it can be re-generated next time
     * it's actually used, and we do not waste CPU power for updating for
     * example invisible widgets.
     * 
     * \sa m_transform()
     * \sa updateGradientImage()
     * \sa m_gradientImageReady()
     */
    QImage m_gradientImage;
    /** If the m_gradienImage() is up-to-date. If false, you have to call
     *  updateGradientImage() before using m_gradienImage(). 
     * \sa m_transform()
     * \sa updateGradientImage()
     * \sa m_gradientImage() */
    bool m_gradientImageReady = false;
    /** @brief The transform for painting on the widget.
     * 
     * Depends on layoutDirection() and orientation() */
    QTransform m_transform;
    QTransform getTransform() const;
    qreal m_fraction = 0.5;
    qreal fromWindowCoordinatesToFraction(QPoint windowCoordinates);
    qreal m_singleStep = 0.01;
    qreal m_pageStep = 0.1;
};

}

#endif // GRADIENTSELECTOR_H
