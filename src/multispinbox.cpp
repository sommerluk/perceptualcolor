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

// Own header
#include "PerceptualColor/multispinbox.h"

#include "PerceptualColor/helper.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStyle>
#include <QStyleOption>

namespace PerceptualColor {

/** @param cursorPosition the cursor position to test
 * @returns <tt>true</tt> if the indicated cursor position is at the
 * value text of the current section (everything from the cursor position
 * exactly before the first character to the cursor position exactly after
 * the last character counts). Otherwise <tt>false</tt>. */
bool MultiSpinBox::cursorPositionIsAtCurrentValueText(
    const int cursorPosition
) const
{
    const bool newPositionIsHighEnough = (
        cursorPosition >= m_currentSectionTextBeforeValue.length()
    );
    const bool newPositionIsLowEnough = (
        cursorPosition <= 
        (lineEdit()->text().length() - m_currentSectionTextAfterValue.length())
    );
    return (newPositionIsHighEnough && newPositionIsLowEnough);
}

void MultiSpinBox::applySectionConfiguration(
    PerceptualColor::MultiSpinBox::Section section,
    QDoubleSpinBox* spinBox
)
{
    spinBox->setMaximum(
        section.maximum
    );
    spinBox->setMinimum(
        section.minimum
    );
    spinBox->setValue(
        section.value
    );
    
    // TODO This does not work (parameter is not a reference).
    // Re-apply this to the original configuration.
    // If the configuration as invalid, for example a value that was out
    // of range minimum-maximum, than the corrections made in the QSpinBox
    // are also applied to the original configuration.
    section.maximum = spinBox->maximum();
    section.minimum = spinBox->minimum();
    section.value = spinBox->value();
}


/** @brief Apply a section configuration to the format spin box.
 * 
 * Writes back to the configuration eventual corrections.
 * @param index the section configuration that will be applied (and,
 * if appropriate, corrected). */
void MultiSpinBox::applySectionConfigurationToFormatSpinbox(int index)
{
    if (!Helper::inRange(0, index, m_configuration.count() - 1)) {
        qWarning()
            << "applySectionConfigurationToFormatSpinbox called with"
                " invalid index"
            << index
            << "while valid range is currently"
            << 0
            << "–"
            << m_configuration.count() - 1;
        return;
    }
    applySectionConfiguration(m_configuration[index], &m_formatSpinBox);
}

int MultiSpinBox::currentSectionIndex() const
{
    return m_currentSectionIndex;
}

/** @brief The recommended minimum size for the widget
 * 
 * Reimplemented from base class.
 * 
 * The minimum size of the widget is the same as @ref sizeHint(). (This is
 * different from QSpinBox and QDoubleSpinBox that have a minimum size hint
 * that allows for displaying only prefix and value, but not the suffix.
 * This does not seem appropriate for a @ref MultiSpinBox because it could
 * be confusing, given that its content is more complex anyway.)
 * 
 * @returns the recommended minimum size for the widget */
QSize MultiSpinBox::minimumSizeHint() const
{
    return sizeHint();
}

void MultiSpinBox::initializeQDoubleSpinBox(QDoubleSpinBox *spinBox)
{
    spinBox->hide();
    spinBox->setDecimals(0);
}

/** @brief Constructor */
MultiSpinBox::MultiSpinBox(QWidget *parent)
: QAbstractSpinBox(parent)
{    
    // Set up the format spin box
    initializeQDoubleSpinBox(&m_formatSpinBox);
    
    connect(
        lineEdit(),
        &QLineEdit::textChanged,
        this,
        &MultiSpinBox::updateValueFromText
    );
    connect(
        lineEdit(),
        &QLineEdit::cursorPositionChanged,
        this,
        &MultiSpinBox::updateSectionFromCursorPosition
    );
    
    m_configuration.append(Section());
    setCurrentSectionIndex(0);
}

void MultiSpinBox::updateSectionFromCursorPosition(
    const int oldPos,
    const int newPos
)
{
    Q_UNUSED(oldPos);
    
    QSignalBlocker myBlocker(lineEdit());
    int reference = 0;
    int i;

    if (cursorPositionIsAtCurrentValueText(newPos)) {
        // We are within the value text of our current section value.
        // Nothing to do.
        return;
    }
    
    // The new position is not at the current value, but the old one might
    // have been. So maybe we have to correct the value, which might change
    // its length. If the new cursor position is after this value, it will
    // have to be adapted.
    const int oldTextLength = lineEdit()->text().length();
    const bool cursorPositionHasToBeAdaptedToTextLenghtChange = (
        newPos >
            (oldTextLength - m_currentSectionTextAfterValue.length())
    );

    for (i = 0; i < m_configuration.count() - 1; ++i) {
        reference += m_configuration.at(i).prefix.length();
        applySectionConfigurationToFormatSpinbox(i);
        reference += m_formatSpinBox.cleanText().length();
        reference += m_configuration.at(i).suffix.length();
        if (newPos <= reference) {
            break;
        }
    }

    setCurrentSectionIndexWithoutSelectingText(i);
    lineEdit()->setText(
        m_currentSectionTextBeforeValue
            + m_currentSectionTextOfTheValue
            + m_currentSectionTextAfterValue
    );
    int correctedCursorPosition = newPos;
    if (cursorPositionHasToBeAdaptedToTextLenghtChange) {
        correctedCursorPosition =
            newPos
                + lineEdit()->text().length()
                - oldTextLength;
    }
    lineEdit()->setCursorPosition(correctedCursorPosition);
    
    // Make sure that the buttons for step up and step down are updated.
    update();
}

// TODO The first section is selected even if widget
// had still no focus (when layout is not expanded).
// That should not be like that.

QSize MultiSpinBox::sizeHint() const
{
    ensurePolished();
    const QFontMetrics fm(fontMetrics());
    SectionList myConfiguration = m_configuration;
    int height = lineEdit()->sizeHint().height();
    int width = 0;
    
    QString textOfMinimumValue;
    QString textOfMaximumValue;
    QString completeString;
    QDoubleSpinBox formatBox;
    initializeQDoubleSpinBox(&formatBox);
    for (int i = 0; i < myConfiguration.count(); ++i) {
        // Prefix
        completeString += myConfiguration.at(i).prefix;
        // For each section, test if the minimum value or the maximum
        // takes more space (width). Choose the one that takes more place
        // (width).
        myConfiguration[i].value = myConfiguration.at(i).minimum;
        applySectionConfiguration(myConfiguration.at(i), &formatBox);
        textOfMinimumValue = formatBox.cleanText();
        myConfiguration[i].value = myConfiguration.at(i).maximum;
        applySectionConfiguration(myConfiguration.at(i), &formatBox);
        textOfMaximumValue = formatBox.cleanText();
        if (
            fm.horizontalAdvance(textOfMinimumValue)
                > fm.horizontalAdvance(textOfMaximumValue)
        ) {
            completeString += textOfMinimumValue;
        } else {
            completeString += textOfMaximumValue;
        }
        // Suffix
        completeString += myConfiguration.at(i).suffix;
    }
    completeString += QStringLiteral(" ");

    // Calculate string width and add two extra pixel for cursor
    // blinking space.
    width = fm.horizontalAdvance(completeString) + 2;
    QStyleOptionSpinBox opt;
    initStyleOption(&opt);
    QSize hint(width, height);
    return style()->sizeFromContents(
        QStyle::CT_SpinBox,
        &opt,
        hint,
        this
    ).expandedTo(QApplication::globalStrut());
}

void MultiSpinBox::setCurrentSectionIndex(int index)
{
    QSignalBlocker myBlocker(lineEdit());
    setCurrentSectionIndexWithoutSelectingText(index);
    // Update the line edit widget // TODO Is this necessary here?
    lineEdit()->setText(
        m_currentSectionTextBeforeValue
            + m_currentSectionTextOfTheValue
            + m_currentSectionTextAfterValue
    );
    if (hasFocus()) {
        lineEdit()->setSelection(
            m_currentSectionTextBeforeValue.length(),
            m_currentSectionTextOfTheValue.length()
        );
    } else {
        lineEdit()->setCursorPosition(
            m_currentSectionTextBeforeValue.length()
                + m_currentSectionTextOfTheValue.length()
        );
    };
}

/** @brief Virtual function that determines whether stepping up and down is
 * legal at any given time.
 * 
 * Reimplemented from base class.
 * 
 * @returns whether stepping up and down is legal */
QAbstractSpinBox::StepEnabled MultiSpinBox::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled flags;
    const Section currentSection = m_configuration.at(m_currentSectionIndex);
    if (currentSection.value < currentSection.maximum) {
        flags.setFlag(StepUpEnabled, true);
    }
    if (currentSection.value > currentSection.minimum) {
        flags.setFlag(StepDownEnabled, true);
    }
qDebug() << "Will return:" << flags;
    return flags;
}

