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
#include <QPointer>

#include "PerceptualColor/extendeddoublevalidator.h"

namespace PerceptualColor {
    
/** @brief A spin box that can hold multiple sections (each with its own
 * value) at the same time.
 * 
 * This widget is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month, year…) within a single spin box.
 * However, <em>this</em> widget is flexible. You can define on your own
 * the behaviour of each section.
 * 
 * This widget works with floating point precision.
 * 
 * Example code to create a @ref MultiSpinBox for a HSV color value
 * (Hue 0°–360°, Saturation 0–255, Value 0–255) comes here:
 * @snippet test/testmultispinbox.cpp MultiSpinBox Basic example
 * 
 * @todo Imagine a section in a @ref MultiSpinBox that has 0 decimals and
 * a value of 15.7. It is displayed as 16. When the user scrolls up with
 * the mouse or uses the key ↑ than the value changes to 16.7 while the
 * display changes to 17. Isn’t is highly confusing that – after a user
 * interaction – the new internal value is different from what the users sees?
 * How does QDoubleSpinBox solves this?
 * 
 * @todo When using this widget in @ref ColorDialog, there are rounding
 * differences, for example when choosing HSV 210° 27 19, then RGB becomes
 * 17 18 19. But when passing through an RGB field, HSV will be changed
 * to 210° 26.8405 19 even if in the RGB field nothing was changed by the
 * user. This is confusing. Also: If values are 0 and then passing through
 * fields of other color representations, the value can become something
 * like 0.001 by rounding error, and so the Arrow-Down-Button will be
 * enabled thought the displayed value is 0 (and this was the value previously
 * manually entered by the user); this is confusing.
 * 
 * @todo Apparently, the validator doesn’t restict the input actually to the
 * given range. For QDoubleSpinBox however, the line edit <em>is</em>
 * restricted! Example: even if 100 is maximum, it is possible to write 444.
 * Maybe our @ref ExtendedDoubleValidator should not rely on Qt’s validator,
 * but on if QLocale is able to convert (result: valid) or not (result:
 * invalid)?!
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
 * 
 * @todo Do not text-cursor-select more than <em>one</em> value when
 * editing (or do not allow text selection at all)?
 * 
 * @todo Do unit tests for correct localization, also
 * when using locales with non-latin numerals. */
class MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    /** @brief The data of a section within a @ref MultiSpinBox.
     * 
     * For a specific section within a @ref MultiSpinBox, this data structure
     * contains on the one hand the @ref value itself, and on the other hand
     * also the various setting parameters for the widget.
     * 
     * This data type can be passed to QDebug thanks to
     * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionData &section)
     * 
     * @todo How to make this future-proof? Maybe later we want to add
     * singleStep() or pageStep() values? Are d-pointers working well when
     * this data structure has to be copy-able? */
    struct SectionData {
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
        /** @brief Holds whether or not @ref value wraps around when it
         * reaches @ref minimum or @ref maximum.
         * 
         * The default is <tt>false</tt>.
         * 
         * If <tt>false</tt>, @ref value shall be bound between @ref minimum
         * and  @ref maximum. If <tt>true</tt>, @ref value shall be treated
         * as a circular value.
         * 
         * Example: You have a section that displays a value measured in
         * degree. @ref minimum is <tt>0</tt>. @ref maximum is <tt>360</tt>.
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
         * |   725 |                 360 |                  5 | */
        bool isWrapping = false;
    };
    
    Q_INVOKABLE MultiSpinBox(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~MultiSpinBox() noexcept override = default;
    virtual QSize minimumSizeHint() const override;
    Q_INVOKABLE QList<MultiSpinBox::SectionData> sections() const;
    Q_INVOKABLE void setSections (
        const QList<MultiSpinBox::SectionData> &newSections
    );
    virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;

protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
    virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)

    /** @brief Only for unit tests. */
    friend class TestMultiSpinBox;

    /** @brief Holds the data for the sections.
     * 
     * This list is guaranteed to contain at least <em>one</em> section.
     * 
     * @sa @ref sections()
     * @sa @ref setSections() */
    QList<MultiSpinBox::SectionData> m_sections;
    /** @brief Holds the index of the currently selected section.
     * @sa @ref setCurrentIndexAndUpdateTextAndSelectValue
     * @sa @ref setCurrentIndexWithoutUpdatingText */
    int m_currentIndex = 0;
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

    static MultiSpinBox::SectionData fixedSection(
        const MultiSpinBox::SectionData &section
    );
    QString formattedValue(const SectionData &mySection) const;
    bool isCursorPositionAtCurrentSectionValue(
        const int cursorPosition
    ) const;
    void setCurrentIndexAndUpdateTextAndSelectValue(int newIndex = 0);
    void setCurrentIndexToZeroAndUpdateTextAndSelectValue();
    void setCurrentIndexWithoutUpdatingText(int newIndex);
    void updatePrefixValueSuffixText();

private Q_SLOTS:
    void reactOnCursorPositionChange(const int oldPos, const int newPos);
    void updateCurrentValueFromText(const QString &lineEditText);
};

QDebug operator<<(
    QDebug dbg,
    const PerceptualColor::MultiSpinBox::SectionData &section
);

}

#endif // MULTISPINBOX_H
