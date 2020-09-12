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

/** @file
 * 
 * Definition of the @ref PerceptualColor::ColorDialog class and its
 * members. */

// Own header
#include "PerceptualColor/colordialog.h"

#include <QApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QRegularExpressionValidator>
#include <QScreen>
#include <QVBoxLayout>

namespace PerceptualColor {

/** @brief Constructor
 * 
 *  @param parent pointer to the parent widget, if any
 *  @post The @ref currentColor property is set to Qt::white. */
ColorDialog::ColorDialog(QWidget *parent) : QDialog(parent)
{
    initialize();
    // Calling setCurrentColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    setCurrentColor(QColor(Qt::white));
}

/** @brief Constructor
 * 
 *  @param initial the initially chosen color of the dialog
 *  @param parent pointer to the parent widget, if any
 *  @post The object is constructed and @ref setCurrentColor() is called
 *  with @em initial. See @ref setCurrentColor() for the modifications that
 *  will be applied before setting the current color. Especially, as
 *  this dialog is constructed by default without alpha support, the
 *  alpha channel of @em initial is ignored and a fully opaque color is
 *  used. */
ColorDialog::ColorDialog(const QColor &initial, QWidget *parent)
    : QDialog(parent)
{
    initialize();
    // Calling setCurrentColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    setCurrentColor(initial);
}

/** @brief Destructor */
ColorDialog::~ColorDialog()
{
    // All the layouts and widgets used here are automatically child widgets
    // of this dialog widget. Therefor they are deleted automatically.
    // Also m_rgbColorSpace is of type RgbColorSpace(), which inherits from
    // QObject, and is a child of this dialog widget, does not need to be
    // deleted manually.
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor ColorDialog::currentColor() const
{
    QColor temp;
    temp = m_currentOpaqueColor.toRgbQColor();
    temp.setAlphaF(m_alphaSelector->alpha());
    return temp;
}

/** @brief Setter for @ref currentColor property.
 * 
 * @param color the new color
 * \post The property @ref currentColor is adapted as follows:
 * - If @em color is not valid, <tt>Qt::black</tt> is used instead.
 * - If <em>color</em>'s <tt>QColor::Spec</tt> is @em not
 *   <tt>QColor::Spec::Rgb</tt> then it will be converted silently
 *   to <tt>QColor::Spec::Rgb</tt>
 * - The RGB part of @ref currentColor will be the RGB part of <tt>color</tt>.
 * - The alpha channel of @ref currentColor will be the alpha channel
 *   of <tt>color</tt> if at the moment of the function call
 *   the <tt>QColorDialog::ColorDialogOption::ShowAlphaChannel</tt> option is
 *   set. It will be fully opaque otherwise. */
void ColorDialog::setCurrentColor(const QColor& color)
{
    QColor temp;
    if (color.isValid()) {
        temp = color;
        // QColorDialog would instead call QColor.rgb() which
        // rounds to 8 bit per channel.
    } else {
        // For invalid colors same behavior as QColorDialog
        temp = QColor(Qt::black);
    }
    if (testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel)) {
        m_alphaSelector->setAlpha(color.alphaF());
    } else {
        m_alphaSelector->setAlpha(1);
    }
    setCurrentOpaqueColor(FullColorDescription(m_rgbColorSpace, temp));
}

/** @brief Opens the dialog and connects its @ref colorSelected() signal to
 * the slot specified by receiver and member.
 * 
 * The signal will be disconnected from the slot when the dialog is closed.
 * 
 * Example:
 * @snippet test/testcolordialog.cpp ColorDialog Open */
void ColorDialog::open(QObject *receiver, const char *member)
{
    connect(this, SIGNAL(colorSelected(QColor)), receiver, member);
    m_receiverToBeDisconnected = receiver;
    m_memberToBeDisconnected = member;
    QDialog::open();
}

/** @brief Convenience version of @ref setCurrentOpaqueColor(), accepting
 * QColor
 * @param color the new color. Expected to be in RGB color
 *              space (RGB, HSV etc.) */
void ColorDialog::setCurrentOpaqueQColor(const QColor& color)
{
    setCurrentOpaqueColor(FullColorDescription(m_rgbColorSpace, color));
}

/** @brief Updates the color patch widget
 * 
 * @post The color page widget will show the color of
 * @ref m_currentOpaqueColor and the alpha value of
 * @ref m_alphaSelector() as available with
 * @ref AlphaSelector::alpha(). */
void ColorDialog::updateColorPatch()
{
    QColor tempRgbQColor = m_currentOpaqueColor.toRgbQColor();
    tempRgbQColor.setAlphaF(m_alphaSelector->alpha());
    m_colorPatch->setColor(tempRgbQColor);
}

/** @brief Updates @ref m_currentOpaqueColor and all affected widgets.
 * 
 * This function ignores the alpha component!
 * @param color the new color
 * @post If color is invalid, nothing happens. If this function is called
 * recursively, nothing happens. Else @ref m_currentOpaqueColor is updated,
 * and the corresponding widgets are updated.
 * @note Recursive functions calls are ignored. This is useful, because you
 * can connect signals from various widgets to this slot without having to
 * worry about infinite recursions. */
void ColorDialog::setCurrentOpaqueColor(const FullColorDescription& color)
{
    if (m_isColorChangeInProgress
        || (!color.isValid())
        || (color == m_currentOpaqueColor)
    ) {
        // Nothing to do!
        return;
    }

    // If we have really work to do, block recursive calls of this function
    m_isColorChangeInProgress = true;

    // Save currentColor() for later comparison
    // Using currentColor() makes sure correct alpha treatment!
    QColor oldQColor = currentColor();

    // Update m_currentOpaqueColor
    m_currentOpaqueColor = color;

    // Update all the widgets for opaque color…
    QColor tempRgbQColor = color.toRgbQColor();
    tempRgbQColor.setAlpha(255);
    m_rgbRedSpinbox->setValue(tempRgbQColor.redF() * 255);
    m_rgbGreenSpinbox->setValue(tempRgbQColor.greenF() * 255);
    m_rgbBlueSpinbox->setValue(tempRgbQColor.blueF() * 255);
    m_hsvHueSpinbox->setValue(color.toHsvQColor().hsvHueF() * 360);
    m_hsvSaturationSpinbox->setValue(
        color.toHsvQColor().hsvSaturationF() * 255
    );
    m_hsvValueSpinbox->setValue(color.toHsvQColor().valueF() * 255);
    m_hlcLineEdit->setText(textForHlcLineEdit());
    m_rgbLineEdit->setText(m_currentOpaqueColor.toRgbHexString());
    m_lchLightnessSelector->setFraction(
        color.toLch().L / static_cast<qreal>(100)
    );
    m_chromaHueDiagram->setColor(color);
    m_wheelColorPicker->setCurrentColor(m_currentOpaqueColor);
    m_alphaSelector->setColor(m_currentOpaqueColor);

    // Update widgets that take alpha information
    updateColorPatch();

    // Emit signal currentColorChanged() only if necessary
    if (currentColor() != oldQColor) {
        Q_EMIT currentColorChanged(currentColor());
    }

    // End of this function. Unblock resursive
    // function calls before returning.
    m_isColorChangeInProgress = false;
}

/** @brief Reads the value from the lightness selector in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::readLightnessValue()
{
    cmsCIELCh lch = m_currentOpaqueColor.toLch();
    lch.L = m_lchLightnessSelector->fraction() * 100;
    setCurrentOpaqueColor(
        FullColorDescription(
            m_rgbColorSpace,
            lch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
    );
}

/** @brief Reads the HSV numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::readHsvNumericValues()
{
    setCurrentOpaqueQColor(
        QColor::fromHsvF(
            m_hsvHueSpinbox->value() / static_cast<qreal>(360),
            m_hsvSaturationSpinbox->value() / static_cast<qreal>(255),
            m_hsvValueSpinbox->value() / static_cast<qreal>(255)
        )
    );
}

/** @brief Reads the decimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::readRgbNumericValues()
{
    setCurrentOpaqueQColor(
        QColor::fromRgbF(
            m_rgbRedSpinbox->value() / static_cast<qreal>(255),
            m_rgbGreenSpinbox->value() / static_cast<qreal>(255),
            m_rgbBlueSpinbox->value() / static_cast<qreal>(255)
        )
    );
}

/** @brief Reads the hexadecimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::readRgbHexValues()
{
    QString temp = m_rgbLineEdit->text();
    if (!temp.startsWith('#')) {
        temp = QString('#') + temp;
    }
    QColor rgb;
    rgb.setNamedColor(temp);
    if (rgb.isValid()) {
        setCurrentOpaqueQColor(rgb);
    }
    // Return to the finally considered value (the new one if valid, the old
    // one otherwise.
    m_rgbLineEdit->setText(m_currentOpaqueColor.toRgbHexString());
}

/** @brief Basic initialization.
 * 
 * Code that is shared between the various overloaded constructors. */
void ColorDialog::initialize()
{
    // initialize color space
    m_rgbColorSpace = new RgbColorSpace(this);

    // create the graphical selectors
    m_wheelColorPicker = new WheelColorPicker(m_rgbColorSpace);
    QWidget *tempHueFirstWidget = new QWidget;
    QHBoxLayout *tempHueFirstLayout = new QHBoxLayout;
    tempHueFirstLayout->addWidget(m_wheelColorPicker);
    tempHueFirstWidget->setLayout(tempHueFirstLayout);
    m_lchLightnessSelector = new GradientSelector(m_rgbColorSpace);
    cmsCIELCh black;
    black.L = 0;
    black.C = 0;
    black.h = 0;
    cmsCIELCh white;
    white.L = 100;
    white.C = 0;
    white.h = 0;
    m_lchLightnessSelector->setColors(
        FullColorDescription(
            m_rgbColorSpace,
            black,
            PerceptualColor::FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        ),
        FullColorDescription(
            m_rgbColorSpace,
            white,
            PerceptualColor::FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        )
    );
    m_chromaHueDiagram = new ChromaHueDiagram(m_rgbColorSpace);
    QHBoxLayout *tempLightnesFirstLayout = new QHBoxLayout();
    tempLightnesFirstLayout->addWidget(m_lchLightnessSelector);
    tempLightnesFirstLayout->addWidget(m_chromaHueDiagram);
    m_lightnessFirstWidget = new QWidget();
    m_lightnessFirstWidget->setLayout(tempLightnesFirstLayout);
    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(
        tempHueFirstWidget,
        tr("&Hue first")
    );
    m_tabWidget->addTab(
        m_lightnessFirstWidget,
        tr("&Lightness first")
    );
    // create the ColorPatch
    m_colorPatch = new ColorPatch();

    // Create widget for the numerical values
    m_numericalWidget = initializeNumericPage();

    // Create layout for graphical and numerical widgets
    m_selectorLayout = new QHBoxLayout();
    m_selectorLayout->addWidget(m_tabWidget);
    m_selectorLayout->addWidget(m_numericalWidget);

    // Create alpha selector
    m_alphaSelector = new AlphaSelector(m_rgbColorSpace);
    QFormLayout *tempAlphaLayout = new QFormLayout();
    m_alphaSelectorLabel = new QLabel(tr("O&pacity:"));
    m_alphaSelector->registerAsBuddy(m_alphaSelectorLabel);
    tempAlphaLayout->addRow(m_alphaSelectorLabel, m_alphaSelector);

    // Create the default buttons
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );
    connect(
        m_buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &PerceptualColor::ColorDialog::accept
    );
    connect(
        m_buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &PerceptualColor::ColorDialog::reject
    );

