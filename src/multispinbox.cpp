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

#include "PerceptualColor/extendeddoublevalidator.h"
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

/** @brief Constructor
 * 
 * @param parent the parent widget */
MultiSpinBox::MultiSpinBox(QWidget *parent)
: QAbstractSpinBox(parent)
{
    // Set up the m_validator
    m_validator = new ExtendedDoubleValidator(this);
    m_validator->setLocale(locale());
    // TODO m_validator has to be initialized, but calling just
    // setCurrentSectionIndex() wouldn’t be enough because if the
    // new section is not different from the old one, nothing happens.
    lineEdit()->setValidator(m_validator);
    
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
    // This function intentionally does not cache the text string.
    // Which variant is the longest text string, that depends on the current
    // font policy. This might have changed since the last call. Therefore,
    // each time this functin is called, we calculate again the longest
    // test string (“completeString”).

    ensurePolished();

    const QFontMetrics myFontMetrics(fontMetrics());
    SectionList myConfiguration = m_configuration;
    int height = lineEdit()->sizeHint().height();
    int width = 0;
    QString textOfMinimumValue;
    QString textOfMaximumValue;
    QString completeString;

    // Get the text for all the sections
    for (int i = 0; i < myConfiguration.count(); ++i) {
        // Prefix
        completeString += myConfiguration.at(i).prefix;
        // For each section, test if the minimum value or the maximum
        // takes more space (width). Choose the one that takes more place
        // (width).
        textOfMinimumValue = locale().toString(
            myConfiguration.at(i).minimum,
            'f',
            myConfiguration.at(i).decimals
        );
        textOfMaximumValue = locale().toString(
            myConfiguration.at(i).maximum,
            'f',
            myConfiguration.at(i).decimals
        );
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
    
    // Add some extra space, just as QSpinBox seems to do also.
    completeString += QStringLiteral(u" ");

    // Calculate string width and add two extra pixel for cursor
    // blinking space.
    width = myFontMetrics.horizontalAdvance(completeString) + 2;

    // Calculate the final size in pixel
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

/** @brief Formats the value of a given section.
 * 
 * @param mySection the section that will be formatted
 * @returns the value, formatted (without prefix or suffix), as text */
QString MultiSpinBox::formattedValue(const Section &mySection) const {
    return locale().toString(
        mySection.value,
        'f',
        mySection.decimals
    );
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
        m_currentSectionTextBeforeValue.append(
            formattedValue(m_configuration.at(i))
        );
        m_currentSectionTextBeforeValue.append(m_configuration.at(i).suffix);
    }
    m_currentSectionTextBeforeValue.append(
        m_configuration.at(m_currentSectionIndex).prefix
    );
    
    // Update m_currentSectionTextOfTheValue
    m_currentSectionTextOfTheValue = formattedValue(
        m_configuration.at(m_currentSectionIndex)
    );
    
    // Update m_currentSectionTextAfterValue
    m_currentSectionTextAfterValue = QString();
    m_currentSectionTextAfterValue.append(
        m_configuration.at(m_currentSectionIndex).suffix
    );
    for (i = m_currentSectionIndex + 1; i < m_configuration.count(); ++i) {
        m_currentSectionTextAfterValue.append(m_configuration.at(i).prefix);
        
        m_currentSectionTextAfterValue.append(
            formattedValue(m_configuration.at(i))
        );
        m_currentSectionTextAfterValue.append(m_configuration.at(i).suffix);
    }
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
    // Make sure that the buttons for step up and step down are updated.
    update();
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
    m_validator->setPrefix(m_currentSectionTextBeforeValue);
    m_validator->setSuffix(m_currentSectionTextAfterValue);
    m_validator->setRange(
        m_configuration.at(m_currentSectionIndex).minimum,
        m_configuration.at(m_currentSectionIndex).maximum
    );

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
    updatePrefixValueSuffixText();
    lineEdit()->setText(
        m_currentSectionTextBeforeValue
            + m_currentSectionTextOfTheValue
            + m_currentSectionTextAfterValue
    );
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
            update();
            return;
    }
}

/** @brief Increase or decrese the current section’s value.
 * 
 * Reimplemented from base class.
 * 
 * As of the base class’s documentation:
 * 
 * > Virtual function that is called whenever the user triggers a step. 
 * > For example, pressing <tt>Qt::Key_Down</tt> will trigger a call
 * > to <tt>stepBy(-1)</tt>, whereas pressing <tt>Qt::Key_PageUp</tt> will
 * > trigger a call to <tt>stepBy(10)</tt>.
 * 
 * The step size in this function is <em>always</em> <tt>1</tt>. Therefore,
 * calling <tt>stepBy(1)</tt> will increase the current section’s value
 * by <tt>1</tt>; no additional factor is applied.
 * 
 * @param steps The <em>steps</em> parameter indicates how many steps were
 * taken. A positive step count increases the value, a negative step count
 * decreases it. */
