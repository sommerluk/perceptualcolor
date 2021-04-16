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

#include "perceptualcolorlib_internal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/colordialog.h"
// Second, the private implementation.
#include "colordialog_p.h"

#include <QAction>
#include <QFormLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QRegularExpressionValidator>
#include <QScreen>
#include <QStyle>
#include <QVBoxLayout>

#include "fallbackiconengine.h"
#include "helper.h"
#include "lchvalues.h"

namespace PerceptualColor
{
/** @brief Constructor
 *
 *  @param parent pointer to the parent widget, if any
 *  @post The @ref currentColor property is set to a default value. */
ColorDialog::ColorDialog(QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize();
    // As initial color, QColorDialog chooses white, probably because
    // it’t quite neutral.
    // We do things differently here: We want a color that has a chroma,
    // because this way, new users do not see the widget diagrams at an
    // extreme position, but somewhere in the middle. This is important,
    // because extreme positions like “white” leave for example the
    // chroma-hue-diagram at zero surface. Therefore, we try a middle
    // lightness and a middle chroma (a lightness-chroma-combination that
    // will be valid through the hole range of hue. For the hue, the natural
    // choice would be 0°, which is red. Why choosing something different
    // from 0° without a technical reason? If the lightness is 50%, among 0°,
    // 90°, 180° and 270°, it’s 180° that has the lowest possible maximum C.
    // Therefore, we choose 180°, because at a given chroma, the resulting
    // color is more vivid than those at 0°, 90° and 270°. Also, be choose
    // 180° because the color seems clean and colorful.
    LchDouble initialColor;
    initialColor.h = 180;
    initialColor.l = LchValues::neutralLightness;
    initialColor.c = LchValues::srgbVersatileChroma;
    // Calling setCurrentFullColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    LchDouble lch = d_pointer->m_rgbColorSpace->nearestInGamutSacrifyingChroma(
        initialColor);
    LchaDouble lcha;
    lcha.l = lch.l;
    lcha.c = lch.c;
    lcha.h = lch.h;
    lcha.a = 1;
    d_pointer->setCurrentFullColor(lcha);
}

/** @brief Constructor
 *
 *  @param initial the initially chosen color of the dialog
 *  @param parent pointer to the parent widget, if any
 *  @post The object is constructed and @ref setCurrentColor() is called
 *  with <em>initial</em>. See @ref setCurrentColor() for the modifications
 *  that will be applied before setting the current color. Especially, as
 *  this dialog is constructed by default without alpha support, the
 *  alpha channel of <em>initial</em> is ignored and a fully opaque color is
 *  used. */
ColorDialog::ColorDialog(const QColor &initial, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize();
    // Calling setCurrentColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    setCurrentColor(initial);
}

