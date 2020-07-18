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
 * The color dialog's function is to allow users to choose colors intuitively.
 * For example, you might use this in a drawing program to allow the user to
 * set the brush color. It is a almost source-compatible replacement for
 * QColorDialog (see below for details) and also adds some extra functionality
 * that is not available in QColordialog. At difference to QColorDialog, this
 * dialog's graphical components are perceptually uniform and therefor more
 * intuitive. It's internally based on the LCh color model, which does reflect
 * the human perceptuan much better than RGB or its transforms like HSV. At
 * the same time, this dialog does not require the user itself to know
 * anything about LCh at all, because the graphical representations tend to be
 * intuitive enough.
 * 
 * The default window title is <em>Select Color</em>, and not the title of
 * your application. It can of course be customized with
 * <tt>QWidget::setWindowTitle()</tt>.
 * 
 * Just as with QColorDialog, the static functions provide modal color
 * dialogs. The static getColor() function shows the dialog, and allows
 * the user to specify a color:
 * @code
 * QColor myColor = PerceptualColor::ColorDialog::getColor();
 * @endcode
 * This function can also be used to let
 * users choose a color with a level of transparency: pass the
 * ColorDialogOption::ShowAlphaChannel option as an
 * additional argument:
 * @code
 * QColor myColor = PerceptualColor::ColorDialog::getColor(
 *     Qt::green,      // current color at widget startup
 *     0,              // parent widget
 *     "Window title", // window title
 *     ColorDialogOption::ShowAlphaChannel
 * );
 * @endcode
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
 * - While the enum declaration <tt>ColorDialogOption</tt> itself aliased
 *   here, this isn't possible for the enum values itself. Therefor, when
 *   working with options(), you cannot use <tt>ShowAlphaChannel</tt> but
 *   you have to use the fully qualified identifier (either
 *   <tt>PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel</tt>
 *   or <tt>QColorDialog::ShowAlphaChannel</tt>, at your option.
 * - Calling setCurrentColor() with colors that
 *   are @em not <tt>QColor::Spec::Rgb</tt> will lead to an automatic
 *   conversion like QColorDialog does, but at difference to QColorDialog, the
 *   PerceptualColor::ColorDialog has more precision, therefor the resulting
 *   currentColor() might be slightly different. The same is true
 *   for <tt>QColor::Spec::Rgb</tt> types with floating point precision:
 *   While QColorDialog would round to full
 *   integers, PerceptualColor::ColorDialog preserves the floating point
 *   precision.
 * 
 * @warning The graphical display in WheelColorPicker() jumps when you choose
 * a gray color like HSV 20 0 125 and then increment or decrement the
 * V component in the spinbox by 1. This is because WheelColorPicker() is
 * based on the LCh model and LCh’s hue component is different from HSV’s hue
 * component, and the jump is a consequence of rounding errors. There is no
 * jump when using the LCh input widget because the old hue is guarded. How
 * can we get a similar continuity for HSV’s hue?
 * 
 * @todo The QLineEdit for the hexadecimal RGB values should change lower-case
 * letters on-the-fly (as-you-type) to upper-case letters.
 * 
 * @todo Provide setWhatsThis() help for widgets. Or tooltips? Or both?
 * What is more appropritate? Or use both? For WheelColorPicker and
 * ChromaLightnessDiagram, this help text could describe the keyboard controls
 * and be integrated as default value in the class itself. For the other
 * widgets, a help text could be defined here within \em this class,
 * if appropriate.
 * 
 * @todo Make sure that ChromaHueDiagram always shows at least at the central
 * pixel an in-gamut color. Solution: Limit the range of the lightness
 * selector? Or a better algorithm in ChromaHueDiagram?
 * 
 * @todo Develop a new wiget inherited from QAbstractSpinbox() that allows
 * input of various numeric values in the same widget, similar to
 * QDateTimeEdit(), but more flexible and appropriate for the different
 * possible color representations. Maybe: Allow entering
 * <em>HLC 359 50 29</em> in the form
 * <em>h*<sub>ab</sub> 359 L* 50 C*<sub>ab</sub> 29</em>? Would this actually
 * be a better user experience, or would it be rather confusing and less
 * comfortable?
 * 
 * @todo The child widget m_hsvHueSpinbox() is a QDoubleSpinBox. The current
 * behaviour for pageStep = 10 is 356 → 360 → 0 → 10. The expected behaviour
 * would be 356 → 6 for a continuous experience. A solution will likely
 * require a new class inherited from QDoubleSpinBox or maybe
 * QAbstractSpinbox.
 * 
 * @todo Support for other models like HSL, Munsell? With an option to
 * enable or disable them? (NCS not, because it is not free.)
 * 
 * @todo Provide (on demand) two patches, like Scribus also does: One for the
 * old color (cannot be modified by the user) and another one for the new
 * color (same behaviour as the yet existing color patch).
 * 
 * @todo For the tab widget, use rather icons instead of the text “hue first”
 * and “lightness first”.
 * 
 * @todo As the aspect ration of the widget content for some of our widgets is
 * fixed (and square), we could make better use of the available space: If
 * 250 px × 350 px are available, why not resize to 250 px × 250 px, as anyway
 * we will not see a bigger pixmap, and use the free 100 px otherwise? But
 * following discussions on Internet about failing heighForWidth(), propably
 * the only way is creating a custom layout manager.
 */
class ColorDialog : public QDialog
{
    Q_OBJECT

    /** @brief Currently selected color in the dialog
     * 
     * @invariant This property is provided as an RGB value.
     * <tt>QColor::isValid()</tt> is always @c true and
     * <tt>QColor::spec()</tt> is always <tt>QColor::Spec::Rgb</tt>.
     * 
     * @invariant The signal currentColorChanged() is emitted always and only
     * when the value of this property changes.
     * 
     * @note The setter setCurrentColor() does not accept all QColor values.
     * See its documentation for details.
     * 
     * @sa READ currentColor()
     * @sa WRITE setCurrentColor()
     * @sa NOTIFY currentColorChanged()
     * @sa m_currentOpaqueColor */
    Q_PROPERTY(
        QColor currentColor
        READ currentColor
        WRITE setCurrentColor
        NOTIFY currentColorChanged
    )

    /** @brief Various options that affect the look and feel of the dialog
     * 
     * These are the same settings as for QColorDialog. For compatibility
     * reasons, they are also of the same type:
     * @c ColorDialogOption
     * 
     * | Option              | Default value | Description
     * | :------------------ | :------------ | :----------
     * | ShowAlphaChannel    | false         | Allow the user to select the alpha component of a color.
     * | NoButtons           | false         | Don't display OK and Cancel buttons. (Useful for “live dialogs”.)
     * | DontUseNativeDialog | true          | Use Qt's standard color dialog instead of the operating system native color dialog.
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
     * @code
     * myDialog->setOption(
     *     PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel,
     *     false
     * );
     * @endcode
     * Or:
     * @code
     * myDialog->setOption(
     *     QColorDialog::ShowAlphaChannel,
     *     false
     * );
     * @endcode
     * @note At difference to QColorDialog, you need a fully qualified
     * identifier for the enum values. The following code would therefore
     * fail:
     * @code
     * myDialog->setOption(
     *     ShowAlphaChannel,
     *     false
     * );
     * @endcode
     * 
     * @sa READ options()
     * @sa testOption()
     * @sa WRITE setOptions()
     * @sa setOption()
     * @sa m_options */
    Q_PROPERTY(
        ColorDialogOptions options
        READ options
        WRITE setOptions
    )

    /** @brief Layout dimensions
     * 
     * Defines if the dialog uses a rather collapsed (small) or a rather
     * expanded (large) layout.
     * 
     * Default value: @c PerceptualColor::DialogLayoutDimensions::automatic
     * 
     * When the layout dimension effectifly changes, also the dialog size
     * is adapted.
     * 
     * @sa DialogLayoutDimensions
     * @sa layoutDimensions
     * @sa setLayoutDimensions */
    Q_PROPERTY(
        DialogLayoutDimensions layoutDimensions
        READ layoutDimensions
        WRITE setLayoutDimensions
    )
        
public:
    /** @brief Local alias for QColorDialog::ColorDialogOption */
    typedef QColorDialog::ColorDialogOption ColorDialogOption;
    /** @brief Local alias for QColorDialog::ColorDialogOptions */
    typedef QColorDialog::ColorDialogOptions ColorDialogOptions;
    /** @brief Layout dimensions */
    enum class DialogLayoutDimensions {
        automatic,  /**< Decide automatically between @c collapsed and
                         @c expanded layout based on evaluating the screen
                         size. The decision is evaluated at the moment when
                         setting this value, and again each time the widget is
                         shown again. It is @em not evaluated again when a yet
                         existing dialog is just moved to another screen. */
        collapsed,  /**< Use the small, “collapsed“ layout of this dialog. */
        expanded    /**< Use the large, “expanded” layout of this dialog.  */
    };
    Q_ENUM(DialogLayoutDimensions);
    explicit ColorDialog(QWidget *parent = nullptr);
    explicit ColorDialog(const QColor &initial, QWidget *parent = nullptr);
    virtual ~ColorDialog() override;
    /** @brief Getter for property currentColor()
     *  @returns the property currentColor() */
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
    /** @brief Getter for property options()
     * @returns the current options */
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
    /** @brief Notify signal for property currentColor().
     * 
     * This signal is emitted whenever the current color changes in the
     * dialog.
     * @param color The current color */
    void currentColorChanged(const QColor &color);

protected:
    virtual void done(int result) override;

private:
    Q_DISABLE_COPY(ColorDialog)

    /** @brief Pointer to the GradientSelector for alpha. */
    AlphaSelector *m_alphaSelector;
    /** @brief Pointer to the QLabel for m_alphaSelector().
     * 
     * We store this in a
     * pointer to allow toggle the visibility later. */
    QLabel *m_alphaSelectorLabel;
    /** @brief Pointer to the QButtonBox of this dialog.
     * 
     * We store this in a pointer
     * to allow toggle the visibility later. */
    QDialogButtonBox  *m_buttonBox;
    /** @brief Pointer to the ChromaHueDiagram. */
    ChromaHueDiagram *m_chromaHueDiagram;
    /** @brief Pointer to the ColorPatch widget. */
    ColorPatch *m_colorPatch;
    /** @brief Holds the current color without alpha information
     * 
     * @note The alpha information within this data member is meaningless.
     * Ignore it. The information about the alpha channel is actually stored
     * within m_alphaSelector().
     * 
     * @sa currentColor() */
    FullColorDescription m_currentOpaqueColor;
    /** @brief Pointer to the GradientSelector for LCh lightness. */
    GradientSelector *m_lchLightnessSelector;
    /** @brief Pointer to the QLineEdit that represents the HLC value. */
    QLineEdit *m_hlcLineEdit;
    /** @brief Pointer to the QSpinbox for HSV hue. */
    QDoubleSpinBox *m_hsvHueSpinbox;
    /** @brief Pointer to the QSpinbox for HSV saturation. */
    QDoubleSpinBox *m_hsvSaturationSpinbox;
    /** @brief Pointer to the QSpinbox for HSV value. */
    QDoubleSpinBox *m_hsvValueSpinbox;
    /** @brief Holds wether currently a color change is ongoing, or not.
     * 
     * Used to avoid infinite recursions when updating the different widgets
     * within this dialog.
     * @sa setCurrentOpaqueFullColor() */
    bool m_isColorChangeInProgress = false;
    /** @brief Internal storage for property layoutDimensions() */
    PerceptualColor::ColorDialog::DialogLayoutDimensions m_layoutDimensions =
        ColorDialog::DialogLayoutDimensions::automatic;
    /** @brief Pointer to the graphical selector widget with lightness and
     *  chroma-hue selector. */
    QWidget *m_lightnessFirstWidget;
    /** @brief Holds the receiver slot (if any) to be disconnected
     *  automatically after closing the dialog.
     * 
     * Its value is only meaningfull if
     * m_receiverToBeDisconnected() is not null.
     * @sa m_receiverToBeDisconnected
     * @sa open() */
    QByteArray m_memberToBeDisconnected;
    /** @brief Pointer to the widget that holds the numeric color
     *         representation. */
    QWidget *m_numericalWidget;
    /** @brief Holds the receiver object (if any) to be disconnected
     *  automatically after closing the dialog.
     * 
     * @sa m_memberToBeDisconnected
     * @sa open() */
    QPointer<QObject> m_receiverToBeDisconnected;
    /** @brief Internal storage for property options() */
    ColorDialogOptions m_options;
    /** @brief Pointer to the QSpinbox for RGB blue. */
    QDoubleSpinBox *m_rgbBlueSpinbox;
    /** @brief Pointer to the RgbColorSpace object. */
    RgbColorSpace *m_rgbColorSpace;
    /** @brief Pointer to the QSpinbox for RGB green. */
    QDoubleSpinBox *m_rgbGreenSpinbox;
    /** @brief Pointer to the QLineEdit that represents the hexadecimal
     *  RGB value. */
    QLineEdit *m_rgbLineEdit;
    /** @brief Pointer to the QSpinbox for RGB red. */
    QDoubleSpinBox *m_rgbRedSpinbox;
    /** @brief Internal storage for selectedColor(). */
    QColor m_selectedColor;
    /** @brief Layout that holds the graphical and numeric selectors. */
    QHBoxLayout *m_selectorLayout;
    /** @brief Pointer to the tab widget. */
    QTabWidget *m_tabWidget;
    /** @brief Pointer to the WheelColorPicker widget. */
    WheelColorPicker *m_wheelColorPicker;

    void applyLayoutDimensions();
    void initialize();
    QWidget* initializeNumericPage();
    QString textForHlcLineEdit() const;

private Q_SLOTS:
    void handleFocusChange(QWidget *old, QWidget *now);
    void readHlcNumericValues();
    void readHsvNumericValues();
    void readLightnessValue();
    void readRgbHexValues();
    void readRgbNumericValues();
    void setCurrentOpaqueColor(
        const PerceptualColor::FullColorDescription &color
    );
    void setCurrentOpaqueQColor(const QColor &color);
};

}

#endif // COLORDIALOG_H
