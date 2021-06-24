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

#ifndef MULTISPINBOXSECTIONCONFIGURATION_H
#define MULTISPINBOXSECTIONCONFIGURATION_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QMetaType>
#include <QString>

#include "PerceptualColor/constpropagatinguniquepointer.h"

namespace PerceptualColor
{
/** @brief The configuration of a single section
 * within a @ref MultiSpinBox.
 *
 * For a specific section within a @ref MultiSpinBox, this configuration
 * contains various settings.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSectionConfiguration &value)
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * @internal
 *
 * Also Qt itself uses this configuration-object-based approach with its
 * QNetworkConfiguration class (including @ref pimpl and
 * copy-constructors).
 *
 * @todo Use Make sure that things like “maximum >= minimum” are
 * guaranteed. */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBoxSectionConfiguration
{
public:
    MultiSpinBoxSectionConfiguration();
    MultiSpinBoxSectionConfiguration(const MultiSpinBoxSectionConfiguration &other);
    ~MultiSpinBoxSectionConfiguration() noexcept;
    MultiSpinBoxSectionConfiguration &operator=(const MultiSpinBoxSectionConfiguration &other);
    MultiSpinBoxSectionConfiguration(MultiSpinBoxSectionConfiguration &&other) noexcept;
    MultiSpinBoxSectionConfiguration &operator=(MultiSpinBoxSectionConfiguration &&other) noexcept;

    int decimals() const;
    bool isWrapping() const;
    double maximum() const;
    double minimum() const;
    QString prefix() const;
    void setDecimals(int newDecimals);
    void setMaximum(double newMaximum);
    void setMinimum(double newMinimum);
    void setPrefix(const QString &newPrefix);
    void setSingleStep(double newSingleStep);
    void setSuffix(const QString &newSuffix);
    void setWrapping(bool newIsWrapping);
    double singleStep() const;
    QString suffix() const;

private:
    class MultiSpinBoxSectionConfigurationPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class MultiSpinBoxSectionConfigurationPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxSectionConfigurationPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestMultiSpinBoxSectionConfiguration;

    /** @internal
     * @brief Internal friend declaration.
     *
     * This class is used as configuration for @ref MultiSpinBox. Thought
     * currently there is no need for this <tt>friend</tt> declaration,
     * it is done nevertheless. (If it would become necessary later,
     * adding it would break the binary API, which we want to avoid.) */
    friend class MultiSpinBox;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSectionConfiguration &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBoxSectionConfiguration)

#endif // MULTISPINBOXSECTIONCONFIGURATION_H
