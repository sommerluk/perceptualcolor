/*
 * Copyright 2020 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FULLCOLORDESCRIPTION_H
#define FULLCOLORDESCRIPTION_H

#include "PerceptualColor/helper.h"
#include "PerceptualColor/rgbcolorspace.h"
#include <QDebug>

namespace PerceptualColor {

/** @brief A fully qualified color.
 * 
 * This is similar to QColor, but contains both Lab color space representations
 * and RGB color space representations. The constructor takes an RgbColorSpace()
 * object to assure color management. Once constructed, the object cannot be
 * modified anymore (except the alpha value, which does not depend on color
 * management).
 * 
 * Contains an RGB, LCh and Lab representation of the color and the alpha
 * channel. The data types are compatible with LittleCMS. The LCh value is
 * normalized.
 * 
 * This class is declared as type to Qt's type system:
 * Q_DECLARE_METATYPE(PerceptualColor::FullColorDescription).
 * Depending on your use case (for example if you want
 * to use it relyably in Qt's signals and slots), you might consider calling
 * qRegisterMetaType() for this type, once you have a QApplication object.
 * 
 * This data type can be passed to QDebug thanks to
 * operator<<(QDebug dbg, const PerceptualColor::FullColorDescription &value)
 */
class FullColorDescription
{
public:

    enum class outOfGamutBehaviour {
        preserve,     /**< Leave LCh values as-is. Only the RGB is forced into the gamut. */
        sacrifyChroma /**< Preserve the hue and the lightness and change the chroma until
        we are within the gamut. Note that it is not always possible to preserve the lightness
        even when it's in the range 0..100 because some color profiles do not have pure black
        in the gamut. In these cases first the nearest available lightness is searched, and than
        the nearest in-gamut chroma at this lightness. */
    };

    FullColorDescription();
    FullColorDescription(RgbColorSpace *colorSpace, const Helper::cmsRGB &rgb, qreal alpha = 1);
    FullColorDescription(RgbColorSpace *colorSpace, QColor rgb);
    FullColorDescription(RgbColorSpace *colorSpace, const cmsCIELab &lab, outOfGamutBehaviour behaviour, qreal alpha = 1);
    FullColorDescription(RgbColorSpace *colorSpace, const cmsCIELCh &lch, outOfGamutBehaviour behaviour, qreal alpha = 1);

    /** @brief Default copy constructor
     *
     * @param other the object to copy
     */
    FullColorDescription(const FullColorDescription& other) = default;

    /** @brief Default move constructor
     *
     * @param other the object to move
     */   
    FullColorDescription(FullColorDescription&& other) noexcept = default;

    /**
     * Destructor
     */
    ~FullColorDescription() = default;

    /** @brief Default Assignment operator */
    FullColorDescription& operator=(const FullColorDescription& other) = default;

    /** @brief Default move assignment operator */
    FullColorDescription& operator=(FullColorDescription&& other) noexcept = default;

    bool operator==(const FullColorDescription& other) const;

    bool operator!=(const FullColorDescription& other) const;

    Helper::cmsRGB toRgb() const;
    QColor toRgbQColor() const;
    QColor toHsvQColor() const;
    QString toRgbHexString() const;
    cmsCIELab toLab() const;
    cmsCIELCh toLch() const;
    qreal alpha() const;
    bool isValid() const;
    void setAlpha(qreal alpha);

private:
    /** RGB representation. */
    Helper::cmsRGB m_rgb;
    /** RGB representation within a QColor object */
    QColor m_rgbQColor;
    /** Lab representation. */
    cmsCIELab m_lab;
    /** LCh representation. */
    cmsCIELCh m_lch;
    /** HSV representation within a QColor object */
    QColor m_hsvQColor;
    /** The range is 0 (fully transparent) to 1 (fully opaque). */
    qreal m_alpha;
    /** Validity of this object. */
    bool m_valid;

    void moveChromaIntoGamut(RgbColorSpace *colorSpace);
    void normalizeLch();
};

QDebug operator<<(QDebug dbg, const PerceptualColor::FullColorDescription &value);

} // namespace PerceptualColor

/** @brief Declares this data type to QMetaType().
 * 
 * This declaration is intentionally outside the namespace, according to Qt documentation. */
Q_DECLARE_METATYPE(PerceptualColor::FullColorDescription);

#endif // FULLCOLORDESCRIPTION_H
