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

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

/** @file
 * 
 * Declaration of the @ref PerceptualColor::ColorDialog class and its
 * members. */

#include "PerceptualColor/alphaselector.h"
#include "PerceptualColor/colorpatch.h"
#include "PerceptualColor/chromahuediagram.h"
#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/gradientselector.h"
#include "PerceptualColor/wheelcolorpicker.h"

#include <QByteArray>
#include <QColorDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QTabWidget>

namespace PerceptualColor {

/** @brief A perceptually uniform color picker dialog
 * 
 * The color dialog’s function is to allow users to choose colors intuitively.
 * For example, you might use this in a drawing program to allow the user to
 * set the brush color.
 * 
 * It is a almost source-compatible replacement for
 * QColorDialog (see below for details) and also adds some extra functionality
 * that is not available in QColorDialog.
 * 
 * At difference to QColorDialog, this
 * dialog's graphical components are perceptually uniform and therefore more
 * intuitive. It's internally based on the LCh color model, which does reflect
 * the human perception much better than RGB or its transforms like HSV. At
 * the same time, this dialog does not require the user itself to know
 * anything about LCh at all, because the graphical representations tend to be
 * intuitive enough.
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
 * @note The API of this class is fully source-compatible to the API of
 * QColorDialog and the API behaves exactly as for QColorDialog (if not,
 * it's a bug; please report it), with the following exceptions:
 * - As this dialog does not provide functionality for custom colors and
 *   standard color, the corresponding static functions of QColorDialog are
 *   not available in this class.
 * - The option <tt>ColorDialogOption::DontUseNativeDialog</tt>
 *   will always remain <tt>false</tt> (even if set explicitly), because it's
 *   just the point of this library to provide an own, non-native dialog.
 * - While the enum declaration @ref ColorDialogOption itself aliased
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
 * 
 * @warning The graphical display in @ref WheelColorPicker jumps when you
 * choose a gray color like HSV 20 0 125 and then increment or decrement the
 * V component in the QSpinBox by 1. This is because @ref WheelColorPicker is
 * based on the LCh model and LCh’s hue component is different from HSV’s hue
 * component, and the jump is a consequence of rounding errors. There is no
 * jump when using the LCh input widget because the old hue is guarded. How
 * can we get a similar continuity for HSV’s hue? (By the way: Similar problem
 * for RGB values changing along the gray axis: #444 → #555 → #666 changes
 * the graphically displayed hue.
 * 
 * @todo Implement unit tests for private slots, methods and members. (For
 * everything else, unit tests are available yet now.) This has to be done
 * <em>after</em> the MultiSpinBox is available and has replaced the
 * indivudual spin boxes and the line edits in this dialog.
 * 
 * @todo Touch screen compatibility: Position of the color patch
 * <em>above</em> the selector widget instead of below?
 * 
 * This would be better for touch screen compatibility (the hand does not
 * hide the widget that shows the resulting color); also Apple does this in
 * its native color dialog.
 * 
 * On the other hand, the overall layout would be less logical: The typical
 * workflow is probably:
 * 1. Start with selecting a color
 * 2. Verify the color in the color patch
 * 3. Modify its alpha component
 * 4. Confirm with OK button
 * 
 * Currently, this workflow is reflected by the layout: All elements are
 * ordered in this very same order, from top to bottom. Changing the
 * position of the color patch would break that.
 * 
 * A possible solution might be to control this with a property (enum
 * DialogColorPatchPosition “optimizeForTouchscreen” “optimizeForWorkflow”).
 * 
 * @todo In general: What would mean better support for touch-screen and 
 * convertible? Probably: A layout that has the @ref ColorPatch in top
 * position instead of bottom position (so the hand of the user won‘t cover
 * it and hide it while using the dialog).
 * 
 * @todo Use @ref MultiSpinBox for <em>all</em> numeric values. We would
 * update the color only if the focus leaves the @ref MultiSpinBox or when
 * the enter key is pressed (and no Okay button in the dialog). No live
 * update if only <em>one</em> of the three components is changed. This is
 * necessary for LCh values that can only be evaluated for all three
 * values at the same time. And it should be done for RGB-based values as
 * well, just to be consistent.
 * 
 * @todo In general: What would mean QML support?
 * 
 * @todo Bug fix this:
 * - Set the value HSV 200 0 0
 * - Push tabulator key until the focus enters in the “Hex” field. The
 *   HSV values stays always 200 0 0.
 * - Push the tabulator key once again: The HSV value changes to
 *   200 0 0.
 * 
 * There are two issues:
 * 
 * First: Focus change should never change values.
 * 
 * Second: Why 360°? Even if this would be the correct value, it should
 * never be represented by 360°, but always by 0°!
 * 
 * @todo Use the <em>actual</em> color profile of the monitor.
 * 
 * @todo High-dpi-support: Scale the icons of the “OK button” and “Cancel
 * button” correctly. See also:
 * https://bugs.kde.org/show_bug.cgi?id=273938
 * 
 * @todo The QLineEdit for the hexadecimal RGB values should change lower-case
 * letters on-the-fly (as-you-type) to upper-case letters.
 * 
 * @todo Provide <tt>setWhatsThis()</tt> help for widgets. Or tool tips? Or
 * both? What is more appropriate? Or use both? For @ref WheelColorPicker and
 * @ref ChromaLightnessDiagram, this help text could describe the keyboard
 * controls and be integrated as default value in the class itself. For the
 * other widgets, a help text could be defined here within <em>this</em>
 * class, if appropriate.
 * 
 * @todo Make sure that @ref ChromaHueDiagram always shows at least at the
 * central pixel an in-gamut color. Solution: Limit the range of the lightness
 * selector? Or a better algorithm in @ref ChromaHueDiagram?
 * 
 * @todo Develop a new widget inherited from QAbstractSpinbox() that allows
 * input of various numeric values in the same widget, similar to
 * QDateTimeEdit(), but more flexible and appropriate for the different
 * possible color representations. Maybe: Allow entering
 * <em>HLC 359 50 29</em> in the form
 * <em>h*<sub>ab</sub> 359 L* 50 C*<sub>ab</sub> 29</em>? Would this
 * actually be a better user experience, or would it be rather confusing
 * and less comfortable?
 * 
 * @todo The child widget @ref m_hsvHueSpinbox() is a QDoubleSpinBox. The
 * current behavior for pageStep‌ = 10 is 356 → 360 → 0 → 10. The expected
 * behavior would be 356 → 6 for a continuous experience. A solution will
 * likely require a new class inherited from QDoubleSpinBox or maybe
 * QAbstractSpinbox. Provide directly a widget that would also do the
 * work for HLC etc (calendar-widget-like with various values within
 * a single spin box).
 * 
 * @todo Background color (or text color) for RGB could be red, green and
 * blue, corresponding to each component?
 * 
 * @todo Support for other models like HSL, Munsell? With an option to
 * enable or disable them? (NCS not, because it is not free.)
 * 
 * @todo Provide (on demand) two patches, like Scribus also does: One for the
 * old color (cannot be modified by the user) and another one for the new
 * color (same behavior as the yet existing color patch). One could be
 * named “before” and the other “after”. Or maybe make this configurable?
 * And put an arrow between the patches, from “before” to “after”. (Be aware:
 * RTL support necessary!)
 * 
 * @todo For the tab widget, use rather icons instead of the text “hue first”
 * and “lightness first”!?
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
 * <a href="https://en.wikipedia.org/wiki/Web_colors#Web-safe_colors">Web-save colors</a>
 * (but with another name, maybe “216 colors”) as bigger palette, without a
 * specific color space?
 * 
 * @todo What about functions that are deprecated in QColorDialog? This seems
 * to be currently only apply to <tt>QRgb QColorDialog::getRgba(QRgb
 * initial = 0xffffffff, bool *ok = nullptr, QWidget *parent = nullptr)</tt>,
 * which was deprecated somewhere at 5.12 following the source code from
 * Woboq, but much earlier following the online documentation.
 * Support them? Or document that we do not support them? */
class ColorDialog : public QDialog
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
     * @sa READ @ref currentColor()
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged()
     * @sa @ref m_currentOpaqueColor */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

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
     *   @invariant The option
     *   <tt>ColorDialogOption::DontUseNativeDialog</tt> will
     *   always be <tt>true</tt> because it's just the point of this
     *   library to provide an own, non-native dialog. (If you set
     *   <tt>ColorDialogOption::DontUseNativeDialog</tt>
     *   explicitly to <tt>false</tt>, this will silently be ignored, while
     *   the other options that you might have set, will be correctly
     *   applied.)
     * 
     * Example:
     * @snippet test/testcolordialog.cpp ColorDialog Set options with local enum
     * Or:
     * @snippet test/testcolordialog.cpp ColorDialog Set options with QColorDialog enum
     * @note At difference to QColorDialog, you need a fully qualified
     * identifier for the enum values. The following code would therefore
     * fail:<br/>
     * <tt>myDialog->setOption(ShowAlphaChannel, false);</tt>
     * 
     * @sa READ @ref options()
     * @sa @ref testOption()
     * @sa WRITE @ref setOptions()
     * @sa @ref setOption()
     * @sa @ref m_options */
    Q_PROPERTY(ColorDialogOptions options READ options WRITE setOptions)

