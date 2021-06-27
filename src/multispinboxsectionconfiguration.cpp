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

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/multispinboxsectionconfiguration.h"
// Second, the private implementation.
#include "multispinboxsectionconfiguration_p.h"

#include <QDebug>

#include "helper.h"

namespace PerceptualColor
{
/** @brief Constructor
 *
 * The object is initialized with default values. */
MultiSpinBoxSectionConfiguration::MultiSpinBoxSectionConfiguration()
    : d_pointer(new MultiSpinBoxSectionConfigurationPrivate())
{
}

/** @brief Destructor */
MultiSpinBoxSectionConfiguration::~MultiSpinBoxSectionConfiguration() noexcept
{
}

/** @brief Copy constructor
 *
 * @param other the object to be copied */
MultiSpinBoxSectionConfiguration::MultiSpinBoxSectionConfiguration(const MultiSpinBoxSectionConfiguration &other)
{
    d_pointer.reset(
        // Create a copy of the private implementation object
        new MultiSpinBoxSectionConfigurationPrivate(*other.d_pointer));
}

/** @brief Copy assignment operator
 *
 * @param other the object to be copied */
MultiSpinBoxSectionConfiguration &MultiSpinBoxSectionConfiguration::operator=(const MultiSpinBoxSectionConfiguration &other)
{
    if (this != &other) { // protect against invalid self-assignment
        d_pointer.reset(
            // Create a copy of the private implementation object
            new MultiSpinBoxSectionConfigurationPrivate(*other.d_pointer));
    }

    return *this; // By convention, always return *this.
}

/** @brief Move constructor
 *
 * @param other the object to move */
MultiSpinBoxSectionConfiguration::MultiSpinBoxSectionConfiguration(MultiSpinBoxSectionConfiguration &&other) noexcept
{
    // .reset() deletes the old object and resets the pointer to the
    // new one. Unlike the move assignment, it does however not update
    // the  deleters. We  use it nevertheless, because move assignment
    // is currently not supported  by our pointer type, and at the
    // same time we do not use deleters.
    d_pointer.reset(other.d_pointer.release());
}

/** @brief Move assignment operator
 *
 * @param other the object to move-assign */
MultiSpinBoxSectionConfiguration &MultiSpinBoxSectionConfiguration::operator=(MultiSpinBoxSectionConfiguration &&other) noexcept
{
    if (this != &other) { // protect against invalid self-assignment
        // .reset() deletes the old object and resets the pointer to the
        // new one. Unlike the move assignment, it does however not update
        // the  deleters. We  use it nevertheless, because move assignment
        // is currently not supported  by our pointer type, and at the
        // same time we do not use deleters.
        d_pointer.reset(other.d_pointer.release());
    }

    return *this; // By convention, always return *this.
}

/** @brief The number of digits after the decimal point.
 *
 * This value can also be <tt>0</tt> to get integer-like behavoir.
 *
 * @sa @ref setDecimals */
int MultiSpinBoxSectionConfiguration::decimals() const
{
    return d_pointer->m_decimals;
}

/** @brief Setter for @ref decimals property.
 *
 * @param newDecimals The new decimals values. */
void MultiSpinBoxSectionConfiguration::setDecimals(int newDecimals)
{
    d_pointer->m_decimals = qBound(0, newDecimals, 323);
}

/** @brief Holds whether or not @ref MultiSpinBox::sectionValues wrap
 * around when they reaches @ref minimum or @ref maximum.
 *
 * The default is <tt>false</tt>.
 *
 * If <tt>false</tt>, @ref MultiSpinBox::sectionValues shall be bound
 * between @ref minimum and  @ref maximum. If <tt>true</tt>,
 * @ref MultiSpinBox::sectionValues shall be treated as a circular.
 *
 * Example: You have a section that displays a value measured in
 * degree. @ref minimum is <tt>0</tt>. @ref maximum is <tt>360</tt>.
 * The following corrections would be applied to input:
 * | Input | isWrapping == false | isWrapping == true |
 * | ----: | ------------------: | -----------------: |
 * |    -5 |                   0 |                355 |
 * |     0 |                   0 |                  0 |
 * |     5 |                   5 |                  5 |
 * |   355 |                 355 |                355 |
 * |   360 |                 360 |                  0 |
 * |   365 |                 360 |                  5 |
 * |   715 |                 360 |                355 |
 * |   720 |                 360 |                  0 |
 * |   725 |                 360 |                  5 |
 *
 * @sa @ref setWrapping */
bool MultiSpinBoxSectionConfiguration::isWrapping() const
{
    return d_pointer->m_isWrapping;
}

/** @brief Setter for @ref isWrapping property.
 *
 * @param newIsWrapping The new isWrapping value. */
void MultiSpinBoxSectionConfiguration::setWrapping(bool newIsWrapping)
{
    d_pointer->m_isWrapping = newIsWrapping;
}

/** @brief The maximum possible value of the section.
 *
 * @sa @ref setMaximum */
double MultiSpinBoxSectionConfiguration::maximum() const
{
    return roundToDigits(d_pointer->m_maximum, d_pointer->m_decimals);
}

/** @brief Setter for @ref maximum property.
 *
 * @param newMaximum The new maximum value. */
void MultiSpinBoxSectionConfiguration::setMaximum(double newMaximum)
{
    d_pointer->m_maximum = newMaximum;
    if (d_pointer->m_minimum > d_pointer->m_maximum) {
        d_pointer->m_minimum = d_pointer->m_maximum;
    }
}

/** @brief The minimum possible value of the section.
 *
 * @sa @ref setMinimum */
double MultiSpinBoxSectionConfiguration::minimum() const
{
    return roundToDigits(d_pointer->m_minimum, d_pointer->m_decimals);
}

/** @brief Setter for @ref minimum property.
 *
 * @param newMinimum The new minimum value. */
void MultiSpinBoxSectionConfiguration::setMinimum(double newMinimum)
{
    d_pointer->m_minimum = newMinimum;
    if (d_pointer->m_maximum < d_pointer->m_minimum) {
        d_pointer->m_maximum = d_pointer->m_minimum;
    }
}

/** @brief A prefix to be displayed before the value.
 *
 * @sa @ref setPrefix */
QString MultiSpinBoxSectionConfiguration::prefix() const
{
    return d_pointer->m_prefix;
}

/** @brief Setter for @ref prefix property.
 *
 * @param newPrefix The new prefix value. */
void MultiSpinBoxSectionConfiguration::setPrefix(const QString &newPrefix)
{
    d_pointer->m_prefix = newPrefix;
}

/** @brief The smaller of two natural steps.
 *
 * Valid range: >= 0
 *
 * When the user uses the arrows to change the spin box’s value
 * the value will be incremented/decremented by the amount of the
 * @ref singleStep.
 *
 * @sa @ref setSingleStep */
double MultiSpinBoxSectionConfiguration::singleStep() const
{
    return d_pointer->m_singleStep;
}

/** @brief Setter for @ref singleStep property.
 *
 * @param newSingleStep The new single step value. */
void MultiSpinBoxSectionConfiguration::setSingleStep(double newSingleStep)
{
    d_pointer->m_singleStep = qMax<double>(0, newSingleStep);
}

/** @brief A suffix to be displayed behind the value.
 *
 * @sa @ref setSuffix */
QString MultiSpinBoxSectionConfiguration::suffix() const
{
    return d_pointer->m_suffix;
}

/** @brief Setter for @ref suffix property.
 *
 * @param newSuffix The new suffix value. */
void MultiSpinBoxSectionConfiguration::setSuffix(const QString &newSuffix)
{
    d_pointer->m_suffix = newSuffix;
}

/** @brief Adds QDebug() support for data type
 * @ref PerceptualColor::MultiSpinBoxSectionConfiguration
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSectionConfiguration &value)
{
    dbg.nospace() << "\nMultiSpinBoxSectionConfiguration("      // Opening line
                  << "\n    prefix: " << value.prefix()         //
                  << "\n    minimum: " << value.minimum()       //
                  << "\n    decimals: " << value.decimals()     //
                  << "\n    isWrapping: " << value.isWrapping() //
                  << "\n    maximum: " << value.maximum()       //
                  << "\n    suffix: " << value.suffix()         //
                  << "\n)"                                      // Closing line
        ;
    return dbg.maybeSpace();
}

} // namespace PerceptualColor
