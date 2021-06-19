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

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/multispinbox.h"
// Second, the private implementation.
#include "multispinbox_p.h"

#include "extendeddoublevalidator.h"
#include "helper.h"

#include <QAction>
#include <QApplication>
#include <QLineEdit>
#include <QStyle>
#include <QStyleOption>
#include <QtMath>

namespace PerceptualColor
{
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
 * <em>value</em> text of the current section. <tt>false</tt> otherwise. */
bool MultiSpinBox::MultiSpinBoxPrivate::isCursorPositionAtCurrentSectionValue(const int cursorPosition) const
{
    const bool newPositionIsHighEnough = (cursorPosition >= m_textBeforeCurrentValue.length());
    const bool newPositionIsLowEnough = (cursorPosition <= (q_pointer->lineEdit()->text().length() - m_textAfterCurrentValue.length()));
    return (newPositionIsHighEnough && newPositionIsLowEnough);
}

/** @brief The recommended minimum size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns the recommended minimum size for the widget
 *
 * @internal
 *
 * @sa @ref sizeHint()
 *
 * @note The minimum size of the widget is the same as @ref sizeHint(). This
 * behavior is different from <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt>
 * that have a minimum size hint that allows for displaying only prefix and
 * value, but not the suffix. However, such a behavior does not seem
 * appropriate for a @ref MultiSpinBox because it could be confusing, given
 * that its content is more complex. */
QSize MultiSpinBox::minimumSizeHint() const
{
    return sizeHint();
}

/** @brief Constructor
 *
 * @param parent the parent widget, if any */
MultiSpinBox::MultiSpinBox(QWidget *parent)
    : QAbstractSpinBox(parent)
    , d_pointer(new MultiSpinBoxPrivate(this))
{
    // Set up the m_validator
    d_pointer->m_validator = new ExtendedDoubleValidator(this);
    d_pointer->m_validator->setLocale(locale());
    lineEdit()->setValidator(d_pointer->m_validator);

    // Connect signals and slots
    connect(lineEdit(),                                      // sender
            &QLineEdit::textChanged,                         // signal
            d_pointer.get(),                                 // receiver
            &MultiSpinBoxPrivate::updateCurrentValueFromText // slot
    );
    connect(lineEdit(),                                       // sender
            &QLineEdit::cursorPositionChanged,                // signal
            d_pointer.get(),                                  // receiver
            &MultiSpinBoxPrivate::reactOnCursorPositionChange // slot
    );
    connect(this,                                                                  // sender
            &QAbstractSpinBox::editingFinished,                                    // signal
            d_pointer.get(),                                                       // receiver
            &MultiSpinBoxPrivate::setCurrentIndexToZeroAndUpdateTextAndSelectValue // slot
    );

    // Initialize the configuration (default: only one section)
    d_pointer->m_sectionValues.append(MultiSpinBoxPrivate::defaultSectionValue);
    setSectionConfigurations(QList<SectionConfiguration> {SectionConfiguration()});
    d_pointer->m_currentIndex = -1; // This will force
    // setCurrentIndexAndUpdateTextAndSelectValue()
    // to really apply the changes, including updating
    // the validator:
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);

    // Initialize accessibility support
    QAccessible::installFactory(
        // It’s save to call installFactory() multiple times with the
        // same factory. If the factory is yet installed, it will not
        // be installed again.
        &AccessibleMultiSpinBox::factory);
}

/** @brief Default destructor */
MultiSpinBox::~MultiSpinBox() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
MultiSpinBox::MultiSpinBoxPrivate::MultiSpinBoxPrivate(MultiSpinBox *backLink)
    : q_pointer(backLink)
{
}

