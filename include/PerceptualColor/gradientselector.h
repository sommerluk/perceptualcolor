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
#include "PerceptualColor/constpropagatinguniquepointer.h"
#include <PerceptualColor/fullcolordescription.h>
#include <PerceptualColor/rgbcolorspace.h>

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
 * 
 * @todo Declare Q_PROPERTY for @ref setFirstColor() and @ref setSecondColor()
 */
class GradientSelector : public AbstractDiagram
{
    Q_OBJECT


    /** @brief Orientation of the widget
     * 
     * @sa setOrientation()
     * @sa m_orientation() */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    /** @brief The value of the gradient
     * 
     * Ranges from 0 to 1.
     * 
     * - 0 means: totally firstColor()
     * - 1 means: totally secondColor()
     * 
     * @sa setFraction()
     * @sa m_fraction() */
    Q_PROPERTY(qreal fraction READ fraction WRITE setFraction NOTIFY fractionChanged USER true)

    /** @brief This property holds the page step.
     * 
     * The larger of two natural steps this widget provides.
     * Corresponds to the user pressing PageUp or PageDown.
     * 
     * @sa setPageStep()
     * @sa m_pageStep() */
    Q_PROPERTY(qreal pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)

    /** @brief This property holds the single step.
     * 
     * The smaller of two natural steps this widget provides.
     * Corresponds to the user pressing an arrow key.
     * 
     * @sa setSingleStep()
     * @sa m_singleStep() */
    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)

public:
    Q_INVOKABLE explicit GradientSelector(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
        QWidget *parent = nullptr
    );
    Q_INVOKABLE explicit GradientSelector(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
        Qt::Orientation orientation,
        QWidget *parent = nullptr
    );
    virtual ~GradientSelector() noexcept override;

    virtual QSize sizeHint() const override;

    virtual QSize minimumSizeHint() const override;
    Qt::Orientation	orientation() const;
    qreal fraction();
    qreal singleStep();
    qreal pageStep();

Q_SIGNALS:
    /** @brief Signal for fraction() property. */
    void fractionChanged(const qreal newFraction);
    void orientationChanged(const Qt::Orientation newOrientation);
    void pageStepChanged(const qreal newPageStep);
    void singleStepChanged(const qreal newSingleStep);

public Q_SLOTS:
    void setOrientation(const Qt::Orientation newOrientation);
    void setColors(
        const PerceptualColor::FullColorDescription &col1,
        const PerceptualColor::FullColorDescription &col2
    );
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

    class GradientSelectorPrivate;
    /** @brief Declare the private implementation as friend class.
     * 
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class GradientSelectorPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<GradientSelectorPrivate> d_pointer;

};

}

#endif // GRADIENTSELECTOR_H