    /** @brief Layout dimensions
     * 
     * Defines if the dialog uses a rather collapsed (small) or a rather
     * expanded (large) layout. In both cases, all elements are present.
     * But for the collapsed variant, more elements are put in
     * tab widgets, while for the expanded variant, more elements are
     * visible at the same time.
     * 
     * Default value: PerceptualColor::DialogLayoutDimensions::automatic
     * 
     * When the layout dimension effectively changes, also the dialog size
     * is adapted.
     * 
     * @sa @ref DialogLayoutDimensions
     * @sa @ref layoutDimensions()
     * @sa @ref setLayoutDimensions() */
    Q_PROPERTY(DialogLayoutDimensions layoutDimensions READ layoutDimensions WRITE setLayoutDimensions)
        
public:
    /** @brief Local alias for QColorDialog::ColorDialogOption */
    typedef QColorDialog::ColorDialogOption ColorDialogOption;
    /** @brief Local alias for QColorDialog::ColorDialogOptions */
    typedef QColorDialog::ColorDialogOptions ColorDialogOptions;
    /** @brief Layout dimensions
     * 
     * This enum is declared to the meta-object system. This happens
     * automatically. You do not need to make any manual calls. */
    enum class DialogLayoutDimensions {
        automatic,  /**< Decide automatically between <tt>collapsed</tt> and
                         <tt>expanded</tt> layout based on evaluating the
                         screen size. The decision is evaluated at the moment
                         when setting this value, and again each time the
                         widget is shown again. It is <em>not</em> evaluated
                         again when a yet existing dialog is just moved to
                         another screen. */
        collapsed,  /**< Use the small, “collapsed“ layout of this dialog. */
        expanded    /**< Use the large, “expanded” layout of this dialog.  */
    };
    Q_ENUM(DialogLayoutDimensions)
    explicit ColorDialog(QWidget *parent = nullptr);
    explicit ColorDialog(const QColor &initial, QWidget *parent = nullptr);
    virtual ~ColorDialog() override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    QColor currentColor() const;
    static QColor getColor(
        const QColor &initial = Qt::white,
        QWidget *parent = nullptr,
        const QString &title = QString(),
        ColorDialogOptions options = ColorDialogOptions()
    );
    ColorDialog::DialogLayoutDimensions layoutDimensions() const;
    // Make sure not to override the base class’s “open“ function
    using QDialog::open;
    void open(QObject *receiver, const char *member);
    /** @brief Getter for property @ref options
     * @returns the current @ref options */
    ColorDialogOptions options() const;
    QColor selectedColor() const;
    void setLayoutDimensions(
        const ColorDialog::DialogLayoutDimensions newLayoutDimensions
    );
    void setOption(ColorDialogOption option, bool on = true);
    void setOptions(ColorDialogOptions options);
    virtual void setVisible(bool visible) override;
    bool testOption(ColorDialogOption option) const;

public Q_SLOTS:
    void setCurrentColor(const QColor &color);

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

protected:
    virtual void done(int result) override;

private:
    Q_DISABLE_COPY(ColorDialog)

