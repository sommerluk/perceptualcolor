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

#ifndef MULTISPINBOX_H
#define MULTISPINBOX_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QAbstractSpinBox>
#include <QAction>
#include <QLineEdit>

#include "PerceptualColor/constpropagatinguniquepointer.h"

namespace PerceptualColor
{
/** @brief A spin box that can hold multiple sections (each with its own
 * value) at the same time.
 *
 * This widget is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month, year…) within a single spin box.
 * However, <em>this</em> widget is flexible. You can define on your own
 * the behaviour of each section.
 *
 * @image html MultiSpinBox.png "MultiSpinBox" width=200
 *
 * This widget works with floating point precision. You can set the
 * precision individually by section using @ref SectionConfiguration::decimals.
 * (This value can also be <tt>0</tt> to get integer-like behavoir.)
 *
 * Example code to create a @ref MultiSpinBox for a HSV color value
 * (Hue 0°–360°, Saturation 0–255, Value 0–255) comes here:
 * @snippet test/testmultispinbox.cpp MultiSpinBox Basic example
 *
 * You can also have additional buttons within the spin box via the
 * @ref addActionButton() function.
 *
 * @note This class inherits from <tt>QAbstractSpinBox</tt>, but some
 * parts of the parent class’s API are not supported in <em>this</em>
 * class. Do not use them:
 * - <tt>selectAll()</tt> does not work as expected.
 * - <tt>wrapping()</tt> is ignored. Instead, you can configures
 *   the <em>wrapping</em> individually for each section via
 *   @ref SectionConfiguration::isWrapping.
 * - <tt>specialValue()</tt> is not supported.
 *   <!-- Just as in QDateTimeEdit! -->
 * - <tt>hasAcceptableInput()</tt> is not guaranteed to obey to a particular
 *   and stable semantic.
 * - <tt>fixup()</tt>, <tt>interpretText()</tt>, <tt>validate()</tt> are
 *   not used nor do they anything.
 * - <tt>keyboardTracking()</tt> is ignored. See the signal
 *   @ref sectionValuesChanged for details.
 * - <tt>correctionMode()</tt> is ignored.
 * - <tt>isGroupSeparatorShown</tt> is ignored.
 *
 * @internal
 *
 * Further remarks on inherited API of <tt>QAbstractSpinBox</tt>:
 * - <tt>selectAll()</tt>:
 *   This slot has a default behaviour that relies on internal
 *   <tt>QAbstracSpinBox</tt> private implementations, which we cannot use
 *   because they are not part of the public API and can therefore change
 *   at any moment. As it isn’t virtual, we cannot reimplement it either.
 * - <tt>fixup(), interpretText(), validate()</tt>:
 *   As long as we do not  interact with the private API of
 *   <tt>QAbstractSpinBox</tt> (which we  cannot do because
 *   there is no stability guaranteed), those functions  are never
 *   called by <tt>QAbstractSpinBox</tt> nor does their default
 *   implementation do anything. (They seem rather like an implementation
 *   detail of Qt that was leaked to the public API.) We don’t use them
 *   either.
 * - <tt>isGroupSeparatorShown</tt>:
 *   Implementing this seems complicate. In the base class, the setter
 *   is  not virtual, and this property does not have a notify signal
 *   eihter.  But we would have to react on a changes in this property:
 *   The content of the <tt>QLineEdit</tt> has to be updated. And the
 *   @ref minimumSizeHint and the @ref sizeHint will change, therefore
 *   <tt>updateGeometry</tt> has to be called. It seems better not to
 *   implement this. Alternativly, it could be implemented with a
 *   per-section approach via  @ref SectionConfiguration.
 *
 * @note The interface of this class could theoretically
 * be similar to other Qt classes that offer similar concepts of various
 * data within a list: QComboBox, QHeaderView, QDateTimeEdit, QList – of
 * course with consistent naming. But usually you will not modify a single
 * section configuration, but the hole set of configurations. Therefore
 * we do the configuration by @ref SectionConfiguration objects, similiar
 * to <tt>QNetworkConfiguration</tt> objects. Allowing changes to individual
 * sections would require a lot of additional code to make sure that after
 * such a change, the text curser is set the the appropriate position and
 * the text selection is also appropriate. This might be problematic,
 * and gives also little benefit.
 * However, a full-featured interface could look like that:
 * @snippet test/testmultispinbox.cpp MultiSpinBox Full-featured interface
 *
 * @todo Now, @ref setSectionValues does not select automatically the first
 * section anymore. Is this in conformance with <tt>QDateTimeEdit</tt>?
 *
 * @todo <tt>Crtl-A</tt> support for this class. (Does this shortcut
 * trigger <tt>selectAll()</tt>?)
 *
 * @todo <tt>Ctrl-U</tt> support for this class? If so, do it via @ref clear().
 *
 * @todo MultiSpinBox should never become 0 because the validator
 * allows something that the converter cannot convert!
 *
 * @todo Bug: In @ref ColorDialog, choose a tab with one of the diagrams.
 * Then, switch back the the “numeric“ tab. Expected behaviour: When
 * a @ref MultiSpinBox gets back the focus, always the first section should
 * be <em>highligted/selected</em>, independent from what was selected or
 * the cursor position before the @ref MultiSpinBox lost the focus.
 * (While <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt> don’t do that
 * either, <tt>QDateTimeEdit</tt> indeed <em>does</em>, and that seems
 * appropriate also for @ref MultiSpinBox.
 *
 * @todo Bug: Enter HLC values like “<tt>80.</tt>” or “<tt>80,</tt>”
 * or “<tt>80e</tt>”. Depending on the locale, it is possible to
 * actually enter these characters, but apparently on validation it
 * is not accepted and the value is replaced by <tt>0</tt>.
 *
 * @todo Sometimes, after a click on the action, the first section
 * is selected. Sometimes not. That’s inconsistant!
 *
 * @todo Currently, if the widget has <em>not</em> the focus but the
 * mouse moves over it and the scroll wheel is used, it’s the first
 * section that will be changed, and not the one where the mouse is,
 * as the user might expect. Even QDateTimeEdit does the same thing
 * (thus they do not change the first section, but the last one that
 * was editig before). But it would be great if we could do better here.
 * But: Is this realistic and will the required code work on all
 * platforms?
 *
 * @todo When adding Bengali digits (for example by copy and paste) to a
 * @ref MultiSpinBox that was localized to en_US, than sometimes this is
 * accepted (thought later “corrected” to 0), and sometimes not. This
 * behaviour is inconsistend and wrong.
 *
 * @todo Imagine a section in a @ref MultiSpinBox that has 0 decimals and
 * a value of 15.7. It is displayed as 16. When the user scrolls up with
 * the mouse or uses the key ↑ than the value changes to 16.7 while the
 * display changes to 17. Isn’t is highly confusing that – after a user
 * interaction – the new internal value is different from what the users sees?
 * When creating a <tt>QDoubleSpinBox</tt> with <tt>decimals == 2</tt>
 * and then <tt>setValue(1.3579)</tt>, than <tt>value() == 1.36</tt>. So
 * <tt>QDoubleSpinBox</tt> does not store internally the original value
 * it gets via <tt>setValue</tt> but a value rounded to the current
 * decimal precision. @ref MultiSpinBox should adopt the exactly same
 * behaviour (for consistency). This should be controlled by unit tests.
 *
 * @todo When using this widget in @ref ColorDialog, there are rounding
 * differences, for example when choosing HSV 210° 27 19, then RGB becomes
 * 17 18 19. But when passing through an RGB field, HSV will be changed
 * to 210° 26.8405 19 even if in the RGB field nothing was changed by the
 * user. This is confusing. Also: If values are 0 and then passing through
 * fields of other color representations, the value can become something
 * like 0.001 by rounding error, and so the Arrow-Down-Button will be
 * enabled thought the displayed value is 0 (and this was the value previously
 * manually entered by the user); this is confusing.
 *
 * @todo Apparently, the validator doesn’t restict the input actually to the
 * given range. For QDoubleSpinBox however, the line edit <em>is</em>
 * restricted! Example: even if 100 is maximum, it is possible to write 444.
 * Maybe our @ref ExtendedDoubleValidator should not rely on Qt’s validator,
 * but on if QLocale is able to convert (result: valid) or not (result:
 * invalid)?!
 *
 * @todo Do not text-cursor-select more than <em>one</em> value when
 * editing (or do not allow text selection at all)? Also: Selecting text
 * with the mouse cursor seems somewhat impredictable. */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

