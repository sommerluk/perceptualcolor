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

#ifndef MULTISPINBOX_H
#define MULTISPINBOX_H

#include <QAbstractSpinBox>
#include <QDoubleSpinBox>
#include <QPointer>
#include <QSharedPointer>

#include <memory>

#include "PerceptualColor/extendeddoublevalidator.h"

namespace PerceptualColor {
    
/** @brief A spin box that can hold and multiple sections (each with its own
 * value) at the same time.
 * 
 * The behavior is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month…) within the same spin box.
 * 
 * This widget provides floating point precision.
 * 
 * @todo This class inherits from QAbstractSpinBox, which provides
 * properties. We have to make sure these properties work also for
 * this class. There are various types of properties.
 * 1.) Properties that have to be implemented here in @ref MultiSpinBox (such
 *     as <tt>correctionMode</tt> or <tt>showGroupSeparator</tt>).
 * 2.) Properties that should work “out of the box” but should be tested
 *     nevertheless (such as <tt>frame</tt> or <tt>accelerated</tt>
 *     or <tt>readonly</tt>).
 * 3.) Properties that cannot work with this class for design reasons, and
 *     this has to be documentated (such as <tt>wrapping</tt> which cannot
 *      be set for the hole @ref MultiSpinBox but has to be individual per
 *     section).
 * @todo Missing features: Correct behaviour when working with the lineEdit()
 * content: Do not cursor-select more than <em>one</em> value when editing?
 * Accept intermediate states like “1,“. Do correct localization, also
 * when using locales with non-latin numerals. Handle always correctly
 * the cursor position.
 * @todo Use this widget in @ref ColorDialog.
 * @todo Allow wrapping. Attention: Than, @ref stepEnabled() also has
 * to be changed, so that both, step up and step down, are always enabled.
 * Attention: When <tt>minimum == maximum</tt>, than the algorithm for
 * calculating the correct value after wrapping should not crash. */
class MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

    /* @brief The configuration of the sections of this widget.
     * 
     * @sa @ref configuration()
     * @sa @ref setConfiguration()
     * @sa @ref m_configuration
     * TODO Using this “configuration” property seems a little wired.
     * What would be a better design? */
//     Q_PROPERTY(SectionList configuration READ configuration WRITE setConfiguration)

public:
    /** @brief The data of a section within a @ref MultiSpinBox. */
    struct Section {
        /** @brief The number of digits after the decimal point. */
        int decimals = 0;
        /** @brief The maximum possible value of the section. */
        double maximum = 100;
        /** @brief The minimum possible value of the section. */
        double minimum = 0;
        /** @brief A prefix to be displayed before the value. */
        QString prefix;
        /** @brief A suffix to be displayed behind the value. */
        QString suffix;
        /** @brief The current actual value of the section. */
        double value = 0;
    };
    /** @brief A list of @ref Section data. */
    typedef QList<Section> SectionList;
    
    Q_INVOKABLE MultiSpinBox(QWidget *parent = nullptr);

    /** @brief Getter for property @ref configuration
     *  @returns the property @ref configuration */
    MultiSpinBox::SectionList configuration() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;

public Q_SLOTS:
    void setConfiguration(
        const MultiSpinBox::SectionList &configuration
    );

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
    virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)

    /** @brief Only for unit tests. */
    friend class TestMultiSpinBox;

    /** @brief Holds the configuration.
     * @sa READ @ref configuration
     * @sa WRITE @ref setConfiguration */
    MultiSpinBox::SectionList m_configuration;
    /** @brief Holds the index of the currently selected section.
     * @sa @ref setCurrentSectionIndex
     * @sa @ref setCurrentSectionIndexWithoutUpdatingText */
    int m_currentSectionIndex = 0;
    /** @brief The string of everything <em>after</em> the current section
     * value.
     * 
     * This includes the suffix of the current section and everything
     * (prefixes, values and suffixes) of all sections that  come after
     * the current sections. */
    QString m_currentSectionTextAfterValue;
    /** @brief The string of everything <em>before</em> the current section
     * value.
     * 
     * This includes everything (prefixes, values and suffixes) of all
     * sections that come before the current section, and the prefix
     * of the current section. */
    QString m_currentSectionTextBeforeValue;
    /** @brief The string of the current section value. */
    QString m_currentSectionTextOfTheValue;
    /** @brief Internal spin box object.
     * 
     * Holds an internal, invisible spin box that is used for
     * formatting correctly the value of the <em>current</em> section. */
    QDoubleSpinBox m_formatSpinBoxForCurrentValue;
    QPointer<ExtendedDoubleValidator> m_validator;

    QString formattedValue(const Section &mySection) const;
    bool isCursorPositionAtCurrentValueText(
        const int cursorPosition
    ) const;
    void setCurrentSectionIndex (int index);
    void setCurrentSectionIndexWithoutUpdatingText(int index);
    void updatePrefixValueSuffixText();

private Q_SLOTS:
    void updateSectionFromCursorPosition(const int oldPos, const int newPos);
    void updateValueFromText(const QString &lineEditText);
};

}

#endif // MULTISPINBOX_H
