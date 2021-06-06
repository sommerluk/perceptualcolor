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
 * precision individually by section using @ref SectionData::decimals.
 * (This value can also be <tt>0</tt> to get integer-like behavoir.)
 *
 * Example code to create a @ref MultiSpinBox for a HSV color value
 * (Hue 0°–360°, Saturation 0–255, Value 0–255) comes here:
 * @snippet test/testmultispinbox.cpp MultiSpinBox Basic example
 *
 * You can also have additional buttons within the spin box via the
 * @ref addActionButton() function.
 *
 * @internal
 *
 * @note It would be possible to add more functions to work with
 * @ref MultiSpinBox::SectionData. The interface could theoretically
 * be similar to other Qt classes that offer similar concepts of various
 * data within a list: QComboBox, QHeaderView, QDateTimeEdit, QList – of
 * course with consistent naming. But probably it’s not worth the pain.
 * Currently, we only allow to get and set all the sections as whole, and for
 * our internal usage, that’s enought. Allowing changes to individual would
 * require a lot of additional code to make sure that after such a change,
 * the text curser is set the the appropriate position and the text selection
 * is also appropriate. For the moment, no further interface functions
 * are planned. However, a full-featured interface could look like that:
 * @snippet test/testmultispinbox.cpp MultiSpinBox Full-featured interface
 *
 * @todo When creating a <tt>QDoubleSpinBox</tt> with <tt>decimals == 2</tt>
 * and then <tt>setValue(1.3579)</tt>, than <tt>value() == 1.36</tt>. So
 * <tt>QDoubleSpinBox</tt> does not store internally the original value
 * it gets via <tt>setValue</tt> but a value rounded to the current
 * decimal precision. @ref MultiSpinBox should adopt the exactly same
 * behaviour (for consistency). This should be controlled by unit tests.
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
 * How does QDoubleSpinBox solves this?
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
 * editing (or do not allow text selection at all)? Also: Selecting text
 * with the mouse cursor seems somewhat impredictable.
 *
 * @todo Splitting @ref MultiSpinBox::SectionData into
 * <tt>SectionConfiguration</tt> and <tt>value</tt>? Disadvantage: Requires
 * to double all access functions. Advantage: Often, you only want the value,
 * and not the <tt>SectionConfiguration</tt>. But this change is probably
 * not worth all the work for little benefit…
 */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    /** @brief The data of a single section within a @ref MultiSpinBox.
     *
     * For a specific section within a @ref MultiSpinBox, this data structure
     * contains on the one hand the @ref value itself, and on the other hand
     * also the various setting parameters for the widget.
     *
     * This data type can be passed to QDebug thanks to
     * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionData &value)
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use it reliably in Qt's signals
     * and slots), you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @internal
     *
     * Possible additions to SectionData:
     * @snippet test/testmultispinbox.cpp SectionData Addons
     *
     * @todo Wouldn’t it be better to use the same defaults as QDoubleSpinBox?
     * Like decimals==2.This could be controlled in a unit test.
     *
     * @todo How to make this future-proof? Maybe later we want to add
     * singleStep() or pageStep() values? Are d-pointers working well when
     * this data structure has to be copy-able? */
    struct SectionData {
        /** @brief The number of digits after the decimal point.
         *
         * (This value can also be <tt>0</tt> to get integer-like behavoir.) */
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
    };

    Q_INVOKABLE MultiSpinBox(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~MultiSpinBox() noexcept override;
    void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual QSize minimumSizeHint() const override;
    Q_INVOKABLE QList<MultiSpinBox::SectionData> sections() const;
    Q_INVOKABLE void setSections(const QList<MultiSpinBox::SectionData> &newSections);
    virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;

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

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBox::SectionData &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBox::SectionData)

#endif // MULTISPINBOX_H