    /** @brief A list containing the values of all sections.
     *
     * @note It is not this property, but @ref sectionConfigurations
     * which determines the actually available count of sections in this
     * widget. If you want to change the number of available sections,
     * call <em>first</em> @ref setSectionConfigurations and only
     * <em>after</em> that adapt this property.
     *
     * @invariant This property contains always as many elements as
     * @ref sectionConfigurations contains.
     *
     * @sa READ @ref sectionValues() const
     * @sa WRITE @ref setSectionValues()
     * @sa NOTIFY @ref sectionValuesChanged() */
    Q_PROPERTY(QList<double> sectionValues READ sectionValues WRITE setSectionValues NOTIFY sectionValuesChanged USER true)

public:
    /** @brief The configuration of a single section
     * within a @ref MultiSpinBox.
     *
     * For a specific section within a @ref MultiSpinBox, this configuration
     * contains various settings.
     *
     * This data type can be passed to QDebug thanks to
     * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionConfiguration &value)
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @internal
     *
     * Also Qt itself uses this configuration-object-based approach with its
     * QNetworkConfiguration class (including @ref pimpl and
     * copy-constructors).
     *
     * @todo Use @ref pimpl with setters and getters. This solves two
     * problems: First: Make sure that things like “maximum >= minimum” are
     * guaranteed. Second: Make this future-proof! Maybe later we want to add
     * <tt>stepType</tt> values? */
    struct SectionConfiguration {
        /** @brief The number of digits after the decimal point.
         *
         * (This value can also be <tt>0</tt> to get integer-like behavoir.) */
        int decimals = 2;
        /** @brief Holds whether or not @ref sectionValues wrap around when
         * they reaches @ref minimum or @ref maximum.
         *
         * The default is <tt>false</tt>.
         *
         * If <tt>false</tt>, @ref sectionValues shall be bound between
         * @ref minimum and  @ref maximum. If <tt>true</tt>,
         * @ref sectionValues shall be treated
         * as a circular sectionValues.
         *
         * Example: You have a section that displays a value measured in
         * degree. @ref minimum is <tt>0</tt>. @ref maximum is <tt>360</tt>.
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
        /** @brief The maximum possible value of the section. */
        double maximum = 99.99;
        /** @brief The minimum possible value of the section. */
        double minimum = 0;
        /** @brief A prefix to be displayed before the value. */
        QString prefix;
        /** @brief The smaller of two natural steps.
         *
         * Valid range: >= 0
         *
         * When the user uses the arrows to change the spin box’s value
         * the value will be incremented/decremented by the amount of the
         * @ref singleStep. */
        double singleStep = 1;
        /** @brief A suffix to be displayed behind the value. */
        QString suffix;
    };

