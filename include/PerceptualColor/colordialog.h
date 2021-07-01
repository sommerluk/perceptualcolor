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

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QColorDialog>

#include "PerceptualColor/constpropagatinguniquepointer.h"

namespace PerceptualColor
{
/** @brief A perceptually uniform color picker dialog
 *
 * The color dialog’s function is to allow users to choose colors intuitively.
 * For example, you might use this in a drawing program to allow the user to
 * set the brush color.
 *
 * At difference to QColorDialog, this
 * dialog’s graphical components are perceptually uniform and therefore more
 * intuitive. It’s internally based on the LCh color model, which does reflect
 * the human perception much better than RGB or its transforms like HSV. At
 * the same time, this dialog does not require the user itself to know
 * anything about LCh at all, because the graphical representations is
 * intuitive.
 *
 * @image html ColorDialog.png "ColorDialog" width=500
 *
 * It is an @ref qdialogsourceompatibility "mostly source-compatible"
 * replacement for QColorDialog. It also adds some extra functionality
 * that is not available in QColorDialog.
 *
 * Just as with QColorDialog, the static functions provide modal color
 * dialogs. The static getColor() function shows the dialog, and allows
 * the user to specify a color:
 *
 * @snippet test/testcolordialog.cpp ColorDialog Get color
 *
 * The function can also be used to let
 * users choose a color with a level of transparency: pass the alpha channel
 * option as an additional argument:
 *
 * @snippet test/testcolordialog.cpp ColorDialog Get color with alpha channel
 *
 * @image html ColorDialogAlpha.png "ColorDialog with alpha channel" width=500
 *
 * For non-modal dialogs, use the normal constructors of this class.
 *
 * The default window title is <em>Select Color</em>, and not the title of
 * your application. It can of course be customized with
 * <tt>QWidget::setWindowTitle()</tt>.
 *
 * At difference to most native platform color dialogs, <em>this</em> dialog
 * can be resized. That makes sense, because it allows to see better the
 * gamut image. Therefore, this dialog is by default bigger than usual
 * native platform color dialogs. You can of course customize the dialog
 * size with QWidget::resize() or force a more space-saving layout through
 * the @ref layoutDimensions property.
 *
 * The @ref ColorPatch that indicates the selected color is placed prominently
 * at the top of the widget. That is also useful for touch screens as the
 * @ref ColorPatch will not be hidden by the hand of the user when the user
 * is touching the above color selection widgets.
 *
 * This dialog uses icons. See @ref hidpisupport "High DPI support"
 * about how to enable support for high-DPI icons.
 *
 * @note @anchor qdialogsourceompatibility The API of this class is mostly
 * source-compatible to the API of QColorDialog. This is a list of
 * incompatibilities:
 * - As this dialog does not provide functionality for custom colors and
 *   standard color, the corresponding static functions of QColorDialog are
 *   not available in this class.
 * - The option <tt>ColorDialogOption::DontUseNativeDialog</tt>
 *   will always remain <tt>false</tt> (even if set explicitly), because it’s
 *   just the point of this library to provide an own, non-native dialog.
 * - While the enum declaration @ref ColorDialogOption itself is aliased
 *   here, this isn't possible for the enum values itself. Therefor, when
 *   working with @ref options, you cannot use <tt>ShowAlphaChannel</tt> but
 *   you have to use the fully qualified identifier (either
 *   <tt>PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel</tt>
 *   or <tt>QColorDialog::ShowAlphaChannel</tt>, at your option.
 * - Calling @ref setCurrentColor() with colors that
 *   are <em>not</em> <tt>QColor::Spec::Rgb</tt> will lead to an automatic
 *   conversion like QColorDialog does, but at difference to QColorDialog, it
 *   is done with more precision, therefor the resulting
 *   @ref currentColor() might be slightly different. The same is true
 *   for <tt>QColor::Spec::Rgb</tt> types with floating point precision:
 *   While QColorDialog would round to full integers, <em>this</em> dialog
 *   preserves the floating point precision.
 * - When the default constructor is used, unlike QColorDialog, the default
 *   color is <em>not</em> guaranteed to be <tt>Qt::white</tt>.
 *
 * @internal
 *
 * In the QColorDialog API, the function <tt>QColorDialog::getRgba()</tt> has
 * been deprecated during the Qt5 life cicly. This class does not provide
 * source compatibility with obsolete functions of teh QColorDialog API.
 *
 * @todo Make sure that @ref ChromaHueDiagram always shows at least at the
 * central physical pixel with an in-gamut color. Solution: Limit the range
 * of the lightness selector? Or a better algorithm in @ref ChromaHueDiagram?
 *
 * @todo Provide (on demand) two patches, like Scribus also does: One for the
 * old color (cannot be modified by the user) and another one for the new
 * color (same behavior as the yet existing color patch). One could be
 * named “before” and the other “after”. Or maybe make this configurable?
 * And put an arrow between the patches, from “before” to “after”. (Be aware:
 * RTL support necessary!)
 *
 * @section uireview Review of the user interface
 *
 * @todo The HLC widget accepts values that are out-of-gamut; the dialog
 * shows the nearest in-gamut color during this moment. Also, the RGB Hex
 * widget accepts intermediatly invalid values (for example, 4-digit-values);
 * the dialog displays the last previously displayed valid color during
 * this moment. Those two situations do not have consiteend behaviour,
 * but there are no good alternatives. However: Would it make sense to
 * keep this behaviour, but display in the @ref ColorPatch an empty
 * value during editing an out-of-gamut HLC value or an invalid RGB Hex values?
 *
 * @todo For the tab widget, use rather icons instead of the text “hue-baded”
 * and “lightness-based”!?
 *
 * @todo Provide <tt>setWhatsThis()</tt> help for widgets. Or tool tips? Or
 * both? What is more appropriate? Or use both? For @ref WheelColorPicker and
 * @ref ChromaLightnessDiagram, this help text could describe the keyboard
 * controls and be integrated as default value in the class itself. For the
 * other widgets, a help text could be defined here within <em>this</em>
 * class, if appropriate.
 *
 * @todo Touch screen compatibility: In general: What would mean better
 * support for touch-screen and convertible? More things to do? For example,
 * A spin box can also be used on mobile phone (putting the numbers
 * with on-screen keyboard). But the + and - button for increasing
 * or decreasing the values might be too small. And mobile UI uses
 * often wheels for this use case…
 *
 * @section additionalcolordialogfeatures Proposals for additional features
 *
 * @todo The QLineEdit for the hexadecimal RGB values should change lower-case
 * letters on-the-fly (as-you-type) to upper-case letters. And: Maybe it
 * could even be switched to @ref MultiSpinBox (but that would mean implement
 * support for hexadecimal digits in @ref MultiSpinBox).
 *
 * @todo Accept <tt>F5</tt> and <tt>Ctrl+R</tt> just with the same
 * functionality as the refresh button in the HCL @ref MultiSpinBox.
 * But attention: If a library user <em>embeds</em> the dialog, he does
 * not want his shortcuts to be intercepted!
 *
 * @todo It might be nice to support keyboard shortcuts for switching tabs
 * like in browsers, which is a concept many users might be familiar to.
 * Crtl+Tab to switch to the next tab in the list. Crtl+Shift+Tab to switch
 * to the previous tab in the list.
 *
 * @todo Start with dialog with Qt::yellow. You get HLC 100° 98% 95.
 * Push the apply button. Actual result: HLC 100° 98% 94 which has a slightly
 * different chroma value. Expected result: Ideally there would be no rounding
 * difference at all: Every once displayed value is always recognized as
 * valid. When the color space conversion takes place, we could (and should)
 * make this sure for HLC values. But then comes the also the rounding
 * in @ref MultiSpinBox. Is there any solution?
 *
 * @todo If there is no alpha widget <em>and</em> the actual layout is
 * expanded (either explicitly by @ref DialogLayoutDimensions::expanded
 * or implicitly by @ref DialogLayoutDimensions::screenSizeDependent on
 * bigger screens) <em>than</em> it would make sense to move
 * the @ref ColorDialogPrivate::m_buttonBox into the same column as
 * the @ref ColorDialogPrivate::m_numericalWidget, namely <em>below</em>
 * the @ref ColorDialogPrivate::m_numericalWidget. This saves screen
 * space and does not confuse the user.
 *
 * @todo Provide an overloaded version of @ref open() that accepts
 * arguments for new-style connect statements, making use of compiler
 * checks.
 *
 * @todo Use the <em>actual</em> color profile of the monitor.
 *
 * @todo The LCh-hue (and so the graphical widgets) jumps forward and backward
 * when changing RGB-based values (also HSV) when entering and leaving the gray
 * axis, due to lack of hue information. Would it be an option to store the
 * old hue to get a meaningful hue?
 * Should it be only really for the gray axis, or allow a certain tolerance
 * around the gray axis is necessary to make this work well - and if so,
 * how much tolerance? Would it be useful to define a certain hue, for
 * example 0°, as default hue for when no old hue is available but the
 * new value is on the gray axis?
 *
 * @todo Custom layout management that has a specific height-per-width ratio
 * considering the @ref ChromaHueDiagram and and @ref WheelColorPicker
 * and <em>their</em> useful height-per-width ratio.
 *
 * @todo Support for other models like HSL (HSB is yet identical to HSV?),
 * Munsell? With an option to enable or disable them? (NCS not, because
 * it is not free…)
 *
 * @todo Would CMYK support make sense? Would it integrate intuitively into
 * the user interface? If we would have CMYK support, we would have two
 * different profiles (RGB and CMYK) active simultaniously. Which one is the
 * profile that defines the available color space? Would this be intuitive?
 * Also, when using two different profiles, it will be possible to have
 * out-of-gamut colors in one of the profiles. How to represent this in the
 * UI?
 *
 * @todo Provide palettes? Include there a palette with QColorDialog’s
 * standard colors as RGB values without a specific color space, with the
 * name “Default”? Add
 * <a href="https://www.w3.org/TR/css-color-4/#named-colors">CSS colors</a> as
 * defined as sRGB values?
 * <a
 * href="https://en.wikipedia.org/wiki/Web_colors#Web-safe_colors">Web-save colors</a>
 * (but with another name, maybe “216 colors”) as bigger palette, without a
 * specific color space – does not make sense, since we provide a perceptual
 * color dialog, which allows things much beyond 216 colors, and 216 colors
 * isn’t a useful standard anymore, and not a nice palette either.
 *
 * @todo Instead of (or additional to) palettes: Discret widgets, that have
 * a fixed (quite limited) number of fields to chose for the user?
 */
class PERCEPTUALCOLOR_IMPORTEXPORT ColorDialog : public QDialog
{
    Q_OBJECT

