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

#ifndef MULTISPINBOX_P_H
#define MULTISPINBOX_P_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "PerceptualColor/multispinbox.h"

#include <QAccessibleWidget>
#include <QPointer>

#include "constpropagatingrawpointer.h"
#include "extendeddoublevalidator.h"

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class MultiSpinBox::MultiSpinBoxPrivate final : public QObject
{
    Q_OBJECT

public:
    MultiSpinBoxPrivate(MultiSpinBox *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~MultiSpinBoxPrivate() noexcept = default;

    /** @brief Only for unit tests. */
    friend class TestMultiSpinBox;

    // constexpr
    /** @brief Default value of a section */
    static constexpr double defaultSectionValue = 0;

    /** @brief Counter for all actions added by @ref addActionButton. */
    int m_actionButtonCount = 0;
    /** @brief Holds the index of the currently selected section.
     * @sa @ref setCurrentIndexAndUpdateTextAndSelectValue
     * @sa @ref setCurrentIndexWithoutUpdatingText */
    int m_currentIndex = 0;
    /** @brief Holds the data for the sections.
     *
     * This list is guaranteed to contain at least <em>one</em> section.
     *
     * @sa @ref sectionConfigurations()
     * @sa @ref setSectionConfigurations() */
    QList<MultiSpinBox::SectionConfiguration> m_sectionConfigurations;
    /** @brief Internal storage for property @ref sectionValues. */
    QList<double> m_sectionValues;
    /** @brief The string of everything <em>after</em> the value of the
     * current section.
     *
     * This includes the suffix of the current section and everything
     * (prefixes, values and suffixes) of all sections that come after
     * the current sections. */
    QString m_textAfterCurrentValue;
    /** @brief The string of everything <em>before</em> the value of the
     * current section.
     *
     * This includes everything (prefixes, values and suffixes) of all
     * sections that come before the current section, and the prefix
     * of the current section. */
    QString m_textBeforeCurrentValue;
    /** @brief The string of the value of the current section. */
    QString m_textOfCurrentValue;
    /** @brief The validator for the <tt>QLineEdit</tt>.
     *
     * This validator allows changes only to the <em>current</em> section.
     *
     * If the current section changes, also this validator’s configuration
     * will be adapted to cover the new current section.
     *
     * @note It is <em>not</em> possible to change various values at the
     * same time, for example by marking all the current text and use
     * Ctrl-V to past a complete new value from the clipbord. This would
     * be impossible to parse reliably, because the prefixes and suffixes
     * of each section might contain (localized) digits that would be
     * difficult to differenciate from the actual value. */
    QPointer<ExtendedDoubleValidator> m_validator;

    // Functions
    void fixSectionValue(int index);
    QString formattedValue(int index) const;
    bool isCursorPositionAtCurrentSectionValue(const int cursorPosition) const;
    void setCurrentIndexAndUpdateTextAndSelectValue(int newIndex = 0);
    void setCurrentIndexToZeroAndUpdateTextAndSelectValue();
    void setCurrentIndexWithoutUpdatingText(int newIndex);
    void updatePrefixValueSuffixText();

public Q_SLOTS:
    void reactOnCursorPositionChange(const int oldPos, const int newPos);
    void updateCurrentValueFromText(const QString &lineEditText);

private:
    Q_DISABLE_COPY(MultiSpinBoxPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<MultiSpinBox> q_pointer;
};

/** @internal
 *
 * @brief Interface for accessible objects. */
class AccessibleMultiSpinBox : public QAccessibleWidget
{
public:
    AccessibleMultiSpinBox(MultiSpinBox *w);
    virtual ~AccessibleMultiSpinBox();
    static QAccessibleInterface *factory(const QString &classname, QObject *object);
};

} // namespace PerceptualColor

#endif // MULTISPINBOX_P_H