    // Create the main layout
    QVBoxLayout *tempMainLayout = new QVBoxLayout();
    tempMainLayout->addLayout(m_selectorLayout);
    tempMainLayout->addLayout(tempAlphaLayout);
    tempMainLayout->addWidget(m_colorPatch);
    tempMainLayout->addWidget(m_buttonBox);
    setLayout(tempMainLayout);
    
    // initialize signal-slot-connections
    connect(
        m_rgbRedSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readRgbNumericValues
    );
    connect(
        m_rgbGreenSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readRgbNumericValues
    );
    connect(
        m_rgbBlueSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readRgbNumericValues
    );
    connect(
        m_rgbLineEdit,
        &QLineEdit::editingFinished,
        this,
        &ColorDialog::readRgbHexValues
    );
    connect(
        m_hsvHueSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readHsvNumericValues
    );
    connect(
        m_hsvSaturationSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readHsvNumericValues
    );
    connect(
        m_hsvValueSpinbox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &ColorDialog::readHsvNumericValues
    );
    connect(
        qApp,
        &QApplication::focusChanged,
        this,
        &ColorDialog::handleFocusChange
    );
    connect(
        m_hlcLineEdit,
        &QLineEdit::returnPressed,
        this,
        &ColorDialog::readHlcNumericValues
    );
    connect(
        m_lchLightnessSelector,
        &GradientSelector::fractionChanged,
        this,
        &ColorDialog::readLightnessValue
    );
    connect(
        m_wheelColorPicker,
        &WheelColorPicker::currentColorChanged,
        this,
        &ColorDialog::setCurrentOpaqueColor
    );
    connect(
        m_chromaHueDiagram,
        &ChromaHueDiagram::colorChanged,
        this,
        &ColorDialog::setCurrentOpaqueColor
    );
    connect(
        m_alphaSelector,
        &AlphaSelector::alphaChanged,
        this,
        &ColorDialog::updateColorPatch
    );

