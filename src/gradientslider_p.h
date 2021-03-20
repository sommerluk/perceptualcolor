// SPDX-License-Identifier: MIT
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

#ifndef GRADIENTSLIDER_P_H
#define GRADIENTSLIDER_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/gradientslider.h"
#include "constpropagatingrawpointer.h"

#include "PerceptualColor/lchdouble.h"

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class GradientSlider::GradientSliderPrivate final
{
public:
    GradientSliderPrivate(GradientSlider *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~GradientSliderPrivate() noexcept = default;

    /** @brief The thickness of the gradient, measured in widget coordinate
     * system.
     *
     * @note It would be interesting to consider the current style’s value for
     * <tt>QStyle::PixelMetric::PM_SliderControlThickness</tt> for this.
     * Unfortunally, many styles simply return <tt>0</tt> for this if
     * the options are not correctly initialized: It seems necessary to
     * use the protected <tt>QSlider::initStyleOption</tt> and also pass
     * <tt>this</tt> as last argument (for the widget) to make this work;
     * therefore, we would have to subclass QSlider. */
    int m_gradientThickness = 20;
    int m_gradientMinimumLength = 84;
    Qt::Orientation m_orientation;
    void initialize(
        const QSharedPointer<RgbColorSpace> &colorSpace,
        Qt::Orientation orientation
    );
    LchaDouble m_firstColor;
    LchaDouble m_secondColor;
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;
    void setOrientationAndForceUpdate(const Qt::Orientation newOrientation);
    void updateGradientImage();
    QPair<LchDouble, qreal> intermediateColor(
        const LchaDouble &firstColor,
        const LchaDouble &secondColor,
        qreal value
    );
    /** @brief Cache for the gradient image
     *
     * Holds the current gradient image (without the selection cursor).
     * Always at the left is the first color, always at the right is the
     * second color. So when painting, it might be necessary to rotate
     * the image.
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
    qreal m_value = 0.5;
    qreal fromWindowCoordinatesToValue(QPoint windowCoordinates);
    qreal m_singleStep = 0.01;
    qreal m_pageStep = 0.1;

private:
    Q_DISABLE_COPY(GradientSliderPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<GradientSlider> q_pointer;
};

}

#endif // GRADIENTSLIDER_P_H
