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

#ifndef COLORWHEELIMAGE_H
#define COLORWHEELIMAGE_H

#include <QImage>
#include <QObject>
#include <QSharedPointer>

#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief An image of a color wheel.
 * 
 * The image has properties that can be accessed by the corresponding setters
 * and getters.
 * 
 * This class has a cache. The data is cached because it is often needed and
 * it would be expensive to calculate it again and again on the fly.
 * 
 * When changing one of the properties, the image is <em>not</em> calculated
 * inmediatly. But the old image in the cache is deleted, so that this
 * memory becomes inmediatly availabe. Once you use @ref getImage() the next
 * time, a new image is calculated and cached. As long as you do not change
 * the properties, the next call of @ref getImage() will be very fast, as
 * it returns just the cache.
 * 
 * This class is intended for usage in widgets that need to display a
 * color wheel. It is recommended to update the properties of this class as
 * early as possible: If your widget is resized, use inmediatly also
 * @ref setImageSize to update this object. (This will reduce your memory
 * usage, as no memory will be hold for data that will not be
 * needed again.)
 * 
 * This class supports HiDPI via its @ref setDevicePixelRatioF function. 
 * 
 * @note Resetting a property to its very same value does not trigger an 
 * image calculation. So, if the border is 5, and you call @ref setBorder
 * <tt>(5)</tt>, than this will not trigger an image calculation, but the
 * cache stays valid and available.
 * 
 * @note This class is not part of the public API, but just for internal
 * usage. Therefore, its interface is incomplete and contains only the
 * functions that are really used in the rest of the source code (property
 * setters are available, but getters are missing), and it does not use the
 * pimpl idiom either. */
class ColorWheelImage final
{
public:
    explicit ColorWheelImage(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
    );
    QImage getImage();
    void setBorder(const qreal newBorder);
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setImageSize(const int newImageSize);
    void setWheelThickness(const qreal newWheelThickness);

private:
    Q_DISABLE_COPY(ColorWheelImage)

    /** @brief Only for unit tests. */
    friend class TestColorWheelImage;

    void generateNewImage();

    /** @brief Internal store for the border size, measured in physical pixels.
     * 
     * @sa @ref setBorder() */
    qreal m_borderPhysical = 0;
    /** @brief Internal storage of the device pixel ratio as floating point.
     * 
     * @sa @ref setDevicePixelRatioF() */
    qreal m_devicePixelRatioF = 1;
    /** @brief Internal storage of the image (cache).
     * 
     * - If <tt>m_image.isNull()</tt> than either no cache is available
     *   or @ref m_imageSizePhysical is <tt>0</tt>. Before using it,
     *   @ref generateNewImage() has to be called to be sure to
     *   have a valid image. (If @ref m_imageSizePhysical is <tt>0</tt>,
     *   this will be extremly fast.)
     * - If <tt>m_image.isNull()</tt> is <tt>false</tt>, than the cache
     *   is valid and can be used directly. */
    QImage m_image;
    /** @brief Internal store for the image size, measured in physical pixels.
     * 
     * @sa @ref setImageSize() */
    int m_imageSizePhysical = 0;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief Internal store for the image size, measured in physical pixels.
     * 
     * @sa @ref setWheelThickness() */
    qreal m_wheelThicknessPhysical = 0;

};

}

#endif // COLORWHEELIMAGE_H
