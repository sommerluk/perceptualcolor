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
#include <QLineEdit>
#include <QStyle>
#include <QStyleOption>

namespace PerceptualColor {

/** @brief Test if a cursor position is at the current value.
 * 
 * Everything from the cursor position exactly before the value itself up
 * to the cursor position exactly after the value itself. Example: “ab12cd”
 * (prefix “ab”, value 12, suffix “cd”). The cursor positions 2, 3 and 4 are
 * <em>at</em> the current value.
 * 
 * @param cursorPosition the cursor position to test
 * 
 * @returns <tt>true</tt> if the indicated cursor position is at the
 * value text of the current section. <tt>false</tt> otherwise. */
bool MultiSpinBox::isCursorPositionAtCurrentValueText(
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

/** @brief Applies a @ref Section configuration to a <tt>QDoubleSpinBox</tt>.
 * 
 * Applies @ref Section::minimum, @ref Section::maximum and
 * @ref Section::value, but ignores the other configuration settings.
 * 
 * @param section the section configuration that will be applied
 * @param spinBox pointer to the spin box that will get configured
 *
 * @sa @ref applySectionConfigurationToFormatSpinbox
 */
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
}

/** @brief Applies a @ref Section configuration
 * to @ref m_formatSpinBoxForCurrentValue.
 * 
 * Convenience function for @ref applySectionConfiguration that applies
 * the changes directly to @ref m_formatSpinBoxForCurrentValue.
 * 
 * @param index The index in @ref m_configuration that will be applied.
 * That must be a valid index; otherwise the function will throw an
 * exception. */
void MultiSpinBox::applySectionConfigurationToFormatSpinbox(int index)
{
    if (!Helper::inRange(0, index, m_configuration.count() - 1)) {
        qWarning()
            << "The function"
            << __func__
            // << "in file"
            // << __FILE__
            // << "near to line"
            // << __LINE__
            << "was called with the invalid “index“ argument"
            << index
            << "while valid range is currently"
            << 0
            << "‥"
            << m_configuration.count() - 1;
        throw 0;
    }
    applySectionConfiguration(m_configuration[index], &m_formatSpinBoxForCurrentValue);
}

/** @brief The recommended minimum size for the widget
 * 
 * Reimplemented from base class.
 * 
 * @note The minimum size of the widget is the same as @ref sizeHint(). This
 * behavior is different from <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt>
 * that have a minimum size hint that allows for displaying only prefix and
 * value, but not the suffix. However, such a behavior does not seem
 * appropriate for a @ref MultiSpinBox because it could be confusing, given
 * that its content is more complex.
 * 
 * @returns the recommended minimum size for the widget */
QSize MultiSpinBox::minimumSizeHint() const
{
    return sizeHint();
}

/** @brief Initializes a <tt>QDoubleSpinBox</tt>.
 * 
 * The widget will get explicitly hidden, and the number of decimals
 * will be configured.
 * 
 * @param spinBox the <tt>QDoubleSpinBox</tt> that will be initialized
 */
void MultiSpinBox::initializeQDoubleSpinBox(QDoubleSpinBox *spinBox)
{
    // Make sure that the widget is explicitly hidden, so that it will
    // never show up unintentionally.
    spinBox->hide();
    // Define the number of visible decimals.
    spinBox->setDecimals(0);
}

/** @brief Constructor
 * 
 * @param parent the parent widget */
MultiSpinBox::MultiSpinBox(QWidget *parent)
: QAbstractSpinBox(parent)
{    
    // Set up the format spin boxes
    initializeQDoubleSpinBox(&m_formatSpinBoxForCurrentValue);
    
    // Connect signals and slots
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

    // Initialize the configuration (default: only one section)
    m_configuration.append(Section());
    setCurrentSectionIndex(0);
}

/** @brief The recommended size for the widget
 * 
 * Reimplemented from base class.
 * 
 * @returns the size hint
 * 
 * @sa @ref minimumSizeHint() */
QSize MultiSpinBox::sizeHint() const
{
    ensurePolished();

    const QFontMetrics myFontMetrics(fontMetrics());
    SectionList myConfiguration = m_configuration;
    int height = lineEdit()->sizeHint().height();
    int width = 0;
    QString textOfMinimumValue;
    QString textOfMaximumValue;
    QString completeString;
    // Create and initialize a QDoubleSpinBox for the format work. It would
    // reduce the overhead not to create this object on the stack each time
    // when calling this function. But as this function is re-implemented
    // from the base class and must be const, we cannot simply use
    // m_formatSpinBoxForFreeUsage, and we are not sure if the the
    // possible performance gain woud be worth to make a const_cast.
    QDoubleSpinBox formatBox;
    initializeQDoubleSpinBox(&formatBox);

    // Get the text for all the sections
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
            myFontMetrics.horizontalAdvance(textOfMinimumValue)
                > myFontMetrics.horizontalAdvance(textOfMaximumValue)
        ) {
            completeString += textOfMinimumValue;
        } else {
            completeString += textOfMaximumValue;
        }
        // Suffix
        completeString += myConfiguration.at(i).suffix;
    }
    // Add some extra space
    completeString += QStringLiteral(" ");

    // Calculate string width and add two extra pixel for cursor
    // blinking space.
    width = myFontMetrics.horizontalAdvance(completeString) + 2;
    QStyleOptionSpinBox myStyleOptionsForSpinBoxes;
    initStyleOption(&myStyleOptionsForSpinBoxes);
    QSize contentSize(width, height);
    return style()->sizeFromContents(
        QStyle::CT_SpinBox,
        &myStyleOptionsForSpinBoxes,
        contentSize,
        this
    ).expandedTo(QApplication::globalStrut());
}