    // initialize the options
    setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);
    
    // initialize the window title
    setWindowTitle(tr("Select Color"));
}

/** @brief Get text for @ref m_hlcLineEdit based on @ref m_currentOpaqueColor
 * @returns A QString appropriate for @ref m_hlcLineEdit */
QString ColorDialog::textForHlcLineEdit() const
{
    return
        QString(QStringLiteral(u"%1 %2 %3"))
            .arg(m_currentOpaqueColor.toLch().h, 0, 'f', 0)
            .arg(m_currentOpaqueColor.toLch().L, 0, 'f', 0)
            .arg(m_currentOpaqueColor.toLch().C, 0, 'f', 0);
}

/** @brief React on focus changes
 * 
 * This function tests if the focus is leaving @ref m_hlcLineEdit(). If so, it
 * will update the other widgets if necessary. We have to to this because
 * @ref m_hlcLineEdit is a QLineEdit and its editingFinished() will not be
 * emitted if the current value is not conform to the input mask and the
 * validator. So we might miss (invalid) value changes. It is nevertheless
 * important to catch these cases, because the widget must be reset to a valid
 * value. */
void ColorDialog::handleFocusChange(QWidget *old/*, QWidget *now*/)
{
    if (old == m_hlcLineEdit) {
        if (m_hlcLineEdit->text() != textForHlcLineEdit()) {
            // Only read-in the values if really changed. That's important
            // because otherwise, passing just with the focus through the
            // m_hlcLineEdit widget, each time the value would be read-in.
            // This would lead to rounding errors: L might be 50.8 before,
            // but displayed is just 51. At read-in, it would be changed from
            // 50.8 to 51, which is undesirable except the user has really
            // changed a value in this widget.
            readHlcNumericValues();
        }
    }
}