// TODO Delete this function (has currently only 1 call and can be
// integrated right there).
void MultiSpinBox::getSectionTexts(
    const SectionList &sections,
    const int activeSectionIndex,
    QString &textBeforeValue,
    QString &textOfTheValue,
    QString &textAfterValue
)
{
    if (!Helper::inRange(0, activeSectionIndex, sections.count() - 1)) {
        qWarning()
            << "getSectionTexts called with invalid activeSectionIndex"
            << activeSectionIndex
            << "while valid range is currently"
            << 0
            << "–"
            << sections.count() - 1;
        return;
    }

    int i;
    
    // Update textBeforeValue
    textBeforeValue = QString();
    for (i = 0; i < activeSectionIndex; ++i) {
        textBeforeValue.append(sections.at(i).prefix);
        applySectionConfigurationToFormatSpinbox(i);
        textBeforeValue.append(m_formatSpinBox.cleanText());
        textBeforeValue.append(sections.at(i).suffix);
    }
    textBeforeValue.append(
        sections.at(activeSectionIndex).prefix
    );
    
    // Update textOfTheValue
    applySectionConfigurationToFormatSpinbox(activeSectionIndex);
    textOfTheValue = m_formatSpinBox.textFromValue(
        sections.at(activeSectionIndex).value
    );
    
    // Update textAfterValue
    textAfterValue = QString();
    textAfterValue.append(
        sections.at(activeSectionIndex).suffix
    );
    for (i = activeSectionIndex + 1; i < sections.count(); ++i) {
        textAfterValue.append(sections.at(i).prefix);
        applySectionConfigurationToFormatSpinbox(i);
        textAfterValue.append(m_formatSpinBox.cleanText());
        textAfterValue.append(sections.at(i).suffix);
    }
}
    