/** @brief Updates prefix, value and suffix text 
 * 
 * Updates @ref m_currentSectionTextBeforeValue,
 * @ref m_currentSectionTextOfTheValue,
 * @ref m_currentSectionTextAfterValue
 * to the correct values based on @ref m_currentSectionIndex. */
void MultiSpinBox::updatePrefixValueSuffixText()
{
    int i;
    
    // Update m_currentSectionTextBeforeValue
    m_currentSectionTextBeforeValue = QString();
    for (i = 0; i < m_currentSectionIndex; ++i) {
        m_currentSectionTextBeforeValue.append(m_configuration.at(i).prefix);
        applySectionConfigurationToFormatSpinbox(i);
        m_currentSectionTextBeforeValue.append(m_formatSpinBoxForCurrentValue.cleanText());
        m_currentSectionTextBeforeValue.append(m_configuration.at(i).suffix);
    }
    m_currentSectionTextBeforeValue.append(
        m_configuration.at(m_currentSectionIndex).prefix
    );
    
    // Update m_currentSectionTextOfTheValue
    applySectionConfigurationToFormatSpinbox(m_currentSectionIndex);
    m_currentSectionTextOfTheValue = m_formatSpinBoxForCurrentValue.textFromValue(
        m_configuration.at(m_currentSectionIndex).value
    );
    
    // Update m_currentSectionTextAfterValue
    m_currentSectionTextAfterValue = QString();
    m_currentSectionTextAfterValue.append(
        m_configuration.at(m_currentSectionIndex).suffix
    );
    for (i = m_currentSectionIndex + 1; i < m_configuration.count(); ++i) {
        m_currentSectionTextAfterValue.append(m_configuration.at(i).prefix);
        applySectionConfigurationToFormatSpinbox(i);
        m_currentSectionTextAfterValue.append(m_formatSpinBoxForCurrentValue.cleanText());
        m_currentSectionTextAfterValue.append(m_configuration.at(i).suffix);
    }
    
    // We have abused the format spinbox, that should normally always
    // have the CURRENT format. Here we restore it:
    applySectionConfigurationToFormatSpinbox(m_currentSectionIndex);
}

/** @brief Sets the current section index.
 *
 * Updates the text in the QLineEdit of this widget. If the widget has focus,
 * it also selects the value of the new current section.
 * 
 * @param index The index of the new current section. Must be a valid index.
 * 
 * @sa @ref setCurrentSectionIndexWithoutUpdatingText */
