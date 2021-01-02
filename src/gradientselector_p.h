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

#ifndef GRADIENTSELECTOR_P_H
#define GRADIENTSELECTOR_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/gradientselector.h"
#include "constpropagatingrawpointer.h"

#include <QPointer>

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class GradientSelector::GradientSelectorPrivate final
{
public:
    GradientSelectorPrivate(GradientSelector *backLink);
    /** @brief Default destructor
     * 
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~GradientSelectorPrivate() noexcept = default;

    int m_gradientThickness = 20;
    int m_gradientMinimumLength = 84;
    Qt::Orientation m_orientation;
    void initialize(RgbColorSpace* colorSpace, Qt::Orientation orientation);
    FullColorDescription m_firstColor;
    FullColorDescription m_secondColor;
    QPointer<RgbColorSpace> m_rgbColorSpace;
    void setOrientationAndForceUpdate(const Qt::Orientation newOrientation);
    void updateGradientImage();
    QPair<cmsCIELCh, qreal> intermediateColor(
        const cmsCIELCh &firstColor,
        const cmsCIELCh &secondColor,
        qreal fraction
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
    qreal m_fraction = 0.5;
    qreal fromWindowCoordinatesToFraction(QPoint windowCoordinates);
    qreal m_singleStep = 0.01;
    qreal m_pageStep = 0.1;

private:
    Q_DISABLE_COPY(GradientSelectorPrivate)
    
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<GradientSelector> q_pointer;
};

}

#endif // GRADIENTSELECTOR_P_H