/** @brief The recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns the size hint
 *
 * @internal
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
    QList<MultiSpinBox::SectionConfiguration> myConfiguration = d_pointer->m_sectionConfigurations;
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
        textOfMinimumValue = locale().toString(myConfiguration.at(i).minimum, // value
                                               'f',                           // format
                                               myConfiguration.at(i).decimals // precision
        );
        textOfMaximumValue = locale().toString(myConfiguration.at(i).maximum, // value
                                               'f',                           // format
                                               myConfiguration.at(i).decimals // precision
        );
        if (myFontMetrics.horizontalAdvance(textOfMinimumValue) > myFontMetrics.horizontalAdvance(textOfMaximumValue)) {
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
    // Calculate widget size necessary to display a given content
    QSize result = style()
                       ->sizeFromContents(QStyle::CT_SpinBox,          // type
                                          &myStyleOptionsForSpinBoxes, // style options
                                          contentSize,                 // size of the content
                                          this                         // optional widget argument (for better caluclations)
                                          )
                       .expandedTo(QApplication::globalStrut());

    if (d_pointer->m_actionButtonCount > 0) {
        // Determine the size of icons for actions similar to what Qt
        // does in QLineEditPrivate::sideWidgetParameters() and than
        // add this to the size hint.
        //
        // This code leads in general to good results. However, some
        // styles like CDE and Motif calculate badly the size for
        // QAbstractSpinBox and therefore also for this widget.
        //
        // The Kvantum style has a strange behaviour: While behaving
        // correctly on QSpinBox, it does not behave correctly on small
        // widget sizes: On small widget sizes, style()->sizeFromContents()
        // returns a value that is too small, but the actual size of
        // the widget in the layout system will be bigger than the
        // reported size, so this looks okay. On big sizes however,
        // the actual size in the layout system is identical to the
        // reported size, which is too small. For these styles,
        // there seems to be no good work-around. These very same
        // styles also have problems with normal QSpinBox widgets and
        // cut some of the text there, so I suppose this is rather a
        // problem of these styles, and not of our code.
        //
        // TODO That‘s wrong. Normal spinboxes work fine in all CDE,
        // Motif and Kvantum styles. It should work also for MultiSpinBox!
        const int actionButtonIconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, // pixel metric type
                                                              nullptr,                  // style options
                                                              lineEdit()                // widget (optional)
        );
        const int actionButtonMargin = actionButtonIconSize / 4;
        const int actionButtonWidth = actionButtonIconSize + 6;
        // Only 1 margin per button:
        const int actionButtonSpace = actionButtonWidth + actionButtonMargin;
        result.setWidth(result.width() + d_pointer->m_actionButtonCount * actionButtonSpace);
    }

    return result;
}

/** @brief Handle state changes
 *
 * Reimplemented from base class.
 *
 * @param event The event to process */
void MultiSpinBox::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
    case QEvent::LocaleChange:
    // The base class’s implementation for QEvent::LayoutDirectionChange
    // would only call update, not updateGeometry…
    case QEvent::LayoutDirectionChange:
        // Updates the widget content and its geometry
        update();
        updateGeometry();
        break;
    default:
        // QEvent::StyleChange or QEvent::FontChange are not handled here
        // because they trigger yet a content and geometry update in the
        // base class’s implementation of this function.
        break;
    }
    QAbstractSpinBox::changeEvent(event);
}

/** @brief Adds to the widget a button associated with the given action.
 *
 * The icon of the action will be displayed as button. If the action has
 * no icon, just an empty space will be displayed.
 *
 * @image html MultiSpinBoxWithButton.png "MultiSpinBox with action button" width=200
 *
 * It is possible to add more than one action.
 *
 * @param action This action that will be executed when clicking the button.
 * (The parentship of the action object remains unchanged.)
 * @param position The position of the button within the widget (left
 * or right)
 * @note See @ref hidpisupport "High DPI support" about how to enable
 * support for high-DPI icons.
 * @note The action will <em>not</em> appear in the
 * <tt>QWidget::actions()</tt> function of this class. */