/** @brief Sets the current section index and selects the corresponding
 *  value in the QLineEdit. */
void MultiSpinBox::setCurrentSectionIndexWithoutSelectingText(int index)
{
    if (!Helper::inRange(0, index, m_configuration.count() - 1)) {
        qWarning()
            << "setCurrentSectionIndexWithoutSelectingText called with"
                " invalid index"
            << index
            << "while valid range is currently"
            << 0
            << "–"
            << m_configuration.count() - 1;
        return;
    }

    m_currentSectionIndex = index;

    getSectionTexts(
        m_configuration,
        m_currentSectionIndex,
        m_currentSectionTextBeforeValue,
        m_currentSectionTextOfTheValue,
        m_currentSectionTextAfterValue
    );

    // Clean up, so that the current state of m_formatSpinBox
    // corresponds to m_currentSectionIndex.
    applySectionConfigurationToFormatSpinbox(currentSectionIndex());
    // Make sure that the buttons for step up and step down are updated.
    update();
}

void MultiSpinBox::setConfiguration(
    const MultiSpinBox::SectionList &configuration
)
{
    if (configuration.count() < 1) {
        return;
    }
    m_configuration = configuration;

    for (int i = 0; i < configuration.count() - 1; ++i) {
        applySectionConfigurationToFormatSpinbox(i);
    }

    setCurrentSectionIndex(0);
    // Make sure that the buttons for step up and step down are updated.
    update();
}

