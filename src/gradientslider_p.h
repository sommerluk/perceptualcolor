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

#include "gradientimage.h"

namespace PerceptualColor
{
/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class GradientSlider::GradientSliderPrivate final
{
public:
    GradientSliderPrivate(GradientSlider *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~GradientSliderPrivate() noexcept = default;

    // Methods
    qreal fromWidgetPixelPositionToValue(QPoint pixelPosition);
    void initialize(const QSharedPointer<RgbColorSpace> &colorSpace, Qt::Orientation orientation);
    void setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation newOrientation);
    int physicalPixelLength() const;
    int physicalPixelThickness() const;

    // Data members
    /** @brief Internal storage for property @ref firstColor */
    LchaDouble m_firstColor;
    /** @brief Cache for the gradient image
     *
     * Holds the current gradient image (without the handle).
     * Always at the left is the first color, always at the right is the
     * second color. This is idependent from the actual @ref orientation
     * and the actual LTR or RTL layout. So when painting, it might be
     * necessary to rotate or mirror the image. */
    GradientImage m_gradientImageCache;
    /** @brief Internal storage for property @ref orientation */
    Qt::Orientation m_orientation;
    /** @brief Internal storage for property @ref m_pageStep */
    qreal m_pageStep = 0.1;
    /** @brief Pointer to the @ref RgbColorSpace object. */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;
    /** @brief Internal storage for property @ref secondColor */
    LchaDouble m_secondColor;
    /** @brief Internal storage for property @ref singleStep */
    qreal m_singleStep = 0.01;
    /** @brief Internal storage for property @ref value */
    qreal m_value = 0.5;

private:
    Q_DISABLE_COPY(GradientSliderPrivate)
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<GradientSlider> q_pointer;
};

} // namespace PerceptualColor

#endif // GRADIENTSLIDER_P_H
