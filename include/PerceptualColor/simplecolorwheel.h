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

#ifndef SIMPLECOLORWHEEL_H
#define SIMPLECOLORWHEEL_H

#include <QImage>
#include <QWidget>

#include "PerceptualColor/abstractcirculardiagram.h"
#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief Widget to choose the hue in a wheel.
 * 
 * This widget allows to select the hue (as defined in the LCh color space)
 * in the form of a wheel.
 * 
 * @note This widget <em>always</em> accepts focus by a mouse click within
 * the circle. This happens regardless of the <tt>QWidget::focusPolicy</tt>
 * property. If you set the <tt>QWidget::focusPolicy</tt> property to a
 * value that accepts focus by mouse click, the focus will not only be
 * accepted for clicks within the actual circle, but also for clicks
 * within the surrounding rectangle.
 */
class SimpleColorWheel : public AbstractCircularDiagram
{
    Q_OBJECT

    /** @brief The currently selected hue.
     * 
     * The hue property is the LCh hue angle (measured in degree) The value
     * gets normalized according to PolarPointF::normalizedAngleDegree()
     * 
     * After changing the hue property, the widget gets updated.
     * 
     * @sa hue()
     * @sa setHue()
     * @sa resetHue()
     * @sa hueChanged()
     * @sa default_hue
     */
    Q_PROPERTY(qreal hue READ hue WRITE setHue RESET resetHue NOTIFY hueChanged USER true)

public:
    Q_INVOKABLE explicit SimpleColorWheel(
        PerceptualColor::RgbColorSpace *colorSpace,
        QWidget *parent = nullptr
    );
    virtual ~SimpleColorWheel() noexcept override;
    qreal hue() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
    static QImage generateWheelImage(
        RgbColorSpace *colorSpace,
        const int outerDiameter,
        const qreal border,
        const qreal thickness,
        const qreal lightness,
        const qreal chroma
    );

Q_SIGNALS:
    /** @brief Signal for hue() property. */
    void hueChanged(const qreal hue);

public Q_SLOTS:
    void resetHue();
    void setHue(const qreal newHue);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    int contentDiameter() const;
    int border() const;
    /** @brief the thickness of the wheel ribbon */
    static constexpr int m_wheelThickness = 20;
    /** @brief the chroma with which the wheel ribbon is painted. */
    qreal wheelRibbonChroma() const;
    
private:
    Q_DISABLE_COPY(SimpleColorWheel)

    class SimpleColorWheelPrivate;
    /** @brief Declare the private implementation as friend class.
     * 
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class SimpleColorWheelPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<SimpleColorWheelPrivate> d_pointer;

    /** @brief Only for unit tests. */
    friend class TestColorDialog;

};

}

#endif // SIMPLECOLORWHEEL_H