/** @brief Reads the HLC numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::readHlcNumericValues()
{
    cmsCIELCh lch;
    QStringList temp = m_hlcLineEdit->text().split(QStringLiteral(u" "));
    if (temp.size() == 3) {
        lch.h = temp.at(0).toInt();
        lch.L = qMin(temp.at(1).toInt(), 100);
        lch.C = temp.at(2).toInt();
        setCurrentOpaqueColor(
            FullColorDescription(
                m_rgbColorSpace,
                lch,
                FullColorDescription::outOfGamutBehaviour::sacrifyChroma
            )
        );
    } else {
        m_hlcLineEdit->setText(textForHlcLineEdit());
    }
}

/** @brief Initialize the numeric input widgets of this dialog.
 * @returns A pointer to a new widget that has the other, numeric input
 * widgets as child widgets. */
QWidget* ColorDialog::initializeNumericPage()
{
    // Create HSV spin boxes
    constexpr int hsvDecimals = 0;
    QHBoxLayout *tempHsvLayout = new QHBoxLayout;
    m_hsvHueSpinbox = new QDoubleSpinBox();
    m_hsvHueSpinbox->setAlignment(Qt::AlignRight);
    m_hsvHueSpinbox->setMaximum(360);
    m_hsvHueSpinbox->setWrapping(true);
    m_hsvHueSpinbox->setDecimals(hsvDecimals);
    m_hsvHueSpinbox->setWhatsThis(
        tr("<p>Hue</p><p>Range: 0–360</p>")
    );
    m_hsvSaturationSpinbox = new QDoubleSpinBox();
    m_hsvSaturationSpinbox->setAlignment(Qt::AlignRight);
    m_hsvSaturationSpinbox->setMaximum(255);
    m_hsvSaturationSpinbox->setDecimals(hsvDecimals);
    m_hsvSaturationSpinbox->setWhatsThis(
        tr("<p>Saturation</p><p>Range: 0–255</p>")
    );
    m_hsvValueSpinbox = new QDoubleSpinBox();
    m_hsvValueSpinbox->setAlignment(Qt::AlignRight);
    m_hsvValueSpinbox->setMaximum(255);
    m_hsvValueSpinbox->setDecimals(hsvDecimals);
    m_hsvValueSpinbox->setWhatsThis(
        tr("<p>Brightness/Value</p><p>Range: 0–255</p>")
    );
    tempHsvLayout->addWidget(m_hsvHueSpinbox);
    tempHsvLayout->addWidget(m_hsvSaturationSpinbox);
    tempHsvLayout->addWidget(m_hsvValueSpinbox);

    // Create RGB spin boxes
    constexpr int rgbDecimals = 0;
    QHBoxLayout *tempRgbLayout = new QHBoxLayout;
    m_rgbRedSpinbox = new QDoubleSpinBox();
    m_rgbRedSpinbox->setAlignment(Qt::AlignRight);
    m_rgbRedSpinbox->setMaximum(255);
    m_rgbRedSpinbox->setDecimals(rgbDecimals);
    m_rgbRedSpinbox->setWhatsThis(
        tr("<p>Red</p><p>Range: 0–255</p>")
    );
    m_rgbGreenSpinbox = new QDoubleSpinBox();
    m_rgbGreenSpinbox->setAlignment(Qt::AlignRight);
    m_rgbGreenSpinbox->setMaximum(255);
    m_rgbGreenSpinbox->setDecimals(rgbDecimals);
    m_rgbGreenSpinbox->setWhatsThis(
        tr("<p>Green</p><p>Range: 0–255</p>")
    );
    m_rgbBlueSpinbox = new QDoubleSpinBox();
    m_rgbBlueSpinbox->setAlignment(Qt::AlignRight);
    m_rgbBlueSpinbox->setMaximum(255);
    m_rgbBlueSpinbox->setDecimals(rgbDecimals);
    m_rgbBlueSpinbox->setWhatsThis(
        tr("<p>Blue</p><p>Range: 0–255</p>")
    );
    tempRgbLayout->addWidget(m_rgbRedSpinbox);
    tempRgbLayout->addWidget(m_rgbGreenSpinbox);
    tempRgbLayout->addWidget(m_rgbBlueSpinbox);

    // Create widget for the hex style color representation
    m_rgbLineEdit = new QLineEdit();
    m_rgbLineEdit->setMaxLength(7);
    QRegularExpression tempRegularExpression(
        QStringLiteral(u"#?[0-9A-Fa-f]{0,6}")
    );
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(
        tempRegularExpression,
        this
    );
    m_rgbLineEdit->setValidator(validator);
    m_rgbLineEdit->setWhatsThis(
        tr(
            "<p>Hexadecimal color code, as used in HTML</p>"
            "<p>#RRGGBB</p>"
            "<ul>RR: two-digit code for red</ul>"
            "<ul>GG: two-digit code for green</ul>"
            "<ul>BB: two-digit code for blue</ul>"
            "Range of each color: 00–FF"
        )
    );
    
    // Create widget for the HLC color representation
    m_hlcLineEdit = new QLineEdit();
    QRegularExpression expression {
        QStringLiteral(u"\\d{1,3}\\s\\d{1,3}\\s\\d{1,3}")
    };
    m_hlcLineEdit->setValidator(
        new QRegularExpressionValidator(expression, this)
    );

    // Create global layout
    QFormLayout *tempNumericPageFormLayout = new QFormLayout();
    QLabel *tempRgbLabel = new QLabel(tr("&RGB"));
    tempRgbLabel->setBuddy(m_rgbRedSpinbox);
    tempNumericPageFormLayout->addRow(tempRgbLabel, tempRgbLayout);
    tempNumericPageFormLayout->addRow(tr("He&x"), m_rgbLineEdit);
    QLabel *tempHsvLabel = new QLabel(tr("HS&V"));
    tempHsvLabel->setBuddy(m_hsvHueSpinbox);
    tempNumericPageFormLayout->addRow(tempHsvLabel, tempHsvLayout);
    tempNumericPageFormLayout->addRow(tr("HL&C"), m_hlcLineEdit);
    
    // Create a global widget using the previously created global layout
    QWidget *tempWidget = new QWidget;
    tempWidget->setLayout(tempNumericPageFormLayout);
    tempWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    
    // Return
    return tempWidget;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColorDialog::ColorDialogOptions ColorDialog::options() const
{
    return m_options;
}

/** @brief Setter for @ref options.
 * 
 * Sets a value for just one single option within @ref options.
 * @param option the option to set
 * @param on the new value of the option
 */
void ColorDialog::setOption(QColorDialog::ColorDialogOption option, bool on)
{
    QColorDialog::ColorDialogOptions temp = m_options;
    temp.setFlag(option, on);
    setOptions(temp);
}

/** @brief Setter for @ref options */
void ColorDialog::setOptions(QColorDialog::ColorDialogOptions options)
{
    // Save the new options
    m_options = options;
    // Correct QColorDialog::ColorDialogOption::DontUseNativeDialog
    // which must be always on
    m_options.setFlag(
        QColorDialog::ColorDialogOption::DontUseNativeDialog,
        true
    );

    // Apply the new options
    m_alphaSelectorLabel->setVisible(
        m_options.testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel)
    );
    m_alphaSelector->setVisible(
        m_options.testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel)
    );
    m_buttonBox->setVisible(
        !m_options.testFlag(QColorDialog::ColorDialogOption::NoButtons)
    );
}