void MultiSpinBox::addActionButton(QAction *action, QLineEdit::ActionPosition position)
{
    lineEdit()->addAction(action, position);
    d_pointer->m_actionButtonCount += 1;
    // The size hints have changed, because an additional button needs
    // more space.
    updateGeometry();
}

/** @brief Get formatted value for a given section.
 * @param index The index of the section
 * @returns The value of the given section, formatted (without prefix or
 * suffix), as text. */
QString MultiSpinBox::MultiSpinBoxPrivate::formattedValue(int index) const
{
    return q_pointer->locale().toString(
        // The value to be formatted:
        m_sectionValues.at(index),
        // Format as floating point with decimal digits
        'f',
        // Number of decimal digits
        m_sectionConfigurations.at(index).decimals);
}

/** @brief Updates prefix, value and suffix text
 *
 * Updates @ref m_textBeforeCurrentValue,
 * @ref m_textOfCurrentValue,
 * @ref m_textAfterCurrentValue
 * to the correct values based on @ref m_currentIndex. */
void MultiSpinBox::MultiSpinBoxPrivate::updatePrefixValueSuffixText()
{
    int i;

    // Update m_currentSectionTextBeforeValue
    m_textBeforeCurrentValue = QString();
    for (i = 0; i < m_currentIndex; ++i) {
        m_textBeforeCurrentValue.append(m_sectionConfigurations.at(i).prefix);
        m_textBeforeCurrentValue.append(formattedValue(i));
        m_textBeforeCurrentValue.append(m_sectionConfigurations.at(i).suffix);
    }
    m_textBeforeCurrentValue.append(m_sectionConfigurations.at(m_currentIndex).prefix);

    // Update m_currentSectionTextOfTheValue
    m_textOfCurrentValue = formattedValue(m_currentIndex);

    // Update m_currentSectionTextAfterValue
    m_textAfterCurrentValue = QString();
    m_textAfterCurrentValue.append(m_sectionConfigurations.at(m_currentIndex).suffix);
    for (i = m_currentIndex + 1; i < m_sectionConfigurations.count(); ++i) {
        m_textAfterCurrentValue.append(m_sectionConfigurations.at(i).prefix);

        m_textAfterCurrentValue.append(formattedValue(i));
        m_textAfterCurrentValue.append(m_sectionConfigurations.at(i).suffix);
    }
}

/** @brief Sets the current section index to <tt>0</tt>.
 *
 * Convenience function that simply calls
 * @ref setCurrentIndexAndUpdateTextAndSelectValue with the
 * argument <tt>0</tt>. */
void MultiSpinBox::MultiSpinBoxPrivate::setCurrentIndexToZeroAndUpdateTextAndSelectValue()
{
    setCurrentIndexAndUpdateTextAndSelectValue(0);
}

/** @brief Sets the current section index.
 *
 * Updates the text in the QLineEdit of this widget. If the widget has focus,
 * it also selects the value of the new current section.
 *
 * @param newIndex The index of the new current section. Must be a valid index.
 * Default is <tt>0</tt> (which is always valid as @ref m_sectionConfigurations
 * is guaranteed to contain at least <em>one</em> section). The update will
 * be done even if this argument is identical to the @ref m_currentIndex.
 *
 * @sa @ref setCurrentIndexToZeroAndUpdateTextAndSelectValue
 * @sa @ref setCurrentIndexWithoutUpdatingText */
void MultiSpinBox::MultiSpinBoxPrivate::setCurrentIndexAndUpdateTextAndSelectValue(int newIndex)
{
    QSignalBlocker myBlocker(q_pointer->lineEdit());
    setCurrentIndexWithoutUpdatingText(newIndex);
    // Update the line edit widget
    q_pointer->lineEdit()->setText(m_textBeforeCurrentValue + m_textOfCurrentValue + m_textAfterCurrentValue);
    if (q_pointer->hasFocus()) {
        q_pointer->lineEdit()->setSelection(m_textBeforeCurrentValue.length(), m_textOfCurrentValue.length());
    } else {
        q_pointer->lineEdit()->setCursorPosition(m_textBeforeCurrentValue.length() + m_textOfCurrentValue.length());
    };
    // Make sure that the buttons for step up and step down are updated.
    q_pointer->update();
}