/** @brief Destructor */
ColorDialog::~ColorDialog() noexcept
{
    // All the layouts and widgets used here are automatically child widgets
    // of this dialog widget. Therefor they are deleted automatically.
    // Also m_rgbColorSpace is of type RgbColorSpace(), which
    // inherits from QObject, and is a child of this dialog widget, does
    // not need to be deleted manually.
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
ColorDialog::ColorDialogPrivate::ColorDialogPrivate(ColorDialog *backLink)
    : q_pointer(backLink)
{
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor ColorDialog::currentColor() const
{
    QColor temp;
    temp = d_pointer->m_rgbColorSpace->colorRgbBound(
        d_pointer->m_currentOpaqueColor);
    temp.setAlphaF(d_pointer->m_alphaGradientSlider->value());
    return temp;
}

/** @brief Setter for @ref currentColor property.
 *
 * @param color the new color
 * @post The property @ref currentColor is adapted as follows:
 * - If <em>color</em> is not valid, <tt>Qt::black</tt> is used instead.
 * - If <em>color</em>'s <tt>QColor::Spec</tt> is <em>not</em>
 *   <tt>QColor::Spec::Rgb</tt> then it will be converted silently
 *   to <tt>QColor::Spec::Rgb</tt>
 * - The RGB part of @ref currentColor will be the RGB part of <tt>color</tt>.
 * - The alpha channel of @ref currentColor will be the alpha channel
 *   of <tt>color</tt> if at the moment of the function call
 *   the <tt>QColorDialog::ColorDialogOption::ShowAlphaChannel</tt> option is
 *   set. It will be fully opaque otherwise. */
void ColorDialog::setCurrentColor(const QColor &color)
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
    LchDouble lch = d_pointer->m_rgbColorSpace->colorLch(temp);
    LchaDouble lcha;
    lcha.l = lch.l;
    lcha.c = lch.c;
    lcha.h = lch.h;
    lcha.a = temp.alphaF();
    d_pointer->setCurrentFullColor(lcha);
}

/** @brief Sets the @ref currentColor property.
 *
 * @param color The new color to set. The alpha value is taken
 * into account. */
void ColorDialog::ColorDialogPrivate::setCurrentFullColor(
    const LchaDouble &color)
{
    qreal myAlphaF;
    if (q_pointer->testOption(ColorDialogOption::ShowAlphaChannel)) {
        myAlphaF = color.a;
    } else {
        myAlphaF = 1;
    }
    m_alphaGradientSlider->setValue(myAlphaF);
    // No need to update m_alphaSpinBox is this is done
    // automatically by signals emitted by m_alphaGradientSlider.
    LchDouble lch;
    lch.l = color.l;
    lch.c = color.c;
    lch.h = color.h;
    setCurrentOpaqueColor(lch);
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
    d_pointer->m_receiverToBeDisconnected = receiver;
    d_pointer->m_memberToBeDisconnected = member;
    QDialog::open();
}

/** @brief Convenience version of @ref setCurrentOpaqueColor(), accepting
 * QColor
 * @param color the new color. Expected to be in RGB color
 *              space (RGB, HSV etc.) */
void ColorDialog::ColorDialogPrivate::setCurrentOpaqueQColor(
    const QColor &color)
{
    setCurrentOpaqueColor(m_rgbColorSpace->colorLch(color));
}

/** @brief Updates the color patch widget
 *
 * @post The color patch widget will show the color
 * of @ref m_currentOpaqueColor and the alpha
 * value of @ref m_alphaGradientSlider. */
void ColorDialog::ColorDialogPrivate::updateColorPatch()
{
    QColor tempRgbQColor = m_rgbColorSpace->colorRgbBound(m_currentOpaqueColor);
    tempRgbQColor.setAlphaF(m_alphaGradientSlider->value());
    m_colorPatch->setColor(tempRgbQColor);
}

/** @brief Updates @ref m_currentOpaqueColor and all affected widgets.
 *
 * This function ignores the alpha component!
 * @param color the new color
 * @post If this function is called recursively, nothing happens. Else
 * the color is moved into the gamut, then @ref m_currentOpaqueColor is
 * updated, and the corresponding widgets are updated.
 * @note Recursive functions calls are ignored. This is useful, because you
 * can connect signals from various widgets to this slot without having to
 * worry about infinite recursions. */
void ColorDialog::ColorDialogPrivate::setCurrentOpaqueColor(
    const LchDouble &color)
{
    if (m_isColorChangeInProgress ||
        (color.hasSameCoordinates(m_currentOpaqueColor))) {
        // Nothing to do!
        return;
    }

    // If we have really work to do, block recursive calls of this function
    m_isColorChangeInProgress = true;

    // Save currentColor() for later comparison
    // Using currentColor() makes sure correct alpha treatment!
    QColor oldQColor = q_pointer->currentColor();

    // Update m_currentOpaqueColor
    m_currentOpaqueColor = color;

    // Update all the widgets for opaque color…
    QColor tempRgbQColor = m_rgbColorSpace->colorRgbBound(color);
    tempRgbQColor.setAlpha(255);
    QList<MultiSpinBox::SectionData> tempSections;

    // Update RGB widget
    tempSections = m_rgbSpinBox->sections();
    tempSections[0].value = tempRgbQColor.redF() * 255;
    tempSections[1].value = tempRgbQColor.greenF() * 255;
    tempSections[2].value = tempRgbQColor.blueF() * 255;
    m_rgbSpinBox->setSections(tempSections);

    // Update HSV widget
    tempSections = m_hsvSpinBox->sections();
    tempSections[0].value = tempRgbQColor.hsvHueF() * 360;
    tempSections[1].value = tempRgbQColor.hsvSaturationF() * 255;
    tempSections[2].value = tempRgbQColor.valueF() * 255;
    m_hsvSpinBox->setSections(tempSections);

    // Update HLC widget
    tempSections = m_hlcSpinBox->sections();
    tempSections[0].value = m_currentOpaqueColor.h;
    tempSections[1].value = m_currentOpaqueColor.l;
    tempSections[2].value = m_currentOpaqueColor.c;
    m_hlcSpinBox->setSections(tempSections);

    // Update RGB hex widget
    m_rgbLineEdit->setText(tempRgbQColor.name());

    // Update the diagrams
    m_lchLightnessSelector->setValue(color.l / static_cast<qreal>(100));
    m_chromaHueDiagram->setCurrentColor(color);
    m_wheelColorPicker->setCurrentColor(m_currentOpaqueColor);

    // Update alpha
    LchaDouble tempColor;
    tempColor.l = m_currentOpaqueColor.l;
    tempColor.c = m_currentOpaqueColor.c;
    tempColor.h = m_currentOpaqueColor.h;
    tempColor.a = 0;
    m_alphaGradientSlider->setFirstColor(tempColor);
    tempColor.a = 1;
    m_alphaGradientSlider->setSecondColor(tempColor);

    // Update widgets that take alpha information
    updateColorPatch();

    // Emit signal currentColorChanged() only if necessary
    if (q_pointer->currentColor() != oldQColor) {
        Q_EMIT q_pointer->currentColorChanged(q_pointer->currentColor());
    }

    // End of this function. Unblock resursive
    // function calls before returning.
    m_isColorChangeInProgress = false;
}

/** @brief Reads the value from the lightness selector in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readLightnessValue()
{
    LchDouble lch = m_currentOpaqueColor;
    lch.l = m_lchLightnessSelector->value() * 100;
    setCurrentOpaqueColor(m_rgbColorSpace->nearestInGamutSacrifyingChroma(lch));
}

/** @brief Reads the HSV numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readHsvNumericValues()
{
    QList<MultiSpinBox::SectionData> hsvSections = m_hsvSpinBox->sections();
    setCurrentOpaqueQColor(
        QColor::fromHsvF(hsvSections[0].value / static_cast<qreal>(360),
                         hsvSections[1].value / static_cast<qreal>(255),
                         hsvSections[2].value / static_cast<qreal>(255)));
}

/** @brief Reads the decimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readRgbNumericValues()
{
    QList<MultiSpinBox::SectionData> rgbSections = m_rgbSpinBox->sections();
    setCurrentOpaqueQColor(
        QColor::fromRgbF(rgbSections[0].value / static_cast<qreal>(255),
                         rgbSections[1].value / static_cast<qreal>(255),
                         rgbSections[2].value / static_cast<qreal>(255)));
}

/** @brief Reads the hexadecimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readRgbHexValues()
{
    QString temp = m_rgbLineEdit->text();
    if (!temp.startsWith(QStringLiteral(u"#"))) {
        temp = QStringLiteral(u"#") + temp;
    }
    QColor rgb;
    rgb.setNamedColor(temp);
    if (rgb.isValid()) {
        setCurrentOpaqueQColor(rgb);
    }
    // Return to the finally considered value (the new one if valid, the old
    // one otherwise.)
    m_rgbLineEdit->setText(
        m_rgbColorSpace->colorRgb(m_currentOpaqueColor).name());
}

/** @brief Basic initialization.
 *
 * Code that is shared between the various overloaded constructors. */
void ColorDialog::ColorDialogPrivate::initialize()
{
    // initialize color space
    m_rgbColorSpace.reset(new RgbColorSpace());

    // create the graphical selectors
    m_wheelColorPicker = new WheelColorPicker(m_rgbColorSpace);
    QWidget *tempHueFirstWidget = new QWidget;
    QHBoxLayout *tempHueFirstLayout = new QHBoxLayout;
    tempHueFirstLayout->addWidget(m_wheelColorPicker);
    tempHueFirstWidget->setLayout(tempHueFirstLayout);
    m_lchLightnessSelector = new GradientSlider(m_rgbColorSpace);
    LchaDouble black;
    black.l = 0;
    black.c = 0;
    black.h = 0;
    black.a = 1;
    LchaDouble white;
    white.l = 100;
    white.c = 0;
    white.h = 0;
    white.a = 1;
    m_lchLightnessSelector->setColors(black, white);
    m_chromaHueDiagram = new ChromaHueDiagram(m_rgbColorSpace);
    QHBoxLayout *tempLightnesFirstLayout = new QHBoxLayout();
    tempLightnesFirstLayout->addWidget(m_lchLightnessSelector);
    tempLightnesFirstLayout->addWidget(m_chromaHueDiagram);
    m_lightnessFirstWidget = new QWidget();
    m_lightnessFirstWidget->setLayout(tempLightnesFirstLayout);
    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(tempHueFirstWidget, tr("&Hue first"));
    m_tabWidget->addTab(m_lightnessFirstWidget, tr("&Lightness first"));

    // Create the ColorPatch
    m_colorPatch = new ColorPatch();
    m_colorPatch->setMinimumSize(m_colorPatch->minimumSizeHint() * 1.5);

    // Create widget for the numerical values
    m_numericalWidget = initializeNumericPage();

    // Create layout for graphical and numerical widgets
    m_selectorLayout = new QHBoxLayout();
    m_selectorLayout->addWidget(m_tabWidget);
    m_selectorLayout->addWidget(m_numericalWidget);

    // Create widgets for alpha value
    QHBoxLayout *m_alphaLayout = new QHBoxLayout();
    m_alphaGradientSlider =
        new GradientSlider(m_rgbColorSpace, Qt::Orientation::Horizontal);
    m_alphaGradientSlider->setSingleStep(singleStepAlpha);
    m_alphaGradientSlider->setPageStep(pageStepAlpha);
    m_alphaSpinBox = new QDoubleSpinBox();
    m_alphaSpinBox->setAlignment(Qt::AlignmentFlag::AlignRight);
    m_alphaSpinBox->setMinimum(0);
    m_alphaSpinBox->setMaximum(100);
    m_alphaSpinBox->setSuffix(tr("%"));
    m_alphaSpinBox->setDecimals(0);
    m_alphaSpinBox->setSingleStep(singleStepAlpha * 100);
    // m_alphaSpinBox is of type QDoubleSpinBox which does not allow to
    // configure the pageStep.
    m_alphaLabel = new QLabel(tr("O&pacity:"));
    m_alphaLabel->setBuddy(m_alphaSpinBox);
    m_alphaLayout->addWidget(m_alphaLabel);
    m_alphaLayout->addWidget(m_alphaGradientSlider);
    m_alphaLayout->addWidget(m_alphaSpinBox);

    // Create the default buttons
    m_buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox,
            &QDialogButtonBox::accepted,
            q_pointer,
            &PerceptualColor::ColorDialog::accept);
    connect(m_buttonBox,
            &QDialogButtonBox::rejected,
            q_pointer,
            &PerceptualColor::ColorDialog::reject);

    // Create the main layout
    QVBoxLayout *tempMainLayout = new QVBoxLayout();
    tempMainLayout->addWidget(m_colorPatch);
    tempMainLayout->addLayout(m_selectorLayout);
    tempMainLayout->addLayout(m_alphaLayout);
    tempMainLayout->addWidget(m_buttonBox);
    q_pointer->setLayout(tempMainLayout);

    // initialize signal-slot-connections
    connect(m_rgbSpinBox, &MultiSpinBox::editingFinished, q_pointer, [this]() {
        readRgbNumericValues();
    });
    connect(m_rgbLineEdit, &QLineEdit::editingFinished, q_pointer, [this]() {
        readRgbHexValues();
    });
    connect(m_hsvSpinBox, &MultiSpinBox::editingFinished, q_pointer, [this]() {
        readHsvNumericValues();
    });
    connect(m_hlcSpinBox, &MultiSpinBox::editingFinished, q_pointer, [this]() {
        readHlcNumericValues();
    });
    connect(m_lchLightnessSelector,
            &GradientSlider::valueChanged,
            q_pointer,
            [this]() { readLightnessValue(); });
    connect(m_wheelColorPicker,
            &WheelColorPicker::currentColorChanged,
            q_pointer,
            [this](const PerceptualColor::LchDouble &color) {
                setCurrentOpaqueColor(color);
            });
    connect(m_chromaHueDiagram,
            &ChromaHueDiagram::currentColorChanged,
            q_pointer,
            [this](const PerceptualColor::LchDouble &color) {
                setCurrentOpaqueColor(color);
            });
    connect(m_alphaGradientSlider,
            &GradientSlider::valueChanged,
            q_pointer,
            [this]() { updateColorPatch(); });
    connect(m_alphaGradientSlider,
            &GradientSlider::valueChanged,
            q_pointer,
            [this](const qreal newFraction) {
                const QSignalBlocker blocker(m_alphaSpinBox);
                m_alphaSpinBox->setValue(newFraction * 100);
            });
    connect(m_alphaSpinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            q_pointer,
            [this](const double newValue) {
                m_alphaGradientSlider->setValue(newValue / 100);
            });

    // Initialize the options
    q_pointer->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);

    // Initialize the window title
    q_pointer->setWindowTitle(tr("Select Color"));

    // Enable size grip
    // As this dialog can indeed be resized, the size grip should
    // be enabled. So, users can see the little triangle at the
    // right botton of the dialog (or the left bottom on a
    // right-to-left layout). So, the user will be aware
    // that he can indeed resize this dialog, which is
    // important as the users are used to the default
    // platform dialogs, which often do not allow resizing. Therefore,
    // by default, QDialog::isSizeGripEnabled() should be true.
    // NOTE: Some widget styles like Oxygen or Breeze leave the size grip
    // widget invisible; nevertheless it reacts on mouse events. Other
    // widget styles indeed show the size grip widget, like Fusion or
    // QtCurve.
    q_pointer->setSizeGripEnabled(true);

    // Refresh button for the HLC spin box
    FallbackIconEngine *myIconEngine = new FallbackIconEngine;
    myIconEngine->setReferenceWidget(m_hlcSpinBox);
    // myIcon takes ownership of myIconEngine, therefore we won’t
    // delete myIconEngine manually.
    QIcon myIcon = QIcon(myIconEngine);
    QAction *myAction = new QAction(
        myIcon,          // icon
        QLatin1String(), // text
        // The q_pointer’s object is still not fully initialized at
        // this point, but it’s base class constructor has fully run;
        // this should be enough to use functionality based on QWidget.
        q_pointer // parent object
    );
    m_hlcSpinBox->addActionButton(myAction,
                                  QLineEdit::ActionPosition::TrailingPosition);
    connect(myAction, &QAction::triggered, q_pointer, [this]() {
        readHlcNumericValues();
    });
}

