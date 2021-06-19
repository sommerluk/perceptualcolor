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

#ifndef MULTICOLOR_H
#define MULTICOLOR_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include "rgbcolorspace.h"
#include "rgbdouble.h"

#include <QColor>
#include <QSharedPointer>

namespace PerceptualColor
{
/** @internal
 *
 * @brief A color stored in multiple color models simultaneously.
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
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * This data type can be passed to QDebug thanks to
 * operator<<(QDebug dbg, const PerceptualColor::MultiColor &value)
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
 * @todo TODO What if an API user sets a MultiColor property
 * within a widget, but widget and MultiColor are
 * based on different color spaces? Display would be wrong! Or not? */
class MultiColor final
{
public:
    enum class OutOfGamutBehaviour {
        preserve,     /**< Leave LCh values as-is. Only the RGB is forced
                           into the gamut. */
        sacrifyChroma /**< Preserve the hue and the lightness and change the
                           chroma until we are within the gamut. Note that it
                           is not always possible to preserve the lightness
                           even when it’s in the range 0..100 because some
                           color profiles do not have pure black in the gamut.
                           In these cases first the nearest available
                           lightness is searched, and than the nearest
                           in-gamut chroma at this lightness. */
    };
    // No Q_ENUM declaration because this only works for QObject-based classes.

    MultiColor();
    MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, const PerceptualColor::RgbDouble &rgb, qreal alpha = 1);
    MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, QColor rgb);
    MultiColor(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &lch, OutOfGamutBehaviour coordinates, qreal alpha = 1);

    /** @brief Default copy constructor
     *
     * @param other the object to copy
     */
    MultiColor(const MultiColor &other) = default;

    /** @brief Default move constructor
     *
     * @param other the object to move
     */
    MultiColor(MultiColor &&other) noexcept = default;

    /** @brief Default Assignment operator */
    MultiColor &operator=(const MultiColor &other) = default;

    /** @brief Default move assignment operator */
    MultiColor &operator=(MultiColor &&other) noexcept = default;

    bool operator==(const MultiColor &other) const;

    bool operator!=(const MultiColor &other) const;

    QSharedPointer<RgbColorSpace> rgbColorSpace() const;

    PerceptualColor::RgbDouble toRgb() const;
    QColor toRgbQColor() const;
    QColor toHsvQColor() const;
    QString toRgbHexString() const;
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
    /** LCh representation. */
    LchDouble m_lch;
    /** HSV representation within a QColor object */
    QColor m_hsvQColor;
    /** The range is 0 (fully transparent) to 1 (fully opaque). */
    qreal m_alpha;
    /** Validity of this object. */
    bool m_isValid;

    void normalizeLch();
};

QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiColor)
Q_DECLARE_METATYPE(PerceptualColor::MultiColor::OutOfGamutBehaviour)

#endif // MULTICOLOR_H