/** @brief Sets the current section index without updating
 * the <tt>QLineEdit</tt>.
 *
 * Does not change neither the text nor the cursor in the <tt>QLineEdit</tt>.
 *
 * @param newIndex The index of the new current section. Must be a valid index.
 *
 * @sa @ref setCurrentIndexAndUpdateTextAndSelectValue */
void MultiSpinBox::MultiSpinBoxPrivate::setCurrentIndexWithoutUpdatingText(int newIndex)
{
    if (!isInRange(0, newIndex, m_sectionConfigurations.count() - 1)) {
        qWarning() << "The function" << __func__                                      //
                   << "in file" << __FILE__                                           //
                   << "near to line" << __LINE__                                      //
                   << "was called with an invalid “newIndex“ argument of" << newIndex //
                   << "thought the valid range is currently [" << 0 << ", " << m_sectionConfigurations.count() - 1 << "]. This is a bug.";
        throw 0;
    }

    if (newIndex == m_currentIndex) {
        // There is nothing to do here.
        return;
    }

    // Apply the changes
    m_currentIndex = newIndex;
    updatePrefixValueSuffixText();
    m_validator->setPrefix(m_textBeforeCurrentValue);
    m_validator->setSuffix(m_textAfterCurrentValue);
    m_validator->setRange(m_sectionConfigurations.at(m_currentIndex).minimum, m_sectionConfigurations.at(m_currentIndex).maximum);

    // The state (enabled/disabled) of the buttons “Step up” and “Step down”
    // has to be updated. To force this, update() is called manually here:
    q_pointer->update();
}

/** @brief Virtual function that determines whether stepping up and down is
 * legal at any given time.
 *
 * Reimplemented from base class.
 *
 * @returns whether stepping up and down is legal */
QAbstractSpinBox::StepEnabled MultiSpinBox::stepEnabled() const
{
    const SectionConfiguration currentSectionConfiguration = d_pointer->m_sectionConfigurations.at(d_pointer->m_currentIndex);
    const double currentSectionValue = d_pointer->m_sectionValues.at(d_pointer->m_currentIndex);

    // When wrapping is enabled, step up and step down are always possible.
    if (currentSectionConfiguration.isWrapping) {
        return QAbstractSpinBox::StepEnabled(StepUpEnabled | StepDownEnabled);
    }

    // When wrapping is not enabled, we have to compare the value with
    // maximum and minimum.
    QAbstractSpinBox::StepEnabled result;
    // Test is step up should be enabled…
    if (currentSectionValue < currentSectionConfiguration.maximum) {
        result.setFlag(StepUpEnabled, true);
    }

    // Test is step down should be enabled…
    if (currentSectionValue > currentSectionConfiguration.minimum) {
        result.setFlag(StepDownEnabled, true);
    }
    return result;
}

/** @brief Fix a given section value
 *
 * @param index The section index
 * @post The value at position <tt>index</tt> of @ref sectionValues is fixed
 * to be conform to @ref SectionConfiguration.minimum,
 * @ref SectionConfiguration.maximum and
 * @ref SectionConfiguration.isWrapping at the given <tt>index</tt>. */
