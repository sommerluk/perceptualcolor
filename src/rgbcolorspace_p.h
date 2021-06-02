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

#ifndef RGBCOLORSPACE_P_H
#define RGBCOLORSPACE_P_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "rgbcolorspace.h"

#include "chromalightnessimage.h"
#include "constpropagatingrawpointer.h"
#include "lchvalues.h"
#include "rgbdouble.h"

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class RgbColorSpace::RgbColorSpacePrivate final
{
public:
    RgbColorSpacePrivate(RgbColorSpace *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~RgbColorSpacePrivate() noexcept = default;

    // Data members:
    /** @brief The darkest in-gamut point on the L* axis.
     * @sa whitepointL */
    qreal m_blackpointL;
    QString m_cmsInfoCopyright;
    QString m_cmsInfoDescription;
    QString m_cmsInfoManufacturer;
    QString m_cmsInfoModel;
    int m_maximumChroma = LchValues::humanMaximumChroma;
    cmsHTRANSFORM m_transformLabToRgb16Handle;
    cmsHTRANSFORM m_transformLabToRgbHandle;
    cmsHTRANSFORM m_transformRgbToLabHandle;
    /** @brief The lightest in-gamut point on the L* axis.
     * @sa blackpointL() */
    qreal m_whitepointL;

    // Functions:
    cmsCIELab colorLab(const RgbDouble &rgb) const;
    RgbDouble colorRgbBoundSimple(const cmsCIELab &Lab) const;
    static QString getInformationFromProfile(cmsHPROFILE profileHandle, cmsInfoType infoType);
    cmsCIELab toLab(const QColor &rgbColor) const;
    QColor toQColorRgbBound(const cmsCIELab &Lab) const;

    // Dirty hacks:
    static QPoint nearestNeighborSearch(const QPoint originalPoint, const QImage &image);
    /** TODO WARNING This is a memory leak. This member is not deleted in
     * the desctuctor of the class. This is because ChromaLightnessImage has
     * a shared pointer to _this_ object. If nearestNeighborSearchImage
     * would be on the heap, deleting in when _this_ object is deleted will
     * call again deleting _this_ object, and this recursive destructor
     * call will crash the library. Therefore, we implement
     * nearestNeighborSearchImage as a pointer with memory leak to avoid
     * the crash. This memory leak is not fixed currently, because
     * anyway we need to re-write all the code for nearest-neigbor search. */
    ChromaLightnessImage *m_nearestNeighborSearchImage;
    static constexpr int nearestNeighborSearchImageHeight = 400;

private:
    Q_DISABLE_COPY(RgbColorSpacePrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<RgbColorSpace> q_pointer;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACE_P_H
