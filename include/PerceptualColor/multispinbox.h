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

#include <QDoubleSpinBox>

namespace PerceptualColor {
    
/** @brief A spin box that can hold and multiple sections at the same time.
 * 
 * The behavior is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month…) within the same spin box.
 * 
 * It provides floating point precision.
 *
 * @todo When apply the new value? See this from QColorDialog handling m_hlcL
 * values: function tests if the focus is leaving @ref m_hlcLineEdit(). If so,
 * it will update the other widgets if necessary. We have to to this because
 * @ref m_hlcLineEdit is a QLineEdit and its editingFinished() will not be
 * emitted if the current value is not conform to the input mask and the
 * validator. So we might miss (invalid) value changes. It is nevertheless
 * important to catch these cases, because the widget must be reset to a
 * valid value. */
class MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

    /** @brief This property holds the index of the current section.
     * 
     * By default, this property has a value of 0.
     * 
     * @sa READ @ref currentSectionIndex()
     * @sa WRITE @ref setCurrentSectionIndex()
     * @sa NOTIFY @ref currentSectionIndexChanged() */
// TODO     Q_PROPERTY(int currentSectionIndex READ currentSectionIndex WRITE setCurrentSectionIndex NOTIFY currentSectionIndexChanged)
    
public:
    MultiSpinBox(QWidget *parent = nullptr);
    struct Section {
        double value = 0;
        double maximum = 100;
        double minimum = 0;
        QString prefix;
        QString suffix;
    };
    typedef QList<Section> SectionList;
    MultiSpinBox::SectionList configuration() const;
    void setConfiguration(
        const MultiSpinBox::SectionList &configuration
    );
    int currentSectionIndex() const;
    virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;
    virtual QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentSectionIndex(int index);

Q_SIGNALS:
//     void currentSectionIndexChanged(int index);

protected:
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
    virtual QValidator::State validate(QString &input, int &pos) const override;
    virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)
    /** @brief Only for unit tests. */
    friend class TestMultiSpinBox;
    /** @brief Layout dimensions
     * 
     * This enum is declared to the meta-object system. This happens
     * automatically. You do not need to make any manual calls. */
    enum class cursorPosition {
        beforeValue, /**< Decide automatically between <tt>collapsed</tt>
                          and <tt>expanded</tt> layout based on evaluating
                          the screen size. The decision is evaluated at the
                          moment when setting this value, and again each time
                          the widget is shown again. It is <em>not</em>
                          evaluated again when a yet existing dialog is just
                          moved to another screen. */
        atValue,     /**< Use the small, “collapsed“ layout of this dialog. */
        afterValue   /**< Use the large, “expanded” layout of this dialog. */
    };

    /** @brief Holds the index of the currently selected section. */
    int m_currentSectionIndex = 0;
    /** @brief Holds the configuration. */
    MultiSpinBox::SectionList m_configuration;
    QList<QString> m_cleanText;
    /** @brief Holds an internal, invisible spin box that is used for
     * formatting correctly the values. */
    QDoubleSpinBox m_formatSpinBox;
    /** @brief The string of everything <em>before</em> the current section
     * value.
     * 
     * This includes everything (prefixes, values and suffixes) that comes
     * before the current section value. */
    QString m_currentSectionTextBeforeValue;
    /** @brief The string of the current section. */
    QString m_currentSectionTextOfTheValue;
    /** @brief The string of everything <em>after</em> the current section
     * value.
     * 
     * This includes everything (prefixes, values and suffixes) that comes
     * before the current section value. */
    QString m_currentSectionTextAfterValue;
    QSize m_sizeHint;

    static void applySectionConfiguration(
        Section section,
        QDoubleSpinBox *spinBox
    );
    static void initializeQDoubleSpinBox(QDoubleSpinBox *spinBox);
    void applySectionConfigurationToFormatSpinbox(int index);
    void setCurrentSectionIndexWithoutSelectingText(int index);
    QString cleanTextFromText(const QString &text) const;
    bool cursorPositionIsAtCurrentValueText(
        const int cursorPosition
    ) const;
    void getSectionTexts(
        const SectionList &sections,
        const int activeSectionIndex,
        QString &textBeforeValue,
        QString &textOfTheValue,
        QString &textAfterValue
    );

private Q_SLOTS:
    void updateValueFromText(const QString &text);
    void updateSectionFromCursorPosition(const int oldPos, const int newPos);
};

}

#endif // MULTISPINBOX_H