void MultiSpinBox::MultiSpinBoxPrivate::fixSectionValue(int index)
{
    const SectionConfiguration mySectionConfiguration = m_sectionConfigurations.at(index);
    if (mySectionConfiguration.isWrapping) {
        double rangeWidth = mySectionConfiguration.maximum - mySectionConfiguration.minimum;
        if (rangeWidth <= 0) {
            // This is a speciel case.
            // This happens when minimum == maximum (or if minimum > maximum,
            // which is invalid).
            m_sectionValues[index] = mySectionConfiguration.minimum;
        } else {
            qreal temp = fmod(m_sectionValues.at(index) - mySectionConfiguration.minimum, rangeWidth);
            if (temp < 0) {
                temp += rangeWidth;
            }
            m_sectionValues[index] = temp + mySectionConfiguration.minimum;
        }
    } else {
        m_sectionValues[index] = qBound(
            // If there is no wrapping, simply bound:
            mySectionConfiguration.minimum,
            m_sectionValues.at(index),
            mySectionConfiguration.maximum);
    }
}

/** @brief Adds QDebug() support for this data type.
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionConfiguration &value)
{
    dbg.nospace() << "\nMultiSpinBox::SectionConfiguration("  // Opening line
                  << "\n    prefix: " << value.prefix         //
                  << "\n    minimum: " << value.minimum       //
                  << "\n    decimals: " << value.decimals     //
                  << "\n    isWrapping: " << value.isWrapping //
                  << "\n    maximum: " << value.maximum       //
                  << "\n    suffix: " << value.suffix         //
                  << "\n)"                                    // Closing line
        ;
    return dbg.maybeSpace();
}

/** @brief Sets the configuration for the sections.
 *
 * The first section will be selected as current section.
 *
 * @param newSectionConfigurations Defines the new sections. The new section
 * count in this widget is the section count given in this list. Each section
 * should have valid values: <tt>@ref SectionConfiguration.minimum ≤
 * @ref SectionConfiguration.maximum</tt>. If the @ref sectionValues are
 * not valid within the new section configurations, they will be fixed.
 *
 * @sa @ref sectionConfigurations() */
void MultiSpinBox::setSectionConfigurations(const QList<MultiSpinBox::SectionConfiguration> &newSectionConfigurations)
{
    if (newSectionConfigurations.count() < 1) {
        return;
    }

    // Adapt the count of m_value:
    while (d_pointer->m_sectionValues.count() < newSectionConfigurations.count()) {
        // Add elements if there are not enough:
        d_pointer->m_sectionValues.append(MultiSpinBoxPrivate::defaultSectionValue);
    }
    while (d_pointer->m_sectionValues.count() > newSectionConfigurations.count()) {
        // Remove elements if there are too many:
        d_pointer->m_sectionValues.removeLast();
    }
    // Make sure the new SectionConfiguration is valid
    // (minimum <= value <= maximum) before adding it.
    d_pointer->m_sectionConfigurations.clear();
    SectionConfiguration tempSection;
    for (int i = 0; i < newSectionConfigurations.count(); ++i) {
        tempSection = newSectionConfigurations.at(i);
        if (tempSection.maximum < tempSection.minimum) {
            tempSection.maximum = tempSection.minimum;
        }
        d_pointer->m_sectionConfigurations.append(tempSection);
        d_pointer->fixSectionValue(i);
    }
    d_pointer->updatePrefixValueSuffixText();
    lineEdit()->setText(d_pointer->m_textBeforeCurrentValue + d_pointer->m_textOfCurrentValue + d_pointer->m_textAfterCurrentValue);
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);

    // Make sure that the buttons for step up and step down are updated.
    update();

    // Make sure that the geometry is updated: sizeHint() and minimumSizeHint()
    // both depend on the section!
    updateGeometry();
}

/** @brief Returns the configuration of all sections.
 *
 * @returns the configuration of all sections.
 *
 * @sa @ref setSectionConfigurations() */
QList<MultiSpinBox::SectionConfiguration> MultiSpinBox::sectionConfigurations() const
{
    return d_pointer->m_sectionConfigurations;
}

/** @brief A list containing the values of all sections.
 *
 * @returns A list containing the values of all sections.
 *
 * @sa @ref setSectionValues() */
QList<double> MultiSpinBox::sectionValues() const
{
    return d_pointer->m_sectionValues;
}

