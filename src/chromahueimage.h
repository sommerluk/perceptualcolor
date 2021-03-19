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

#ifndef CHROMAHUEIMAGE_H
#define CHROMAHUEIMAGE_H

#include <QImage>
#include <QObject>
#include <QSharedPointer>

#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief An image of a chroma hue plane.
 *
 * This is a cut through the gamut body. The cut is orthogonal to
 * the L axis, so it shows the a-b diagram (speaking in terms of LAB
 * color model) also known as chroma-hue diagram (speaking in terms
 * of LCH color model). The center of the coordiante system is in
 * the center of the image. This might be a non-integer value. Each
 * pixel has the color that corresponds to the coordinate point <em>at
 * the middle</em> of the pixel.
 *
 * The image has properties that can be accessed by the corresponding setters
 * and getters.
 *
 * This class has a cache. The data is cached because it is expensive to
 * calculate the image again and again on the fly.
 *
 * When changing one of the properties, the image is <em>not</em> calculated
 * inmediatly. But the old image in the cache is deleted, so that this
 * memory becomes inmediatly availabe. Once you use @ref getImage() the next
 * time, a new image is calculated and cached. As long as you do not change
 * the properties, the next call of @ref getImage() will be very fast, as
 * it returns just the cache.
 *
 * This class is intended for usage in widgets that need to display a
 * chroma-hue diagram. It is recommended to update the properties of this
 * class as early as possible: If your widget is resized, use inmediatly also
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
 * @note This class is not based on <tt>QCache</tt> or <tt>QPixmapCache</tt>
 * because the semantic is different.
 *
 * @note This class is not part of the public API, but just for internal
 * usage. Therefore, its interface is incomplete and contains only the
 * functions that are really used in the rest of the source code (property
 * setters are available, but getters might be missing), and it does not use
 * the pimpl idiom either. */
class ChromaHueImage final
{
public:
    explicit ChromaHueImage(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
    );
    QImage getImage();
    void setBorder(const qreal newBorder);
    void setChromaRange(const qreal newChromaRange);
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setImageSize(const int newImageSize);
    void setLightness(const qreal newLightness);

private:
    Q_DISABLE_COPY(ChromaHueImage)

    /** @brief Only for unit tests. */
    friend class TestChromaHueImage;

    /** @brief Internal store for the border size, measured in physical pixels.
     *
     * @sa @ref setBorder() */
    qreal m_borderPhysical = 0;
    /** @brief Internal storage of the device pixel ratio property
     * as floating point.
     *
     * @sa @ref setDevicePixelRatioF() */
    qreal m_devicePixelRatioF = 1;
    /** @brief Internal storage of the image (cache).
     *
     * - If <tt>m_image.isNull()</tt> than either no cache is available
     *   or @ref m_imageSizePhysical is <tt>0</tt>. Before using it,
     *   a new image has to be rendered. (If @ref m_imageSizePhysical
     *   is <tt>0</tt>, this will be extremly fast.)
     * - If <tt>m_image.isNull()</tt> is <tt>false</tt>, than the cache
     *   is valid and can be used directly. */
    QImage m_image;
    /** @brief Internal store for the image size, measured in physical pixels.
     *
     * @sa @ref setImageSize() */
    int m_imageSizePhysical = 0;
    /** @brief Internal store for the lightness.
     *
     * This is the lightness (L) value in the LCH color model.
     *
     * Range: <tt>[0, 100]</tt>
     *
     * @sa @ref setLightness() */
    qreal m_lightness = 50;
    /** @brief Internal store for the chroma range.
     *
     * This is the chroma (C) value in the LCH color model.
     *
     * Only the range from <tt>0</tt> up to this value will be visible
     * in the diagram image.
     *
     * @sa @ref setChromaRange() */
    qreal m_chromaRange = 0;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;

};

}

#endif // CHROMAHUEIMAGE_H
