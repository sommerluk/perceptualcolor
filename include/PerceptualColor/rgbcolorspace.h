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

/** @brief Interface to LittleCMS for working with an RGB color space
 * 
 * @todo Declare Q_PROPERTY for @ref profileInfoCopyright(),
 * @ref profileInfoDescription(), @ref profileInfoManufacturer(),
 * @ref profileInfoModel(), @ref whitepointL() */
class RgbColorSpace : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE RgbColorSpace(QObject *parent = nullptr);
    virtual ~RgbColorSpace();
    qreal blackpointL() const;
    Q_INVOKABLE QColor colorRgb(const cmsCIELab &Lab) const;
    Q_INVOKABLE QColor colorRgb(const cmsCIELCh &LCh) const;
    Q_INVOKABLE Helper::cmsRGB colorRgbBoundSimple(const cmsCIELab &Lab) const;
    Q_INVOKABLE QColor colorRgbBound(const cmsCIELab &Lab) const;
    Q_INVOKABLE QColor colorRgbBound(const cmsCIELCh &LCh) const;
    Q_INVOKABLE cmsCIELab colorLab(const QColor &rgbColor) const;
    Q_INVOKABLE cmsCIELab colorLab(const Helper::cmsRGB &rgb) const;
    Q_INVOKABLE bool inGamut(const cmsCIELab &Lab);
    Q_INVOKABLE bool inGamut(
        const cmsFloat64Number lightness,
        const cmsFloat64Number chroma,
        const cmsFloat64Number hue
    );
    Q_INVOKABLE bool inGamut(const cmsCIELCh &LCh);
    QString profileInfoCopyright() const;
    QString profileInfoDescription() const;
    QString profileInfoManufacturer() const;
    QString profileInfoModel() const;
    qreal whitepointL() const;

private:
    Q_DISABLE_COPY(RgbColorSpace)
    qreal m_blackpointL;
    QString m_cmsInfoCopyright;
    QString m_cmsInfoDescription;
    QString m_cmsInfoManufacturer;
    QString m_cmsInfoModel;
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