/** @brief Sets the values for all sections of this widget.
 *
 * The first section will be selected as current section.
 *
 * @param newSectionValues The new section values. This list must have
 * exactly as many items as @ref sectionConfigurations. If the new values
 * are not within the  boundaries defined in the @ref sectionConfigurations,
 * they will be adapted before being applied.
 *
 * @note It is not this function, but @ref sectionConfigurations
 * which determines the actually available count of sections in this widget.
 *
 * @sa @ref sectionValues() */
void MultiSpinBox::setSectionValues(const QList<double> &newSectionValues)
{
    if (newSectionValues.count() < 1) {
        return;
    }

    d_pointer->m_sectionValues = newSectionValues;
    // Adapt the count of m_value:
    while (d_pointer->m_sectionValues.count() < d_pointer->m_sectionConfigurations.count()) {
        // Add elements if there are not enough:
        d_pointer->m_sectionValues.append(MultiSpinBoxPrivate::defaultSectionValue);
    }
    while (d_pointer->m_sectionValues.count() > d_pointer->m_sectionConfigurations.count()) {
        // Remove elements if there are too many:
        d_pointer->m_sectionValues.removeLast();
    }
    // Make sure the new sectionValues are valid (minimum <= value <= maximum):
    for (int i = 0; i < d_pointer->m_sectionConfigurations.count(); ++i) {
        d_pointer->fixSectionValue(i);
    }
    d_pointer->updatePrefixValueSuffixText();
    lineEdit()->setText(d_pointer->m_textBeforeCurrentValue + d_pointer->m_textOfCurrentValue + d_pointer->m_textAfterCurrentValue);
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);

    // Make sure that the buttons for step up and step down are updated.
    update();

    // Make sure that the geometry is updated: sizeHint() and minimumSizeHint()
    // both depend on the section!
    updateGeometry();
}