/** @brief Getter for @ref options
 * 
 * Gets the value of just one single option within @ref options.
 * 
 * @param option the requested option
 * @returns the value of the requested option
 */
bool ColorDialog::testOption(QColorDialog::ColorDialogOption option) const
{
    return m_options.testFlag(option);
}

/** @brief Pops up a modal color dialog, lets the user choose a color, and
 *  returns that color.
 * 
 * @param initial initial value for currentColor()
 * @param parent  parent widget of the dialog (or 0 for no parent)
 * @param title   window title (or an empty string for the default window
 *                title)
 * @param options the options() for customizing the look and feel of the
 *                dialog
 * @returns       selectedColor(): The color the user has selected; or an
 *                invalid color if the user has canceled the dialog. */
QColor ColorDialog::getColor(
    const QColor &initial,
    QWidget *parent,
    const QString &title,
    QColorDialog::ColorDialogOptions options
)
{
    ColorDialog temp(parent);
    if (!title.isEmpty()) {
        temp.setWindowTitle(title);
    }
    temp.setOptions(options);
    // setCurrentColor() must be after setOptions()
    // to allow alpha channel support
    temp.setCurrentColor(initial);
    temp.exec();
    return temp.selectedColor();
}

/** @brief The color that was actually selected by the user.
 * 
 * At difference to the @ref currentColor property, this function provides
 * the color that was actually selected by the user by clicking the OK button
 * or pressing the return key or another equivalent action.
 * 
 * This function most useful to get the actually selected color @em after that
 * the dialog has been closed.
 * 
 * When a dialog that had been closed or hidden is shown again,
 * this function returns to an invalid QColor().
 * 
 * @returns Just after showing the dialog, the value is an invalid QColor. If
 * the user selects a color by clicking the OK button or another equivalent
 * action, the value is the selected color. If the user cancels the dialog
 * (Cancel button, or by pressing the Escape key), the value remains an
 * invalid QColor. */