void MultiSpinBox::stepBy(int steps)
{
    // As explained in QAbstractSpinBox documentation:
    //    “Note that this function is called even if the resulting value will
    //     be outside the bounds of minimum and maximum. It's this function's
    //     job to handle these situations.”
    // Therefore, the result is bound to the actual minimum and maximum
    // values:
    m_configuration[m_currentSectionIndex].value = qBound<double>(
        m_configuration.at(m_currentSectionIndex).minimum,
        m_configuration.at(m_currentSectionIndex).value + steps,
        m_configuration.at(m_currentSectionIndex).maximum
    );
    // Update the internal representation
    updatePrefixValueSuffixText();
    // Update the content of the QLineEdit and select the current
    // value (as cursor text selection):
    setCurrentSectionIndex(m_currentSectionIndex);
    // Make sure that the buttons for step up and step down are updated.
    update();
}

// TODO WARNING startsWith and endsWith have strange behaviour is the argument is empty, but not null.

/** @brief Updates the value of the current section in @ref m_configuration.
 * 
 * This slot is meant to be connected to the
 * <tt>&QLineEdit::textChanged()</tt> signal of
 * the <tt>MultiSpinBox::lineEdit()</tt> child widget.
 * ,
 * @param lineEditText The text of the <tt>lineEdit()</tt>. The value
 * will be updated according to this parameter. Only changes in
 * the <em>current</em> section’s value are expected, no changes in
 * other sectins. (If this parameter has an ivalid value, a warning will
 * be printed to stderr and the function returns without further action.) */
void MultiSpinBox::updateValueFromText(const QString &lineEditText)
{
    // Get the clean test. That means, we start with “text”, but
    // we remove the m_currentSectionTextBeforeValue and the
    // m_currentSectionTextAfterValue, so that only the text of
    // the value itself remains.
    QString cleanText = lineEditText;
    if (cleanText.startsWith(m_currentSectionTextBeforeValue)) {
        cleanText.remove(0, m_currentSectionTextBeforeValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning()
            << "The function"
            << __func__
            // << "in file"
            // << __FILE__
            // << "near to line"
            // << __LINE__
            << "was called with the invalid “lineEditText“ argument “"
            << lineEditText
            << "” that does not start with the expected character sequence “"
            << m_currentSectionTextBeforeValue
            << ". The call is ignored.";
        return;
    }
    if (cleanText.endsWith(m_currentSectionTextAfterValue)) {
        cleanText.chop(m_currentSectionTextAfterValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning()
            << "The function"
            << __func__
            // << "in file"
            // << __FILE__
            // << "near to line"
            // << __LINE__
            << "was called with the invalid “lineEditText“ argument “"
            << lineEditText
            << "” that does not end with the expected character sequence “"
            << m_currentSectionTextAfterValue
            << ". The call is ignored.";
        return;
    }

    // Update…
    bool ok;
    m_configuration[m_currentSectionIndex].value =
        locale().toDouble(cleanText, &ok);
    // TODO What to do if ok has returned false?
    // Make sure that the buttons for step up and step down are updated.
    update();
    // The lineEdit()->text() property is intentionally not updated because
    // this function is ment to receive signals of the very same lineEdit().
}

/** @brief Updates @ref m_currentSectionIndex according to the new cursor
 * position.
 * 
 * This slot is meant to be connected to the
 * <tt>QLineEdit::cursorPositionChanged()</tt> signal of
 * the <tt>MultiSpinBox::lineEdit()</tt> child widget.
 * 
 * @param oldPos the old cursor position (previous position)
 * @param newPos the new cursor position (current position) */
void MultiSpinBox::updateSectionFromCursorPosition(
    const int oldPos,
    const int newPos
)
{
    // This slot is meant to be connected to the
    // QLineEdit::cursorPositionChanged() signal of
    // the MultiSpinBox::lineEdit() child widget.
    // This signal emits the two “int” parameters “oldPos”
    // and “newPos”. We only need the second one, but we have
    // to take both of them as parameter if we want to stay
    // compatible. Therefore, we mark the first one
    // with Q_UNUSED to avoid compiler warnings.
    Q_UNUSED(oldPos);

    if (isCursorPositionAtCurrentValueText(newPos)) {
        // We are within the value text of our current section value.
        // There is nothing to do here.
        return;
    }

    QSignalBlocker myBlocker(lineEdit());
    int reference = 0;
    int sectionOfTheNewCursorPosition;
    
    // The new position is not at the current value, but the old one might
    // have been. So maybe we have to correct the value, which might change
    // its length. If the new cursor position is after this value, it will
    // have to be adapted (if the value had been changed or alternated).
    const int oldTextLength = lineEdit()->text().length();
    const bool cursorPositionHasToBeAdaptedToTextLenghtChange = (
        newPos > (oldTextLength - m_currentSectionTextAfterValue.length())
    );

    // Calculat in which section the cursor is
    for (
        sectionOfTheNewCursorPosition = 0;
        sectionOfTheNewCursorPosition < m_configuration.count() - 1;
        ++sectionOfTheNewCursorPosition
    ) {
        reference +=
            m_configuration.at(sectionOfTheNewCursorPosition).prefix.length();
        reference +=
            formattedValue(
                m_configuration.at(sectionOfTheNewCursorPosition)
            ).length(); // TODO length(), size(), count() are counting code units. Is the cursor position also calculated in code units or rather in real code points?
        reference +=
            m_configuration.at(sectionOfTheNewCursorPosition).suffix.length();
        if (newPos <= reference) {
            break;
        }
    }

    setCurrentSectionIndexWithoutUpdatingText(
        sectionOfTheNewCursorPosition
    );
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