/** @brief Focus handling for <em>Tab</em> respectively <em>Shift+Tab</em>.
 *
 * Reimplemented from base class.
 *
 * @note If it’s about moving the focus <em>within</em> this widget, the focus
 * move is actually done. If it’s about moving the focus to <em>another</em>
 * widget, the focus move is <em>not</em> actually done.
 * The documentation of the base class is not very detailed. This
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
        if (d_pointer->m_currentIndex < (d_pointer->m_sectionConfigurations.count() - 1)) {
            d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(d_pointer->m_currentIndex + 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    } else { // Move focus backward (Shift+Tab)
        if (d_pointer->m_currentIndex > 0) {
            d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(d_pointer->m_currentIndex - 1);
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    }

    // Make sure that the buttons for step up and step down are updated.
    update();

    // Return
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
void MultiSpinBox::focusOutEvent(QFocusEvent *event)
{
    QAbstractSpinBox::focusOutEvent(event);
    switch (event->reason()) {
    case Qt::ShortcutFocusReason:
    case Qt::TabFocusReason:
    case Qt::BacktabFocusReason:
    case Qt::MouseFocusReason:
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
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
void MultiSpinBox::focusInEvent(QFocusEvent *event)
{
    QAbstractSpinBox::focusInEvent(event);
    switch (event->reason()) {
    case Qt::ShortcutFocusReason:
    case Qt::TabFocusReason:
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        // Make sure that the buttons for step up and step down
        // are updated.
        update();
        return;
    case Qt::BacktabFocusReason:
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(d_pointer->m_sectionConfigurations.count() - 1);
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
    const int currentIndex = d_pointer->m_currentIndex;
    d_pointer->m_sectionValues[currentIndex] += steps * d_pointer->m_sectionConfigurations.at(currentIndex).singleStep;
    // As explained in QAbstractSpinBox documentation:
    //    “Note that this function is called even if the resulting value will
    //     be outside the bounds of minimum and maximum. It’s this function’s
    //     job to handle these situations.”
    // Therefore, the result is bound to the actual minimum and maximum
    // values:
    d_pointer->fixSectionValue(currentIndex);
    // Update the internal representation
    d_pointer->updatePrefixValueSuffixText();
    // Update the content of the QLineEdit and select the current
    // value (as cursor text selection):
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(currentIndex);
    update(); // Make sure the buttons for step-up and step-down are updated.
}

/** @brief Updates the value of the current section.
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
void MultiSpinBox::MultiSpinBoxPrivate::updateCurrentValueFromText(const QString &lineEditText)
{
    // Get the clean test. That means, we start with “text”, but
    // we remove the m_currentSectionTextBeforeValue and the
    // m_currentSectionTextAfterValue, so that only the text of
    // the value itself remains.
    QString cleanText = lineEditText;
    if (cleanText.startsWith(m_textBeforeCurrentValue)) {
        cleanText.remove(0, m_textBeforeCurrentValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning() << "The function" << __func__                                                                         //
                   << "in file" << __FILE__                                                                              //
                   << "near to line" << __LINE__                                                                         //
                   << "was called with the invalid “lineEditText“ argument “" << lineEditText                            //
                   << "” that does not start with the expected character sequence “" << m_textBeforeCurrentValue << ". " //
                   << "The call is ignored. This is a bug.";
        return;
    }
    if (cleanText.endsWith(m_textAfterCurrentValue)) {
        cleanText.chop(m_textAfterCurrentValue.count());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning() << "The function" << __func__                                                                      //
                   << "in file" << __FILE__                                                                           //
                   << "near to line" << __LINE__                                                                      //
                   << "was called with the invalid “lineEditText“ argument “" << lineEditText                         //
                   << "” that does not end with the expected character sequence “" << m_textAfterCurrentValue << ". " //
                   << "The call is ignored. This is a bug.";
        return;
    }

    // Update…
    bool ok;
    m_sectionValues[m_currentIndex] = q_pointer->locale().toDouble(cleanText, &ok);
    fixSectionValue(m_currentIndex);
    updatePrefixValueSuffixText();
    // Make sure that the buttons for step up and step down are updated.
    q_pointer->update();
    // The lineEdit()->text() property is intentionally not updated because
    // this function is ment to receive signals of the very same lineEdit().
}

/** @brief The main event handler.
 *
 * Reimplemented from base class.
 *
 * On <tt>QEvent::Type::LocaleChange</tt> it updates the spinbox content
 * accordingly. Apart from that, it calls the implementation in the parent
 * class.
 *
 * @param event the event to be handled. */
bool MultiSpinBox::event(QEvent *event)
{
    if (event->type() == QEvent::Type::LocaleChange) {
        d_pointer->updatePrefixValueSuffixText();
        d_pointer->m_validator->setPrefix(d_pointer->m_textBeforeCurrentValue);
        d_pointer->m_validator->setSuffix(d_pointer->m_textAfterCurrentValue);
        d_pointer->m_validator->setRange(
            // Minimum
            d_pointer->m_sectionConfigurations.at(d_pointer->m_currentIndex).minimum,
            // Maximum
            d_pointer->m_sectionConfigurations.at(d_pointer->m_currentIndex).maximum);
        lineEdit()->setText(d_pointer->m_textBeforeCurrentValue + d_pointer->m_textOfCurrentValue + d_pointer->m_textAfterCurrentValue);
    }
    return QAbstractSpinBox::event(event);
}

/** @brief Updates the widget according to the new cursor position.
 *
 * This slot is meant to be connected to the
 * <tt>QLineEdit::cursorPositionChanged()</tt> signal of
 * the <tt>MultiSpinBox::lineEdit()</tt> child widget.
 *
 * @param oldPos the old cursor position (previous position)
 * @param newPos the new cursor position (current position) */
