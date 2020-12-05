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
#include <QSharedPointer>

#include <memory>

namespace PerceptualColor {
    
/** @brief A spin box that can hold and multiple sections (each with its own
 * value) at the same time.
 * 
 * The behavior is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month…) within the same spin box.
 * 
 * This widget provides floating point precision.
 * 
 * @todo All unit tests have to run green.
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
    
public:
    /** @brief The data of a section within a @ref MultiSpinBox. */
    struct Section {
        /** @brief The minimum possible value of the section. */
        double minimum = 0;
        /** @brief The maximum possible value of the section. */
        double maximum = 100;
        /** @brief A prefix to be displayed before the value. */
        QString prefix;
        /** @brief A suffix to be displayed behind the value. */
        QString suffix;
        /** @brief The current actual value of the section. */
        double value = 0;
    };
    /** @brief A list of @ref Section data. */
    typedef QList<Section> SectionList;
    
    MultiSpinBox(QWidget *parent = nullptr);

    MultiSpinBox::SectionList configuration() const;
    virtual QSize minimumSizeHint() const override;
    void setConfiguration(
        const MultiSpinBox::SectionList &configuration
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
    /** @brief <em>Not</em> initialized internal spin box object.
     * 
     * Holds an internal, invisible spin box that can be used for formatting.
     * The spin box is guaranteed to exists, but does not have a defined
     * state. It can be freely used by all member functions of this class.
     * So before each usage, it has to be initialized (prefix, suffix,
     * value and so on).
     * 
     * Note that the very same spin box object is shared between all instances
     * of this class. It exists as long as at least <em>one</em> instance of
     * this class exists. The synchronization between instances happens
     * through the static member @ref m_formatSpinBoxNonInitializedWeak: If it
     * actually holds a spin box, the constructor @ref MultiSpinBox() will use
     * this very same spin box is also here; if not, a new spin box is created
     * and also provided to future other instances through the stacic
     * @ref m_formatSpinBoxNonInitialized member. As the static member is
     * a <em>weak</em> pointer, while the object member is a <em>shared</em>
     * pointer, if the last instance is destroyed, automatically also the
     * spin box will be destroyed (to save memory). If a new instance is
     * created, also a new spin box will be created. */
    QSharedPointer<QDoubleSpinBox> m_formatSpinBoxNonInitialized;
    // No documentation here (documentation of this static member variable
    // is in the .cpp file at the initialization.
    static QWeakPointer<QDoubleSpinBox> m_formatSpinBoxNonInitializedWeak;

    static void applySectionConfiguration(
        const Section &section,
        QDoubleSpinBox *spinBox
    );
    void applySectionConfigurationToFormatSpinbox(int index);
    static void initializeQDoubleSpinBox(QDoubleSpinBox *spinBox);
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
