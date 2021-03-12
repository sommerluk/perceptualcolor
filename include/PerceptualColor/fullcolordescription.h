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

#ifndef FULLCOLORDESCRIPTION_H
#define FULLCOLORDESCRIPTION_H

#include "PerceptualColor/rgbcolorspace.h"

#include <QColor>
#include <QDebug>
#include <QSharedPointer>

namespace PerceptualColor {

/** @brief A fully qualified color.
 * 
 * This is similar to QColor, but contains both Lab color space
 * representations and RGB color space representations. The constructor takes
 * an RgbColorSpace() object to assure color management. Once constructed,
 * the object cannot be modified anymore (except the alpha value, which does
 * not depend on color management).
 * 
 * Contains an RGB, LCh and Lab representation of the color and the alpha
 * channel. The data types are compatible with LittleCMS. The LCh value is
 * normalized.
 * 
 * This class is declared as type to Qt's type system:
 * <tt>Q_DECLARE_METATYPE(PerceptualColor::FullColorDescription)</tt>.
 * Depending on your use case (for example if you want to use it
 * reliably in Qt's signals and slots), you might consider calling
 * <tt>qRegisterMetaType()</tt> for this type, once you have a QApplication
 * object.
 * 
 * This data type can be passed to QDebug thanks to
 * operator<<(QDebug dbg, const PerceptualColor::FullColorDescription &value)
 * 
 * @todo Conversion HSV-RGB with Qt’s QColor: Can we get a higher precison
 * with our own implementation?
 * 
 * @todo This class is rather a data type than a functional class. Currently,
 * it does not provide pimpl idiom. And: It is copyable and <tt>final</tt>.
 * Should it be switched to pimpl or not? Pimpl would be bad for performance,
 * which matters as this class will get instanciated a lot. On the other
 * hand, this class has often changed during code development and is not
 * stable, which is quite a risk if we to not have pimpl.
 * 
 * @todo TODO What if an API user sets a FullColorDescription property
 * within a widget, but widget and FullColorDescription are
 * based on different color spaces? Display would be wrong! Or not? */
class FullColorDescription final
{
public:

    enum class outOfGamutBehaviour {
        preserve,     /**< Leave LCh values as-is. Only the RGB is forced
                           into the gamut. */
        sacrifyChroma /**< Preserve the hue and the lightness and change the
                           chroma until we are within the gamut. Note that it
                           is not always possible to preserve the lightness
                           even when it's in the range 0..100 because some
                           color profiles do not have pure black in the gamut.
                           In these cases first the nearest available
                           lightness is searched, and than the nearest
                           in-gamut chroma at this lightness. */
    };
    // Q_ENUM(outOfGamutBehaviour); TODO Not possible for non-QObject classes?

    FullColorDescription();
    FullColorDescription(
        const QSharedPointer<RgbColorSpace> &colorSpace,
        const PerceptualColor::RgbDouble &rgb,
        qreal alpha = 1
    );
    FullColorDescription(
        const QSharedPointer<RgbColorSpace> &colorSpace,
        QColor rgb
    );
    FullColorDescription(
        const QSharedPointer<RgbColorSpace> &colorSpace,
        const LabDouble &lab,
        outOfGamutBehaviour coordinates,
        qreal alpha = 1
    );
    FullColorDescription(
        const QSharedPointer<RgbColorSpace> &colorSpace,
        const LchDouble &lch,
        outOfGamutBehaviour coordinates,
        qreal alpha = 1
    );

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

    /** @brief Default Assignment operator */
    FullColorDescription& operator=(
        const FullColorDescription& other
    ) = default;

    /** @brief Default move assignment operator */
    FullColorDescription& operator=(
        FullColorDescription&& other
    ) noexcept = default;

    bool operator==(const FullColorDescription& other) const;

    bool operator!=(const FullColorDescription& other) const;
    
    QSharedPointer<RgbColorSpace> rgbColorSpace() const;

    PerceptualColor::RgbDouble toRgb() const;
    QColor toRgbQColor() const;
    QColor toHsvQColor() const;
    QString toRgbHexString() const;
    LabDouble toLab() const;
    LchDouble toLch() const;
    qreal alpha() const;
    bool isValid() const;
    void setAlpha(qreal alpha);

private:
    /** @brief Pointer to the used RGB color space.
     * @note Thought there is intentionally no public interface to change
     * this value, it is not marked as <tt>const</tt>; this is done to allow
     * the implementation of copy constructors. */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;
    /** RGB representation. */
    PerceptualColor::RgbDouble m_rgb;
    /** RGB representation within a QColor object */
    QColor m_rgbQColor;
    /** Lab representation. */
    LabDouble m_lab;
    /** LCh representation. */
    LchDouble m_lch;
    /** HSV representation within a QColor object */
    QColor m_hsvQColor;
    /** The range is 0 (fully transparent) to 1 (fully opaque). */
    qreal m_alpha;
    /** Validity of this object. */
    bool m_valid;

    void moveChromaIntoGamut(const QSharedPointer<RgbColorSpace> &colorSpace);
    void normalizeLch();
    static LabDouble toLab(const LchDouble &lch);
    static LchDouble toLch(const LabDouble &lab);
};

QDebug operator<<(
    QDebug dbg,
    const PerceptualColor::FullColorDescription &value
);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::FullColorDescription)

#endif // FULLCOLORDESCRIPTION_H
