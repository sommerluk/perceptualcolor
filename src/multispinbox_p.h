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

// Include the header of the public class of this private implementation.
#include "PerceptualColor/multispinbox.h"
#include "constpropagatingrawpointer.h"

#include <QPointer>

#include "extendeddoublevalidator.h"

namespace PerceptualColor
{
/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class MultiSpinBox::MultiSpinBoxPrivate final
{
public:
    MultiSpinBoxPrivate(MultiSpinBox *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~MultiSpinBoxPrivate() noexcept = default;

    /** @brief Only for unit tests. */
    friend class TestMultiSpinBox;

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
     * @sa @ref sections()
     * @sa @ref setSections() */
    QList<MultiSpinBox::SectionData> m_sections;
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

    static MultiSpinBox::SectionData
    fixedSection(const MultiSpinBox::SectionData &section);
    QString formattedValue(const SectionData &mySection) const;
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

}

#endif // MULTISPINBOX_P_H