    Q_INVOKABLE MultiSpinBox(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~MultiSpinBox() noexcept override;
    void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual void clear() override;
    virtual QSize minimumSizeHint() const override;
    Q_INVOKABLE QList<MultiSpinBox::SectionConfiguration> sectionConfigurations() const;
    /** @brief Getter for property @ref sectionValues
     *  @returns the property @ref sectionValues */
    QList<double> sectionValues() const;
    Q_INVOKABLE void setSectionConfigurations(const QList<MultiSpinBox::SectionConfiguration> &newSectionConfigurations);
    virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;

public Q_SLOTS:
    void setSectionValues(const QList<double> &newSectionValues);

Q_SIGNALS:
    /** @brief Notify signal for property @ref sectionValues.
     *
     * This signal is emitted whenever the value in one or more sections
     * changes.
     *
     * @param newSectionValues the new @ref sectionValues
     *
     * Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @note The property <tt>keyboardTracking()</tt> of the base class
     * is currently ignored. Keyboard tracking is <em>always</em> enabled:
     * The spinbox emits this signal while the new value is being entered
     * from the keyboard – one signal for each key stroke. */
    void sectionValuesChanged(const QList<double> &newSectionValues);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual bool event(QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)

    class MultiSpinBoxPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class MultiSpinBoxPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestMultiSpinBox;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionConfiguration &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBox::SectionConfiguration)

#endif // MULTISPINBOX_H