    /** @brief Currently selected color in the dialog
     *
     * @invariant This property is provided as an RGB value.
     * <tt>QColor::isValid()</tt> is always <tt>true</tt> and
     * <tt>QColor::spec()</tt> is always <tt>QColor::Spec::Rgb</tt>.
     *
     * @invariant The signal @ref currentColorChanged() is emitted always and
     * only when the value of this property changes.
     *
     * @note The setter @ref setCurrentColor() does not accept all QColor
     * values. See its documentation for details.
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged()
     *
     * @internal
     *
     * @note This property does not have <tt>USER true</tt>. While it would
     * be nice to have it, we do not do this because of conformance with
     * QColorDialog, which doesn’t have it either. */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

    /** @brief Layout dimensions
     *
     * Defines if the dialog uses a rather collapsed (small) or a rather
     * expanded (large) layout. In both cases, all elements are present.
     * But for the collapsed variant, more elements are put in
     * tab widgets, while for the expanded variant, more elements are
     * visible at the same time.
     *
     * @image html ColorDialogCollapsed.png "collapsed" height=300
     * @image html ColorDialogExpanded.png "expanded" height=300
     *
     * Default value:
     * @snippet src/colordialog_p.h layoutDimensionsDefaultValue
     *
     * When the layout dimension effectively changes, also the dialog size
     * is adapted.
     *
     * @sa @ref DialogLayoutDimensions
     * @sa READ @ref layoutDimensions() const
     * @sa WRITE @ref setLayoutDimensions()
     * @sa NOTIFY @ref layoutDimensionsChanged */
    Q_PROPERTY(DialogLayoutDimensions layoutDimensions READ layoutDimensions WRITE setLayoutDimensions NOTIFY layoutDimensionsChanged)

