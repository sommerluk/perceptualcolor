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

// Include the header of the public class of this private implementation.
#include "PerceptualColor/rgbcolorspace.h"
#include "constpropagatingrawpointer.h"

#include "rgbdouble.h"

namespace PerceptualColor
{
/** @brief Private implementation within the <em>Pointer to
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

    qreal m_blackpointL;
    QString m_cmsInfoCopyright;
    QString m_cmsInfoDescription;
    QString m_cmsInfoManufacturer;
    QString m_cmsInfoModel;
    cmsHTRANSFORM m_transformLabToRgb16Handle;
    cmsHTRANSFORM m_transformLabToRgbHandle;
    cmsHTRANSFORM m_transformRgbToLabHandle;
    qreal m_whitepointL;

    cmsCIELab colorLab(const RgbDouble &rgb) const;
    RgbDouble colorRgbBoundSimple(const cmsCIELab &Lab) const;
    static QString getInformationFromProfile(cmsHPROFILE profileHandle, cmsInfoType infoType);

private:
    Q_DISABLE_COPY(RgbColorSpacePrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<RgbColorSpace> q_pointer;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACE_P_H
