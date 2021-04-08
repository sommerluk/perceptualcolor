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

#ifndef GRADIENTIMAGE_H
#define GRADIENTIMAGE_H

#include <QImage>
#include <QSharedPointer>

#include "PerceptualColor/lchadouble.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief An image of a gradient.
 *
 * As the hue is a circular property, there exists two ways to go one hue to
 * another (clockwise or counter-clockwise). This gradient takes always the
 * shortest way.
 *
 * The image has properties that can be accessed by the corresponding setters
 * and getters.
 *
 * This class has a cache. The data is cached because it is expensive to
 * calculate it again and again on the fly.
 *
 * When changing one of the properties, the image is <em>not</em> calculated
 * inmediatly. But the old image in the cache is deleted, so that this
 * memory becomes inmediatly availabe. Once you use @ref getImage() the next
 * time, a new image is calculated and cached. As long as you do not change
 * the properties, the next call of @ref getImage() will be very fast, as
 * it returns just the cache.
 *
 * This class is intended for usage in widgets that need to display a
 * gradient. It is recommended to update the properties of this class as
 * early as possible: If your widget is resized, use inmediatly also
 * @ref setGradientLength and @ref setGradientThickness to update this object.
 * (This will reduce your memory usage, as no memory will be hold for
 * out-of-date cache data.)
 *
 * This class supports HiDPI via its @ref setDevicePixelRatioF function.
 *
 * @note Resetting a property to its very same value does not trigger an
 * image calculation. So, if @ref setGradientThickness is 5, and you
 * call @ref setGradientThickness <tt>(5)</tt>, than this will not
 * trigger an image calculation, but the cache stays valid and available.
 *
 * @note This class is not based on <tt>QCache</tt> or <tt>QPixmapCache</tt>
 * because the semantic is different.
 *
 * @note This class is not part of the public API, but just for internal
 * usage. Therefore, its interface is incomplete and contains only the
 * functions that are really used in the rest of the source code (property
 * setters are available, but getters might be missing), and it does not use
 * the pimpl idiom either. */
class GradientImage final
{
public:
    explicit GradientImage(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
    );
    LchaDouble colorFromValue(qreal value) const;
    QImage getImage();
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setFirstColor(const LchaDouble &newFirstColor);
    void setGradientLength(const int newGradientLength);
    void setGradientThickness(const int newGradientThickness);
    void setSecondColor(const LchaDouble &newFirstColor);

private:
    Q_DISABLE_COPY(GradientImage)

    /** @brief Only for unit tests. */
    friend class TestGradientImage;

    // Methods
    static LchaDouble completlyNormalizedAndBounded(const LchaDouble &color);
    void updateSecondColor();

    // Data members
    /** @brief Internal storage of the device pixel ratio as floating point.
     *
     * @sa @ref setDevicePixelRatioF() */
    qreal m_devicePixelRatioF = 1;
    /** @brief Internal storage of the first color.
     *
     * The color is normalized and bound to the LCH color space.
     * @sa @ref completlyNormalizedAndBounded() */
    LchaDouble m_firstColorCorrected;
    /** @brief Internal storage for the gradient length, measured in
     * physical pixels.
     *
     * @sa @ref setGradientLength() */
    int m_gradientLength = 0;
    /** @brief Internal storage for the gradient thickness, measured in
     * physical pixels.
     *
     * @sa @ref setGradientThickness() */
    int m_gradientThickness = 0;
    /** @brief Internal storage of the image (cache).
     *
     * - If <tt>m_image.isNull()</tt> than either no cache is available
     *   or @ref m_gradientLength or @ref m_gradientThickness is <tt>0</tt>.
     *   Before using it, a new image has to be rendered. (If
     *   @ref m_gradientLength or @ref m_gradientThickness is
     *   <tt>0</tt>, this will be extremly fast.)
     * - If <tt>m_image.isNull()</tt> is <tt>false</tt>, than the cache
     *   is valid and can be used directly. */
    QImage m_image;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief Internal storage of the second color (corrected and altered
     * value).
     *
     * The color is normalized and bound to the LCH color space. In an
     * additional step, it has been altered (by increasing or decreasing the
     * hue component in steps of 360°) to minimize the distance in hue
     * from this color to @ref m_firstColorCorrected. This is necessary to
     * easily allow to calculate the intermediate colors of the gradient, so
     * that they take the shortest way through the color space.
     * @sa @ref setFirstColor()
     * @sa @ref setSecondColor()
     * @sa @ref completlyNormalizedAndBounded()
     * @sa @ref updateSecondColor() */
    LchaDouble m_secondColorCorrectedAndAltered;

};

}

#endif // GRADIENTIMAGE_H