/** @brief Reads the HLC numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readHlcNumericValues()
{
    QList<MultiSpinBox::SectionData> hlcSections = m_hlcSpinBox->sections();
    LchDouble lch;
    lch.h = hlcSections[0].value;
    lch.l = hlcSections[1].value;
    lch.c = hlcSections[2].value;
    setCurrentOpaqueColor(m_rgbColorSpace->nearestInGamutSacrifyingChroma(lch));
}

/** @brief Initialize the numeric input widgets of this dialog.
 * @returns A pointer to a new widget that has the other, numeric input
 * widgets as child widgets. */
QWidget *ColorDialog::ColorDialogPrivate::initializeNumericPage()
{
    // Setup
    constexpr int decimals = 0;
    MultiSpinBox::SectionData mySection;
    mySection.decimals = decimals;

    // Create RGB MultiSpinBox
    m_rgbSpinBox = new MultiSpinBox();
    QList<MultiSpinBox::SectionData> rgbSections;
    mySection.minimum = 0;
    mySection.maximum = 255;
    mySection.suffix = QStringLiteral(u" ");
    rgbSections.append(mySection);
    mySection.prefix = QStringLiteral(u" ");
    rgbSections.append(mySection);
    mySection.suffix = QString();
    rgbSections.append(mySection);
    m_rgbSpinBox->setSections(rgbSections);
    m_rgbSpinBox->setWhatsThis(tr("<p>Red, green, blue: 0–255</p>"));

    // Create widget for the hex style color representation
    m_rgbLineEdit = new QLineEdit();
    m_rgbLineEdit->setMaxLength(7);
    QRegularExpression tempRegularExpression(
        QStringLiteral(u"#?[0-9A-Fa-f]{0,6}"));
    QRegularExpressionValidator *validator =
        new QRegularExpressionValidator(tempRegularExpression, q_pointer);
    m_rgbLineEdit->setValidator(validator);
    m_rgbLineEdit->setWhatsThis(
        tr("<p>Hexadecimal color code, as used in HTML</p>"
           "<p>#RRGGBB</p>"
           "<ul>RR: two-digit code for red</ul>"
           "<ul>GG: two-digit code for green</ul>"
           "<ul>BB: two-digit code for blue</ul>"
           "Range for each color: 00–FF"));

    // Create HSV spin box
    m_hsvSpinBox = new MultiSpinBox();
    QList<MultiSpinBox::SectionData> hsvSections;
    mySection.prefix = QString();
    mySection.minimum = 0;
    mySection.maximum = 360;
    mySection.isWrapping = true;
    mySection.suffix = QStringLiteral(u"° ");
    hsvSections.append(mySection);
    mySection.prefix = QStringLiteral(u" ");
    mySection.maximum = 255;
    mySection.isWrapping = false;
    mySection.suffix = QStringLiteral(u" ");
    hsvSections.append(mySection);
    mySection.suffix = QString();
    hsvSections.append(mySection);
    m_hsvSpinBox->setSections(hsvSections);
    m_hsvSpinBox->setWhatsThis(
        tr("<p>Hue: 0°–360°</p>"
           "<p>Saturation: 0–255</p>"
           "<p>Brightness/Value: 0–255</p>"));

    // Create RGB layout
    QFormLayout *tempRgbFormLayout = new QFormLayout();
    QLabel *tempRgbLabel = new QLabel(tr("&RGB"));
    tempRgbLabel->setBuddy(m_rgbSpinBox);
    tempRgbFormLayout->addRow(tempRgbLabel, m_rgbSpinBox);
    tempRgbFormLayout->addRow(tr("He&x"), m_rgbLineEdit);
    QLabel *tempHsvLabel = new QLabel(tr("HS&V"));
    tempHsvLabel->setBuddy(m_hsvSpinBox);
    tempRgbFormLayout->addRow(tempHsvLabel, m_hsvSpinBox);
    QGroupBox *rgbGroupBox = new QGroupBox();
    rgbGroupBox->setLayout(tempRgbFormLayout);
    // LittleCMS gives access to the following data fields in profiles:
    // description, manufacturer, model, copyright. Each field might also
    // be empty. The most interesting field is “description”. We use it
    // as title for the group box that contains the RGB based input widgets.
    // Also the fields “manufacturer” and “model” might be
    // interesting. If one of those three fields is not empty, we will
    // provide a tool-tip that contains the description, manufacturer and
    // model data fields. Otherwise, no tool-tip is used.
    // The “copyright” field is not really interesting for the user; we do
    // not use it here.
    rgbGroupBox->setTitle(m_rgbColorSpace->profileInfoDescription());
    if (!m_rgbColorSpace->profileInfoDescription().isEmpty() ||
        !m_rgbColorSpace->profileInfoManufacturer().isEmpty() ||
        !m_rgbColorSpace->profileInfoModel().isEmpty()) {
        // The profile infos might theoretically contain HTML tagging,
        // and the whatsThis property might interpretate them as such
        // (it switches on a heuristic between rich text with HTML tags,
        // and normal text). However, this is not likely, it would anyway
        // not be clear which is the correct rendering, and a slightly
        // wrong rendering will not harm…
        QStringList profileInfo;
        profileInfo.append(tr(
            "<b>Information about the currently used RGB color profile</b>"));
        if (!m_rgbColorSpace->profileInfoDescription().isEmpty()) {
            profileInfo.append(
                tr("Description: <i>%1</i>")
                    .arg(m_rgbColorSpace->profileInfoDescription()));
        }
        if (!m_rgbColorSpace->profileInfoManufacturer().isEmpty()) {
            profileInfo.append(
                tr("Manufacturer: <i>%1</i>")
                    .arg(m_rgbColorSpace->profileInfoManufacturer()));
        }
        if (!m_rgbColorSpace->profileInfoModel().isEmpty()) {
            profileInfo.append(tr("Model: <i>%1</i>")
                                   .arg(m_rgbColorSpace->profileInfoModel()));
        }
        // Set help text.
        rgbGroupBox->setWhatsThis(profileInfo.join(QStringLiteral("<br>")));
    }

    // Create widget for the HLC color representation
    QList<MultiSpinBox::SectionData> hlcSections;
    m_hlcSpinBox = new MultiSpinBox;
    mySection.minimum = 0;
    mySection.maximum = 360;
    mySection.prefix = QLatin1String();
    mySection.suffix = QStringLiteral(u"° ");
    mySection.isWrapping = true;
    hlcSections.append(mySection);
    mySection.maximum = 100;
    mySection.prefix = QStringLiteral(u" ");
    mySection.suffix = QStringLiteral(u"% ");
    mySection.isWrapping = false;
    hlcSections.append(mySection);
    mySection.maximum = LchValues::humanMaximumChroma;
    mySection.prefix = QStringLiteral(u" ");
    mySection.suffix = QLatin1String();
    mySection.isWrapping = false;
    hlcSections.append(mySection);
    m_hlcSpinBox->setSections(hlcSections);
    m_hlcSpinBox->setWhatsThis(
        tr("<p>Hue: 0°–360°</p>"
           "<p>Lightness: 0%–100%</p>"
           "<p>Chroma: 0–255</p>"));

    // Create a global widget
    QWidget *tempWidget = new QWidget;
    QVBoxLayout *tempMainLayout = new QVBoxLayout;
    tempWidget->setLayout(tempMainLayout);
    tempWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QFormLayout *cielabFormLayout = new QFormLayout;
    cielabFormLayout->addRow(tr("HL&C"), m_hlcSpinBox);
    tempMainLayout->addLayout(cielabFormLayout);
    tempMainLayout->addWidget(rgbGroupBox);
    tempMainLayout->addStretch();

    // Return
    return tempWidget;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColorDialog::ColorDialogOptions ColorDialog::options() const
{
    return d_pointer->m_options;
}

/** @brief Setter for @ref options.
 *
 * Sets a value for just one single option within @ref options.
 * @param option the option to set
 * @param on the new value of the option
 */
void ColorDialog::setOption(
    PerceptualColor::ColorDialog::ColorDialogOption option,
    bool on)
{
    QColorDialog::ColorDialogOptions temp = d_pointer->m_options;
    temp.setFlag(option, on);
    setOptions(temp);
}

/** @brief Setter for @ref options */
void ColorDialog::setOptions(
    PerceptualColor::ColorDialog::ColorDialogOptions newOptions)
{
    if (newOptions == d_pointer->m_options) {
        return;
    }

    // Save the new options
    d_pointer->m_options = newOptions;
    // Correct QColorDialog::ColorDialogOption::DontUseNativeDialog
    // which must be always on
    d_pointer->m_options.setFlag(
        QColorDialog::ColorDialogOption::DontUseNativeDialog, true);

    // Apply the new options (alpha)
    const bool alphaVisibility = d_pointer->m_options.testFlag(
        QColorDialog::ColorDialogOption::ShowAlphaChannel);
    d_pointer->m_alphaLabel->setVisible(alphaVisibility);
    d_pointer->m_alphaGradientSlider->setVisible(alphaVisibility);
    d_pointer->m_alphaSpinBox->setVisible(alphaVisibility);

    // Apply the new options (buttons)
    d_pointer->m_buttonBox->setVisible(!d_pointer->m_options.testFlag(
        QColorDialog::ColorDialogOption::NoButtons));

    // Notify
    Q_EMIT optionsChanged(d_pointer->m_options);
}

/** @brief Getter for @ref options
 *
 * Gets the value of just one single option within @ref options.
 *
 * @param option the requested option
 * @returns the value of the requested option
 */
bool ColorDialog::testOption(
    PerceptualColor::ColorDialog::ColorDialogOption option) const
{
    return d_pointer->m_options.testFlag(option);
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
QColor ColorDialog::getColor(const QColor &initial,
                             QWidget *parent,
                             const QString &title,
                             QColorDialog::ColorDialogOptions options)
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
 * This function most useful to get the actually selected color <em>after</em>
 * that the dialog has been closed.
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
    return d_pointer->m_selectedColor;
}

/** @brief Setter for property <em>visible</em>
 *
 * Reimplemented from base class.
 *
 * When a dialog, that wasn't formerly visible, gets visible,
 * it's @ref ColorDialogPrivate::m_selectedColor is cleared.
 *
 * @param visible holds whether or not the dialog should be visible */
void ColorDialog::setVisible(bool visible)
{
    if (visible && (!isVisible())) {
        // Only delete the selected color if the dialog wasn’t visible before
        // and will be made visible now.
        d_pointer->m_selectedColor = QColor();
        d_pointer->applyLayoutDimensions();
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
        d_pointer->m_selectedColor = currentColor();
        Q_EMIT colorSelected(d_pointer->m_selectedColor);
    } else {
        d_pointer->m_selectedColor = QColor();
    }
    QDialog::done(result);
    if (d_pointer->m_receiverToBeDisconnected) {
        // This “disconnect” uses the old-style syntax, which does not
        // detect errors on compile time. However, we do not see a
        // possibility how to substitute it with the better new-style
        // syntax, given that d_pointer->m_memberToBeDisconnected
        // can contain different classes, which would be difficult
        // it typing the class name directly in the new syntax.
        disconnect(this,
                   SIGNAL(colorSelected(QColor)),
                   d_pointer->m_receiverToBeDisconnected,
                   d_pointer->m_memberToBeDisconnected.constData());
        d_pointer->m_receiverToBeDisconnected = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
ColorDialog::DialogLayoutDimensions ColorDialog::layoutDimensions() const
{
    return d_pointer->m_layoutDimensions;
}

/** @brief Setter for property @ref layoutDimensions */
void ColorDialog::setLayoutDimensions(
    const ColorDialog::DialogLayoutDimensions newLayoutDimensions)
{
    d_pointer->m_layoutDimensions = newLayoutDimensions;
    d_pointer->applyLayoutDimensions();
    Q_EMIT layoutDimensionsChanged(d_pointer->m_layoutDimensions);
}

/** @brief Arranges the layout conforming to @ref layoutDimensions
 *
 * If @ref layoutDimensions is DialogLayoutDimensions::automatic than it is
 * first evaluated again if for the current display the collapsed or the
 * expanded layout is used. */
void ColorDialog::ColorDialogPrivate::applyLayoutDimensions()
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
    case DialogLayoutDimensions::screenSizeDependent:
        // Note: The following code works correctly on scaled
        // devices (high-DPI…).

        // We should not use more than 70% of the screen for a dialog.
        // That's roughly the same as the default maximum sizes for
        // a QDialog.
        effectivelyAvailableScreenWidth = qRound(
            QGuiApplication::primaryScreen()->availableSize().width() * 0.7);

        // Now we calculate the space we need for displaying the
        // graphical selectors and the numerical selector at their
        // preferred size in an expanded layout.
        // Start with the size of the graphical selectors.
        widthThreeshold = qMax(m_wheelColorPicker->sizeHint().width(),
                               m_lightnessFirstWidget->sizeHint().width());
        // Add the size of the numerical selector.
        widthThreeshold += m_numericalWidget->sizeHint().width();
        // Add some space for margins.
        widthThreeshold = qRound(widthThreeshold * 1.2);

        // Now decide between collapsed layout and expanded layout
        collapsedLayout = (effectivelyAvailableScreenWidth < widthThreeshold);
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
            // Adopt size of dialog to new layout’s size hint:
            q_pointer->adjustSize();
        }
    } else {
        if (m_selectorLayout->indexOf(m_numericalWidget) < 0) {
            // Indeed we have collapsed layout and have to switch to
            // expanded layout…
            m_selectorLayout->addWidget(m_numericalWidget);
            // We call show because the widget is hidden by removing it
            // from its old parent, and needs to be shown explicitly.
            m_numericalWidget->show();
            // Adopt size of dialog to new layout’s size hint:
            q_pointer->adjustSize();
        }
    }
}

}