    /** @brief Various options that affect the look and feel of the dialog
     *
     * These are the same settings as for QColorDialog. For compatibility
     * reasons, they are also of the same type: @ref ColorDialogOptions
     *
     * | Option              | Default value | Description
     * | :------------------ | :------------ | :----------
     * | ShowAlphaChannel    | false         | Allow the user to select the alpha component of a color.
     * | NoButtons           | false         | Don't display OK and Cancel buttons. (Useful for “live dialogs”.)
     * | DontUseNativeDialog | true          | Use Qt’s standard color dialog instead of the operating system native color dialog.
     *
     *   @invariant The option <tt>ColorDialogOption::DontUseNativeDialog</tt>
     *   will always be <tt>true</tt> because it’s just the point of
     *   this library to provide an own, non-native dialog. (If you
     *   set  <tt>ColorDialogOption::DontUseNativeDialog</tt> explicitly
     *   to <tt>false</tt>, this will silently be ignored, while the
     *   other options that you might have set, will be correctly applied.)
     *
     * Example:
     * @snippet test/testcolordialog.cpp setOptionsWithLocalEnum
     * Or:
     * @snippet test/testcolordialog.cpp setOptionsWithQColorDialogEnum
     * @note At difference to QColorDialog, you need a fully qualified
     * identifier for the enum values. The following code would therefore
     * fail:<br/>
     * <tt>myDialog->setOption(ShowAlphaChannel, false);</tt>
     *
     * @sa READ @ref options() const
     * @sa @ref testOption()
     * @sa WRITE @ref setOptions()
     * @sa @ref setOption()
     * @sa NOTIFY @ref optionsChanged()*/
    Q_PROPERTY(ColorDialogOptions options READ options WRITE setOptions NOTIFY optionsChanged)

public:
    /** @brief Local alias for QColorDialog::ColorDialogOption
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    typedef QColorDialog::ColorDialogOption ColorDialogOption;
    /** @brief Local alias for QColorDialog::ColorDialogOptions
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    typedef QColorDialog::ColorDialogOptions ColorDialogOptions;
    /** @brief Layout dimensions
     *
     * This enum is declared to the meta-object system. This happens
     * automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    enum class DialogLayoutDimensions {
        screenSizeDependent, /**< Decide automatically between
            <tt>collapsed</tt> and <tt>expanded</tt> layout: <tt>collapsed</tt>
            is used on small screens, and <tt>expanded</tt> on big screens. The
            decision is based on the screen size of the <em>default screen</em>
            of the widget (see <tt>QGuiApplication::primaryScreen()</tt> for
            details). The decision is evaluated at the moment when setting this
            value, and again each time the widget is shown again. It is
            <em>not</em> evaluated again when a yet existing dialog is just
            moved to another screen. */
        collapsed,           /**< Use the small, “collapsed“ layout of this dialog. */
        expanded             /**< Use the large, “expanded” layout of this dialog.  */
    };
    Q_ENUM(DialogLayoutDimensions)
    Q_INVOKABLE explicit ColorDialog(QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QColor &initial, QWidget *parent = nullptr);
    virtual ~ColorDialog() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    QColor currentColor() const;
    static QColor getColor(const QColor &initial = Qt::white, QWidget *parent = nullptr, const QString &title = QString(), ColorDialogOptions options = ColorDialogOptions());
    /** @brief Getter for property @ref layoutDimensions
     *  @returns the property @ref layoutDimensions */
    ColorDialog::DialogLayoutDimensions layoutDimensions() const;
    // Make sure not to override the base class’s “open“ function:
    using QDialog::open;
    Q_INVOKABLE void open(QObject *receiver, const char *member);
    /** @brief Getter for property @ref options
     * @returns the current @ref options */
    ColorDialogOptions options() const;
    Q_INVOKABLE QColor selectedColor() const;
    virtual void setVisible(bool visible) override;
    Q_INVOKABLE bool testOption(PerceptualColor::ColorDialog::ColorDialogOption option) const;

public Q_SLOTS:
    void setCurrentColor(const QColor &color);
    void setLayoutDimensions(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    Q_INVOKABLE void setOption(PerceptualColor::ColorDialog::ColorDialogOption option, bool on = true);
    void setOptions(PerceptualColor::ColorDialog::ColorDialogOptions newOptions);

Q_SIGNALS:
    /** @brief This signal is emitted just after the user has clicked OK to
     * select a color to use.
     *  @param color the chosen color */
    void colorSelected(const QColor &color);
    /** @brief Notify signal for property @ref currentColor.
     *
     * This signal is emitted whenever the “current color” changes in the
     * dialog.
     * @param color the new “current color” */
    void currentColorChanged(const QColor &color);
    /** @brief Notify signal for property @ref layoutDimensions.
     * @param newLayoutDimensions the new layout dimensions */
    void layoutDimensionsChanged(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    /** @brief Notify signal for property @ref options.
     * @param newOptions the new options */
    void optionsChanged(const PerceptualColor::ColorDialog::ColorDialogOptions newOptions);

protected:
    virtual void done(int result) override;

private:
    Q_DISABLE_COPY(ColorDialog)

    class ColorDialogPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ColorDialogPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorDialogPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestColorDialog;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::ColorDialogOption)
Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::ColorDialogOptions)
Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::DialogLayoutDimensions)

#endif // COLORDIALOG_H