void MultiSpinBox::setCurrentSectionIndex(int index)
{
    QSignalBlocker myBlocker(lineEdit());
    setCurrentSectionIndexWithoutUpdatingText(index);
    // Update the line edit widget
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

/** @brief Sets the current section index without updating
 * the <tt>QLineEdit</tt>.
 *
 * Does not change neither the text nor the cursor in the <tt>QLineEdit</tt>.
 * 
 * @param index The index of the new current section. Must be a valid index.
 * 
 * @sa @ref setCurrentSectionIndex */
void MultiSpinBox::setCurrentSectionIndexWithoutUpdatingText(int index)
{
    if (!Helper::inRange(0, index, m_configuration.count() - 1)) {
        qWarning()
            << "The function"
            << __func__
            // << "in file"
            // << __FILE__
            // << "near to line"
            // << __LINE__
            << "was called with an invalid “index“ argument of"
            << index
            << "while valid range is currently"
            << 0
            << "‥"
            << m_configuration.count() - 1;
        throw 0;
    }

    if (index == m_currentSectionIndex) {
        // There is nothing to do here.
        return;
    }

    // Apply the changes
    m_currentSectionIndex = index;
    updatePrefixValueSuffixText();

    // Clean up, so that the current state of m_formatSpinBox
    // corresponds to m_currentSectionIndex.
    applySectionConfigurationToFormatSpinbox(m_currentSectionIndex);

    // The state (enabled/disabled) of the buttons “Step up” and “Step down”
    // has to be updated. To force this, update() is called manually here:
    update();
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
    
    // Test is step up should be enabled…
    if (currentSection.value < currentSection.maximum) {
        flags.setFlag(StepUpEnabled, true);
    }
    
    // Test is step down should be enabled…
    if (currentSection.value > currentSection.minimum) {
        flags.setFlag(StepDownEnabled, true);
    }
    
    // Return
    return flags;
}

/** @brief Set a configuration.
 * 
 * @param configuration The new configuration. Each section must have valid
 * values: <tt>@ref Section.minimum ≤ @ref Section.value ≤
 * @ref Section.maximum </tt> If the values are not valid, they will be
 * adjunsted silently.
 * 
 * @sa @ref configuration()
 */
void MultiSpinBox::setConfiguration(
    const MultiSpinBox::SectionList &configuration
)
{
    if (configuration.count() < 1) {
        return;
    }

    // Make sure the configuration is valid (minimum <= value <= maximum)
    m_configuration.clear();
    Section tempSection;
    for (int i = 0; i < configuration.count(); ++i) {
        tempSection = configuration.at(i);
        if (tempSection.maximum < tempSection.minimum) {
            tempSection.maximum = tempSection.minimum;
        }
        if (tempSection.value < tempSection.minimum) {
            tempSection.value = tempSection.minimum;
        }
        if (tempSection.value > tempSection.maximum) {
            tempSection.value = tempSection.maximum;
        }
        m_configuration.append(tempSection);
    }
    setCurrentSectionIndex(0);

    // Make sure that the buttons for step up and step down are updated.
    update();
}

/** @brief Provides the current configuration.
 * 
 * @returns the current configuration
 * 
 * @sa @ref setConfiguration() */
MultiSpinBox::SectionList MultiSpinBox::configuration() const
{
    return m_configuration;
}

/** @brief Focus handling for <em>Tab</em> respectively <em>Shift+Tab</em>.
 * 
 * Reimplemented from base class.
 * 
 * If it’s about moving the focus <em>within</em> this widget, the focus move
 * is actually done. If it’s about moving the focus to <em>another</em>
 * widget, the focus move is <em>not</em> actually done.
 * 
 * @note The documentation of the base class is not very detailed. This
 * reimplementation does not exactly behave as the documentation of the
 * base class suggests. Especially, it handles directly the focus move
 * <em>within</em> the widget itself. This was, however, the only working
 * solution we found.
 * 
 * @param next <tt>true</tt> stands for focus handling for <em>Tab</em>.
 * <tt>false</tt> stands for focus handling for <em>Shift+Tab</em>.
 * 
 * @returns <tt>true</tt> if the focus has actually been moved within
 * this widget or if a move to another widget is possible. <tt>false</tt>
 * otherwise. */
bool MultiSpinBox::focusNextPrevChild(bool next)
{
    if (next == true) { // Move focus forward (Tab)
        if (m_currentSectionIndex < (m_configuration.count() - 1)) {
qDebug() << "1: m_currentSectionIndex is" << m_currentSectionIndex;
            setCurrentSectionIndex(m_currentSectionIndex + 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
qDebug() << "1: m_currentSectionIndex is now" << m_currentSectionIndex;
            return true;
        }
    } else { // Move focus backward (Shift+Tab)
        if (m_currentSectionIndex > 0) {
qDebug() << "2: m_currentSectionIndex is" << m_currentSectionIndex;
            setCurrentSectionIndex(m_currentSectionIndex - 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
qDebug() << "2: m_currentSectionIndex is" << m_currentSectionIndex;
            return true;
        }
    }


qDebug() << "3: m_currentSectionIndex is" << m_currentSectionIndex;
    // Make sure that the buttons for step up and step down are updated.
    update();
qDebug() << "3: m_currentSectionIndex is now" << m_currentSectionIndex;
    return QWidget::focusNextPrevChild(next);
}

/** @brief Handles a <tt>QEvent::FocusOut</tt>.
 * 
 * Reimplemented from base class.
 * 
 * Updates the widget (except for windows that do not
 * specify a <tt>focusPolicy()</tt>).
 * 
 * @param event the <tt>QEvent::FocusOut</tt> to be handled. */
void MultiSpinBox::focusOutEvent(QFocusEvent* event)
{
    QAbstractSpinBox::focusOutEvent(event);
    switch (event->reason()) {
        case Qt::ShortcutFocusReason:
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
        case Qt::MouseFocusReason:
            setCurrentSectionIndexWithoutUpdatingText(0);
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
            update();
            return;
    }
}

/** @brief Handles a <tt>QEvent::FocusIn</tt>.
 * 
 * Reimplemented from base class.
 * 
 * Updates the widget (except for windows that do not
 * specify a <tt>focusPolicy()</tt>).
 * 
 * @param event the <tt>QEvent::FocusIn</tt> to be handled. */
void MultiSpinBox::focusInEvent(QFocusEvent* event)
{
    QAbstractSpinBox::focusInEvent(event);
    switch (event->reason()) {
        // TODO Why is is necessary to setCurrentSectionIndex(0) for
        // Qt::ShortcutFocusReason? It should work without that! But a unit
        // test fails then…
        case Qt::ShortcutFocusReason:
        case Qt::TabFocusReason:
            qDebug() << "index before:" << m_currentSectionIndex;
            setCurrentSectionIndex(0);
            qDebug() << "index after:" << m_currentSectionIndex;
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
            update();
            return;
    }
}

// TODO xxx

void MultiSpinBox::stepBy(int steps)
{
    // TODO support for wrapping…
    m_configuration[m_currentSectionIndex].value = qBound<double>(
        m_configuration[m_currentSectionIndex].minimum,
        m_configuration[m_currentSectionIndex].value + steps,
        m_configuration[m_currentSectionIndex].maximum
    ); // TODO Update QLineEdit!?
    // Select the current value (cursor text selection)
    setCurrentSectionIndex(m_currentSectionIndex);
    // Make sure that the buttons for step up and step down are updated.
    update();
}

void MultiSpinBox::updateValueFromText(const QString &text)
{
    // Get the clean test. That means, we start with “text”, but
    // we remove the m_currentSectionTextBeforeValue and the
    // m_currentSectionTextAfterValue, so that only the text of
    // the value itself remains.
    QString cleanText = text;
    if (cleanText.startsWith(m_currentSectionTextBeforeValue)) {
        cleanText.remove(0, m_currentSectionTextBeforeValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error. Fall back to an empty string.
        cleanText.clear();
    }
    if (cleanText.endsWith(m_currentSectionTextAfterValue)) {
        cleanText.chop(m_currentSectionTextAfterValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error. Fall back to an empty string.
        cleanText.clear();
    }

    // Update…
    m_configuration[m_currentSectionIndex].value =
        m_formatSpinBoxForCurrentValue.valueFromText(cleanText);
}

/** @brief Updates @ref m_currentSectionIndex according to the new cursor
 * position.
 * 
 * This slot works well in connection to the signal
 * <tt>QLineEdit::cursorPositionChanged()</tt>.
 * 
 * @param oldPos the old cursor position (previous position)
 * @param newPos the new cursor position (current position) */
void MultiSpinBox::updateSectionFromCursorPosition(
    const int oldPos,
    const int newPos
)
{
    Q_UNUSED(oldPos);
    
    QSignalBlocker myBlocker(lineEdit());
    int reference = 0;
    int i;

    if (isCursorPositionAtCurrentValueText(newPos)) {
        // We are within the value text of our current section value.
        // There is nothing to do here.
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
        // We abuse m_formatSpinBoxForCurrentValue, which should normally have
        // always the format of the CURRENT section. But we will restore
        // it later…
        applySectionConfigurationToFormatSpinbox(i);
        reference += m_formatSpinBoxForCurrentValue.cleanText().length();
        reference += m_configuration.at(i).suffix.length();
        if (newPos <= reference) {
            break;
        }
    }
    // Restore m_formatSpinBoxForCurrentValue to the correct format
    applySectionConfigurationToFormatSpinbox(m_currentSectionIndex);
    setCurrentSectionIndexWithoutUpdatingText(i);

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

}