void MultiSpinBox::MultiSpinBoxPrivate::reactOnCursorPositionChange(const int oldPos, const int newPos)
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

    // We are working here with QString::length() and
    // QLineEdit::cursorPosition(). Both are of type “int”, and both are
    // measured in UTF-16 code units.  While it feels quite uncomfortable
    // to measure cursor positions in code _units_ and not at least in
    // in code _points_, it does not matter for this code, as the behaviour
    // is consistent between both usages.

    if (isCursorPositionAtCurrentSectionValue(newPos)) {
        // We are within the value text of our current section value.
        // There is nothing to do here.
        return;
    }

    QSignalBlocker myBlocker(q_pointer->lineEdit());

    // The new position is not at the current value, but the old one might
    // have been. So maybe we have to correct the value, which might change
    // its length. If the new cursor position is after this value, it will
    // have to be adapted (if the value had been changed or alternated).
    const int oldTextLength = q_pointer->lineEdit()->text().length();
    const bool cursorPositionHasToBeAdaptedToTextLenghtChange = (newPos > (oldTextLength - m_textAfterCurrentValue.length()));

    // Calculat in which section the cursor is
    int sectionOfTheNewCursorPosition;
    int reference = 0;
    for (sectionOfTheNewCursorPosition = 0; sectionOfTheNewCursorPosition < m_sectionConfigurations.count() - 1; ++sectionOfTheNewCursorPosition) {
        reference += m_sectionConfigurations.at(sectionOfTheNewCursorPosition).prefix.length();
        reference += formattedValue(sectionOfTheNewCursorPosition).length();
        reference += m_sectionConfigurations.at(sectionOfTheNewCursorPosition).suffix.length();
        if (newPos <= reference) {
            break;
        }
    }

    updatePrefixValueSuffixText();
    setCurrentIndexWithoutUpdatingText(sectionOfTheNewCursorPosition);
    q_pointer->lineEdit()->setText(m_textBeforeCurrentValue + m_textOfCurrentValue + m_textAfterCurrentValue);
    int correctedCursorPosition = newPos;
    if (cursorPositionHasToBeAdaptedToTextLenghtChange) {
        correctedCursorPosition = newPos + q_pointer->lineEdit()->text().length() - oldTextLength;
    }
    q_pointer->lineEdit()->setCursorPosition(correctedCursorPosition);

    // Make sure that the buttons for step up and step down are updated.

    q_pointer->update();
}

/** @brief Constructor
 *
 * @param w The widget to which the newly created object will correspond. */
AccessibleMultiSpinBox::AccessibleMultiSpinBox(MultiSpinBox *w)
    : QAccessibleWidget(w, QAccessible::Role::SpinBox)
{
}

/** @brief Destructor */
AccessibleMultiSpinBox::~AccessibleMultiSpinBox()
{
}

/** @brief Factory function.
 *
 * This signature of this function is exactly as defined by
 * <tt>QAccessible::InterfaceFactory</tt>. A pointer to this function
 * can therefore be passed to <tt>QAccessible::installFactory()</tt>.
 *
 * @param classname The class name for which an interface is requested
 * @param object The object for which an interface is requested
 *
 * @returns If this class corresponds to the request, it returns an object
 * of this class. Otherwise, a null-pointer will be returned. */
QAccessibleInterface *AccessibleMultiSpinBox::factory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
    const QString multiSpinBoxClassName = QString::fromUtf8(
        // className() returns const char *. Its encoding is not documented.
        // Hopefully, as we use UTF8  in this library as “input character set”
        // and also as “Narrow execution character set”, the encoding
        // might be also UTF8…
        MultiSpinBox::staticMetaObject.className());
    MultiSpinBox *myMultiSpinBox = qobject_cast<MultiSpinBox *>(object);
    if ((classname == multiSpinBoxClassName) && myMultiSpinBox) {
        interface = new AccessibleMultiSpinBox(myMultiSpinBox);
    }
    return interface;
}

} // namespace PerceptualColor