void MultiSpinBox::stepBy(int steps)
{
    m_configuration[m_currentSectionIndex].value = qBound<double>(
        m_configuration[m_currentSectionIndex].minimum,
        m_configuration[m_currentSectionIndex].value + steps,
        m_configuration[m_currentSectionIndex].maximum
    );
    // Select the current value (cursor text selection)
    setCurrentSectionIndex(m_currentSectionIndex);
    // Make sure that the buttons for step up and step down are updated.
    update();
}

MultiSpinBox::SectionList MultiSpinBox::configuration() const
{
    return m_configuration;
}

bool MultiSpinBox::focusNextPrevChild(bool next)
{
    if (next == true) { // Move focus forward (Tab)
        if (m_currentSectionIndex < (m_configuration.count() - 1)) {
            setCurrentSectionIndex(m_currentSectionIndex + 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    } else { // Move focus backward (Shift+Tab)
        if (m_currentSectionIndex > 0) {
            setCurrentSectionIndex(m_currentSectionIndex - 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    }

    // Make sure that the buttons for step up and step down are updated.
    update();
    return QWidget::focusNextPrevChild(next);
}

void MultiSpinBox::focusInEvent(QFocusEvent* event)
{
    QAbstractSpinBox::focusInEvent(event);
    switch (event->reason()) {
        case Qt::ShortcutFocusReason:
        case Qt::TabFocusReason:
            setCurrentSectionIndex(0);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return;
        case Qt::BacktabFocusReason:
            setCurrentSectionIndex(m_configuration.count() - 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return;
        case Qt::MouseFocusReason:
        case Qt::ActiveWindowFocusReason:
        case Qt::PopupFocusReason:
        case Qt::MenuBarFocusReason:
        case Qt::OtherFocusReason:
        case Qt::NoFocusReason:
        default:
            return;
    }
}

void MultiSpinBox::focusOutEvent(QFocusEvent* event)
{
    QAbstractSpinBox::focusOutEvent(event);
    switch (event->reason()) {
        case Qt::ShortcutFocusReason:
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
        case Qt::MouseFocusReason:
            setCurrentSectionIndexWithoutSelectingText(0);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return;
        case Qt::ActiveWindowFocusReason:
        case Qt::PopupFocusReason:
        case Qt::MenuBarFocusReason:
        case Qt::OtherFocusReason:
        case Qt::NoFocusReason:
        default:
            return;
    }
}

QString MultiSpinBox::cleanTextFromText(const QString &text) const
{
    QString cleanText = text;
    if (cleanText.startsWith(m_currentSectionTextBeforeValue)) {
        cleanText.remove(0, m_currentSectionTextBeforeValue.count());
    } else {
        cleanText.clear();
    }
    if (cleanText.endsWith(m_currentSectionTextAfterValue)) {
        cleanText.chop(m_currentSectionTextAfterValue.count());
    } else {
        cleanText.clear();
    }
    return cleanText;
}

void MultiSpinBox::updateValueFromText(const QString &text)
{
    m_configuration[m_currentSectionIndex].value =
        m_formatSpinBox.valueFromText(cleanTextFromText(text));
}

QValidator::State MultiSpinBox::validate(QString& input, int& pos) const
{
    return QAbstractSpinBox::validate(input, pos);
    return m_formatSpinBox.validate(input, pos);
}

}
