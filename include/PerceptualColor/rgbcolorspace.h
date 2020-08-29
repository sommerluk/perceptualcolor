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

#ifndef RGBCOLORSPACE_H
#define RGBCOLORSPACE_H

#include <QObject>

#include <lcms2.h>

#include <PerceptualColor/helper.h>

namespace PerceptualColor {

/** @brief Interface to LittleCMS for working with an RGB color space */
class RgbColorSpace : public QObject
{
    Q_OBJECT

public:
    RgbColorSpace(QObject *parent = nullptr);
    virtual ~RgbColorSpace();
    qreal blackpointL() const;
    QColor colorRgb(const cmsCIELab &Lab) const;
    QColor colorRgb(const cmsCIELCh &LCh) const;
    Helper::cmsRGB colorRgbBoundSimple(const cmsCIELab &Lab) const;
    QColor colorRgbBound(const cmsCIELab &Lab) const;
    QColor colorRgbBound(const cmsCIELCh &LCh) const;
    cmsCIELab colorLab(const QColor &rgbColor) const;
    cmsCIELab colorLab(const Helper::cmsRGB &rgb) const;
    QString description() const;
    bool inGamut(const cmsCIELab &Lab);
    bool inGamut(
        const cmsFloat64Number lightness,
        const cmsFloat64Number chroma,
        const cmsFloat64Number hue
    );
    bool inGamut(const cmsCIELCh &LCh);
    qreal whitepointL() const;

private:
    Q_DISABLE_COPY(RgbColorSpace)
    qreal m_blackpointL;
    /** internal storage for description() property. */
    QString m_description;
    cmsHTRANSFORM m_transformLabToRgb16Handle;
    cmsHTRANSFORM m_transformLabToRgbHandle;
    cmsHTRANSFORM m_transformRgbToLabHandle;
    qreal m_whitepointL;
    static QString getInformationFromProfile(
        cmsHPROFILE profileHandle,
        cmsInfoType infoType
    );
};

}

#endif // RGBCOLORSPACE_H