    /** @brief Pointer to the @ref GradientSelector for alpha. */
    QPointer<AlphaSelector> m_alphaSelector;
    /** @brief Pointer to the QLabel for @ref m_alphaSelector().
     * 
     * We store this in a
     * pointer to allow toggle the visibility later. */
    QPointer<QLabel> m_alphaSelectorLabel;
    /** @brief Pointer to the QButtonBox of this dialog.
     * 
     * We store this in a pointer
     * to allow toggle the visibility later. */
    QPointer<QDialogButtonBox> m_buttonBox;
    /** @brief Pointer to the @ref ChromaHueDiagram. */
    QPointer<ChromaHueDiagram> m_chromaHueDiagram;
    /** @brief Pointer to the @ref ColorPatch widget. */
    QPointer<ColorPatch> m_colorPatch;
    /** @brief Holds the current color without alpha information
     * 
     * @note The alpha information within this data member is meaningless.
     * Ignore it. The information about the alpha channel is actually stored
     * within @ref m_alphaSelector.
     * 
     * @sa @ref currentColor() */
    FullColorDescription m_currentOpaqueColor;
    /** @brief Pointer to the @ref GradientSelector for LCh lightness. */
    QPointer<GradientSelector> m_lchLightnessSelector;
    /** @brief Pointer to the QLineEdit that represents the HLC value. */
    QPointer<QLineEdit> m_hlcLineEdit;
    /** @brief Pointer to the QSpinBox for HSV hue. */
    QPointer<QDoubleSpinBox> m_hsvHueSpinbox;
    /** @brief Pointer to the QSpinBox for HSV saturation. */
    QPointer<QDoubleSpinBox> m_hsvSaturationSpinbox;
    /** @brief Pointer to the QSpinBox for HSV value. */
    QPointer<QDoubleSpinBox> m_hsvValueSpinbox;
    /** @brief Holds whether currently a color change is ongoing, or not.
     * 
     * Used to avoid infinite recursions when updating the different widgets
     * within this dialog.
     * @sa @ref setCurrentOpaqueColor() */
    bool m_isColorChangeInProgress = false;
    /** @brief Internal storage for property @ref layoutDimensions */
    PerceptualColor::ColorDialog::DialogLayoutDimensions m_layoutDimensions =
        ColorDialog::DialogLayoutDimensions::automatic;
    /** @brief Pointer to the graphical selector widget that groups lightness
     *  and chroma-hue selector. */
    QPointer<QWidget> m_lightnessFirstWidget;
    /** @brief Holds the receiver slot (if any) to be disconnected
     *  automatically after closing the dialog.
     * 
     * Its value is only meaningful if
     * @ref m_receiverToBeDisconnected is not null.
     * @sa @ref m_receiverToBeDisconnected
     * @sa @ref open() */
    QByteArray m_memberToBeDisconnected;
    /** @brief Pointer to the widget that holds the numeric color
     *         representation. */
    QPointer<QWidget> m_numericalWidget;
    /** @brief Holds the receiver object (if any) to be disconnected
     *  automatically after closing the dialog.
     * 
     * @sa @ref m_memberToBeDisconnected
     * @sa @ref open() */
    QPointer<QObject> m_receiverToBeDisconnected;
    /** @brief Internal storage for property @ref options */
    ColorDialogOptions m_options;
    /** @brief Pointer to the QSpinBox for RGB blue. */
    QPointer<QDoubleSpinBox> m_rgbBlueSpinbox;
    /** @brief Pointer to the RgbColorSpace object. */
    QPointer<RgbColorSpace> m_rgbColorSpace;
    /** @brief Pointer to the QSpinBox for RGB green. */
    QPointer<QDoubleSpinBox> m_rgbGreenSpinbox;
    /** @brief Pointer to the QLineEdit that represents the hexadecimal
     *  RGB value. */
    QPointer<QLineEdit> m_rgbLineEdit;
    /** @brief Pointer to the QSpinBox for RGB red. */
    QPointer<QDoubleSpinBox> m_rgbRedSpinbox;
    /** @brief Internal storage for selectedColor(). */
    QColor m_selectedColor;
    /** @brief Layout that holds the graphical and numeric selectors. */
    QPointer<QHBoxLayout> m_selectorLayout;
    /** @brief Pointer to the tab widget. */
    QPointer<QTabWidget> m_tabWidget;
    /** @brief Pointer to the @ref WheelColorPicker widget. */
    QPointer<WheelColorPicker> m_wheelColorPicker;

    void applyLayoutDimensions();
    void initialize();
    QWidget* initializeNumericPage();
    QString textForHlcLineEdit() const;

private Q_SLOTS:
    void handleFocusChange(QWidget *old/*, QWidget *now*/);
    void readHlcNumericValues();
    void readHsvNumericValues();
    void readLightnessValue();
    void readRgbHexValues();
    void readRgbNumericValues();
    void setCurrentOpaqueColor(
        const PerceptualColor::FullColorDescription &color
    );
    void setCurrentOpaqueQColor(const QColor &color);
    void updateColorPatch();
};

}

#endif // COLORDIALOG_H