QColor ColorDialog::selectedColor() const
{
    return m_selectedColor;
}

/** @brief Setter for property @em visible
 * 
 * Reimplemented from base class.
 * 
 * When a dialog, that wasn't formerly visible, gets visible,
 * it's @ref m_selectedColor is cleared.
 * 
 * @param visible holds whether or not the dialog should be visible */
void ColorDialog::setVisible(bool visible)
{
    if (visible && (!isVisible())) {
        // Only delete the selected color if the dialog wasn’t visible before
        // and will be made visible now.
        m_selectedColor = QColor();
        applyLayoutDimensions();
    }
    QDialog::setVisible(visible);
}

/** @brief Various updates when closing the dialog.
 * 
 * Reimplemented from base class.
 * @param result The result with which the dialog has been closed */
void ColorDialog::done(int result)
{
    if (result == QDialog::DialogCode::Accepted) {
        m_selectedColor = currentColor();
        Q_EMIT colorSelected(m_selectedColor);
    } else {
        m_selectedColor = QColor();
    }
    QDialog::done(result);
    if (m_receiverToBeDisconnected) {
        disconnect(
            this,
            SIGNAL(colorSelected(QColor)),
            m_receiverToBeDisconnected,
            m_memberToBeDisconnected
        );
        m_receiverToBeDisconnected = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
ColorDialog::DialogLayoutDimensions ColorDialog::layoutDimensions() const
{
    return m_layoutDimensions;
}

/** @brief Setter for property @ref layoutDimensions */
void ColorDialog::setLayoutDimensions(
    const ColorDialog::DialogLayoutDimensions newLayoutDimensions
)
{
    m_layoutDimensions = newLayoutDimensions;
    applyLayoutDimensions();
}

/** @brief Arranges the layout conforming to @ref layoutDimensions
 * 
 * If @ref layoutDimensions is DialogLayoutDimensions::automatic than it is
 * first evaluated again if for the current display the collapsed or the
 * expanded layout is used. */
void ColorDialog::applyLayoutDimensions()
{
    bool collapsedLayout; // true for small layout, false for large layout.
    int effectivelyAvailableScreenWidth;
    int widthThreeshold;
    switch (m_layoutDimensions) {
        case DialogLayoutDimensions::collapsed:
            collapsedLayout = true;
            break;
        case DialogLayoutDimensions::expanded:
            collapsedLayout = false;
            break;
        case DialogLayoutDimensions::automatic:
            // Note: The following code works correctly on scaled
            // devices (high-DPI…).
            
            // We should not use more than 70% of the screen for a dialog.
            // That's roughly the same as the default maximum sizes for
            // a QDialog.
            effectivelyAvailableScreenWidth = qRound(
                QGuiApplication::primaryScreen()->availableSize().width() * 0.7
            );

            // Now we calculate the space we need for displaying the
            // graphical selectors and the numerical selector at their
            // preferred size in an expanded layout.
            // Start with the size of the graphical selectors.
            widthThreeshold = qMax(
                m_wheelColorPicker->sizeHint().width(),
                m_lightnessFirstWidget->sizeHint().width()
            );
            // Add the size of the numerical selector.
            widthThreeshold += m_numericalWidget->sizeHint().width();
            // Add some space for margins.
            widthThreeshold = qRound(widthThreeshold * 1.2);

            // Now decide between collapsed layout and expanded layout
            collapsedLayout = (
                effectivelyAvailableScreenWidth < widthThreeshold
            );
            break;
        default:
            throw;
    }

    if (collapsedLayout) {
        if (m_selectorLayout->indexOf(m_numericalWidget) >= 0) {
            // Indeed we have expanded layout and have to switch to
            // collapsed layout…
            m_tabWidget->addTab(m_numericalWidget, tr("&Numeric"));
            // We don’t call m_numericalWidget->show(); because this
            // is controlled by the QTabWidget.
            adjustSize(); // Adopt size of dialog to new layout’s size hint
        }
    } else {
        if (m_selectorLayout->indexOf(m_numericalWidget) < 0) {
            // Indeed we have collapsed layout and have to switch to
            // expanded layout…
            m_selectorLayout->addWidget(m_numericalWidget);
            // We call show because the widget is hidden by removing it
            // from its old parent, and needs to be shown explicitly.
            m_numericalWidget->show();
            adjustSize(); // Adopt size of dialog to new layout’s size hint
        }
    }
}

}
