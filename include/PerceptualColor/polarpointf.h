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

#ifndef POLARPOINTF_H
#define POLARPOINTF_H

#include <QDebug>
#include <QPointF>
#include <QMetaType>

namespace PerceptualColor {

/** @brief A point in the polar coordinate system
 * 
 * Defines a point in the two-dimensional polar coordinate system using
 * floating point precision.
 * 
 * Polar coordinates allow multiple representations for a single point:
 * - An angle of 0° is the same as 360° is the same as 720° is the same
 *   as −360°.
 * - A radial of 1 and an angle of 0° is the same as a radial of −1 and an
 *   angle of 180°.
 * - If the radial is 0, the angle is meaningless: A radial of 0 and an angle
 *   of 57° is the same as a radial of 0 and an angle of 233°.
 * 
 * This class automatically normalizes the values:
 * @invariant
 * - the @ref radial() is normalized to value ≥ 0
 * - the @ref angleDegree() is normalized to 0° ≤ value < 360°
 *   (see @ref normalizedAngleDegree() for details)
 * 
 * When the radial is 0, often by convention the (meaningless) angle is set
 * also 0. This class does <em>not</em> normalize the angle to 0 when the
 * radial is 0 as long as initialized with a radial and an angle. However,
 * when initialized with Cartesian coordinates (0, 0) then the result is
 * radial 0 and angle 0. See also @ref operator==().
 * 
 * This class is declared as type to Qt’s type system:
 * <tt>Q_DECLARE_METATYPE(PerceptualColor::PolarPointF)</tt>. Depending on
 * your use case (for example if you want to use it reliably in Qt's signals
 * and slots), you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 * 
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::PolarPointF polarpointf)
 * 
 * @todo This class is rather a data type than a functional class. Currently,
 * it does not provide pimpl idiom. And: It is copyable and <tt>final</tt>.
 * Should it be switched to pimpl or not? Pimpl would be bad for performance,
 * which matters as this class will get instanciated a lot. On the other
 * hand, this class has not been often changed during code development,
 * but you never know? */
class PolarPointF final
{
public:
    explicit PolarPointF();

    /** @brief Default copy constructor
     *
     * @param other the object to copy */
    PolarPointF(const PolarPointF& other) = default;

    /** @brief Default move constructor
     *
     * @param other the object to move */    
    PolarPointF(PolarPointF&& other) noexcept = default;

    explicit PolarPointF(const qreal newRadial, const qreal newAngleDegree);

    explicit PolarPointF(const QPointF cartesianCoordiantes);

    /** @brief Default assignment operator
     *
     * @param other the object to assign */    
    PolarPointF& operator=(const PolarPointF &other) = default; // clazy:exclude=function-args-by-value

    /** @brief Default move assignment operator
     * 
     * @param other the object to move-assign */    
    PolarPointF& operator=(PolarPointF&& other) noexcept = default;

    bool operator==(const PolarPointF other) const;

    bool operator!=(const PolarPointF other) const;

    qreal angleDegree() const;

    static qreal normalizedAngleDegree(const qreal angleDegree);

    qreal radial() const;

    QPointF toCartesian() const;

private:

    /** @brief Holds the @ref angleDegree() value. */
    qreal m_angleDegree;

    /** @brief Holds the @ref radial() value. */
    qreal m_radial;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF polarpointf);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PolarPointF)

#endif // POLARPOINTF_H
