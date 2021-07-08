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

#include "PerceptualColor/rgbcolorspacefactory.h"
#include "helper.h"
#include "lchvalues.h"
#include "refreshiconengine.h"
#include "rgbcolorspace.h"

namespace PerceptualColor
{
/** @brief Constructor
 *
 *  @param colorSpace The color space within which this widget should operate.
 *  Can be created with @ref RgbColorSpaceFactory.
 *  @param parent pointer to the parent widget, if any
 *  @post The @ref currentColor property is set to a default value. */
ColorDialog::ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(colorSpace);
    // As m_currentOpaqueColor is invalid be default, and therefore
    // different to our value, setCurrentColorWithAlpha() will actually
    // update all widgets.
    LchDouble lch =
        // Move the color into the currently actually used gamut
        d_pointer->m_rgbColorSpace->nearestInGamutColorByAdjustingChroma(
            // Default color:
            LchValues::srgbVersatileInitialColor());
    d_pointer->setCurrentColorWithAlpha( //
        MultiColor::fromLch(d_pointer->m_rgbColorSpace, lch),
        1);
}

/** @brief Constructor
 *
 *  @param colorSpace The color space within which this widget should operate.
 *  Can be created with @ref RgbColorSpaceFactory.
 *  @param initial the initially chosen color of the dialog
 *  @param parent pointer to the parent widget, if any
 *  @post The object is constructed and @ref setCurrentColor() is called
 *  with <em>initial</em>. See @ref setCurrentColor() for the modifications
 *  that will be applied before setting the current color. Especially, as
 *  this dialog is constructed by default without alpha support, the
 *  alpha channel of <em>initial</em> is ignored and a fully opaque color is
 *  used. */
ColorDialog::ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, const QColor &initial, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(colorSpace);
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
    QColor temp = d_pointer->m_currentOpaqueColor.toRgbQColor();
    temp.setAlphaF(d_pointer->m_alphaGradientSlider->value());
    return temp;
}

/** @brief Setter for @ref currentColor property.
 *
 * @param color the new color
 * @post The property @ref currentColor is adapted as follows:
 * - If <em>color</em> is not valid, <tt>Qt::black</tt> is used instead.
 * - If <em>color</em>’s <tt>QColor::Spec</tt> is <em>not</em>
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
        // Make sure that the QColor::spec() is QColor::Spec::Rgb.
        // QColorDialog apparently calls QColor.rgb() within
        // its setCurrentColor function; this will however round to 8 bit
        // per channel. We prefer a more exact convertion to RGB:
        temp = QColor::fromRgbF( //
            color.redF(),
            color.greenF(),
            color.blueF(),
            color.alphaF());
    } else {
        // For invalid colors same behavior as QColorDialog
        temp = QColor(Qt::black);
    }
    d_pointer->setCurrentColorWithAlpha( //
        MultiColor::fromRgbQColor(d_pointer->m_rgbColorSpace, temp),
        temp.alphaF());
}

/** @brief Sets the @ref currentColor property.
 *
 * @param color The new color to set.
 * @param alpha The new alpha to set. */
void ColorDialog::ColorDialogPrivate::setCurrentColorWithAlpha(const MultiColor &color, double alpha)
{
    if (q_pointer->testOption(ColorDialogOption::ShowAlphaChannel)) {
        m_alphaGradientSlider->setValue(alpha);
    } else {
        m_alphaGradientSlider->setValue(1);
    }
    // No need to update m_alphaSpinBox as this is done
    // automatically by signals emitted by m_alphaGradientSlider.
    setCurrentOpaqueColor(color, nullptr);
}

/** @brief Opens the dialog and connects its @ref colorSelected() signal to
 * the slot specified by receiver and member.
 *
 * The signal will be disconnected from the slot when the dialog is closed.
 *
 * Example:
 * @snippet test/testcolordialog.cpp ColorDialog Open
 *
 * @param receiver the object that will receive the @ref colorSelected() signal
 * @param member the slot that will receive the @ref colorSelected() signal */
void ColorDialog::open(QObject *receiver, const char *member)
{
    connect(this,                          // sender
            SIGNAL(colorSelected(QColor)), // signal
            receiver,                      // receiver
            member);                       // slot
    d_pointer->m_receiverToBeDisconnected = receiver;
    d_pointer->m_memberToBeDisconnected = member;
    QDialog::open();
}

/** @brief Updates the color patch widget
 *
 * @post The color patch widget will show the color
 * of @ref m_currentOpaqueColor and the alpha
 * value of @ref m_alphaGradientSlider. */
void ColorDialog::ColorDialogPrivate::updateColorPatch()
{
    QColor tempRgbQColor = m_currentOpaqueColor.toRgbQColor();
    tempRgbQColor.setAlphaF(m_alphaGradientSlider->value());
    m_colorPatch->setColor(tempRgbQColor);
}

/** @brief Updates @ref m_currentOpaqueColor and affected widgets.
 *
 * @param color the new color
 *
 * @param ignoreWidget A widget that should <em>not</em> be updated. Or
 * <tt>nullptr</tt> to update <em>all</em> widgets.
 *
 * @post If this function is called recursively, nothing happens. Else
 * the color is moved into the gamut, then @ref m_currentOpaqueColor is
 * updated, and the corresponding widgets are updated (except the widget
 * specified to be ignored – if any).
 *
 * @note Recursive functions calls are ignored. This is useful, because you
 * can connect signals from various widgets to this slot without having to
 * worry about infinite recursions. */
void ColorDialog::ColorDialogPrivate::setCurrentOpaqueColor(const PerceptualColor::MultiColor &color, QWidget *const ignoreWidget)
{
    if (m_isColorChangeInProgress || (color == m_currentOpaqueColor)) {
        // Nothing to do!
        return;
    }

    // If we have really some work to do, block recursive calls of this function
    m_isColorChangeInProgress = true;

    // Save currentColor() for later comparison
    // Using currentColor() makes sure correct alpha treatment!
    QColor oldQColor = q_pointer->currentColor();

    // Update m_currentOpaqueColor
    m_currentOpaqueColor = color;

    // Variables
    QColor tempRgbQColor = color.toRgbQColor();
    tempRgbQColor.setAlpha(255);
    QList<double> valueList;

    // Update RGB widget
    if (m_rgbSpinBox != ignoreWidget) {
        valueList.clear();
        valueList.append(tempRgbQColor.redF() * 255);
        valueList.append(tempRgbQColor.greenF() * 255);
        valueList.append(tempRgbQColor.blueF() * 255);
        m_rgbSpinBox->setSectionValues(valueList);
    }

    // Update HSV widget
    if (m_hsvSpinBox != ignoreWidget) {
        valueList.clear();
        valueList.append(tempRgbQColor.hsvHueF() * 360);
        valueList.append(tempRgbQColor.hsvSaturationF() * 255);
        valueList.append(tempRgbQColor.valueF() * 255);
        m_hsvSpinBox->setSectionValues(valueList);
    }

    // Update HLC widget
    if (m_hlcSpinBox != ignoreWidget) {
        m_hlcSpinBox->setSectionValues(m_currentOpaqueColor.toHlc());
    }

    // Update RGB hex widget
    if (m_rgbLineEdit != ignoreWidget) {
        updateRgbHexButBlockSignals();
    }

    // Update lightness selector
    if (m_lchLightnessSelector != ignoreWidget) {
        m_lchLightnessSelector->setValue(color.toLch().l / static_cast<qreal>(100));
    }

    // Update chroma-hue diagram
    if (m_chromaHueDiagram != ignoreWidget) {
        m_chromaHueDiagram->setCurrentColor(color.toLch());
    }

    // Update wheel color picker
    if (m_wheelColorPicker != ignoreWidget) {
        m_wheelColorPicker->setCurrentColor(m_currentOpaqueColor.toLch());
    }

    // Update alpha gradient slider
    if (m_alphaGradientSlider != ignoreWidget) {
        LchaDouble tempColor;
        tempColor.l = m_currentOpaqueColor.toLch().l;
        tempColor.c = m_currentOpaqueColor.toLch().c;
        tempColor.h = m_currentOpaqueColor.toLch().h;
        tempColor.a = 0;
        m_alphaGradientSlider->setFirstColor(tempColor);
        tempColor.a = 1;
        m_alphaGradientSlider->setSecondColor(tempColor);
    }

    // Update widgets that take alpha information
    if (m_colorPatch != ignoreWidget) {
        updateColorPatch();
    }

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
    LchDouble lch = m_currentOpaqueColor.toLch();
    lch.l = m_lchLightnessSelector->value() * 100;
    lch = m_rgbColorSpace->nearestInGamutColorByAdjustingChroma(lch);
    setCurrentOpaqueColor( //
        MultiColor::fromLch(m_rgbColorSpace, lch),
        m_lchLightnessSelector);
}

/** @brief Reads the HSV numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readHsvNumericValues()
{
    const QList<double> hsvValues = m_hsvSpinBox->sectionValues();
    // Let QColor do the conversion from HSV to RGB
    const QColor myQColor = QColor::fromHsvF(hsvValues.at(0) / 360.0, //
                                             hsvValues.at(1) / 255.0, //
                                             hsvValues.at(2) / 255.0  //
                                             )
                                .toRgb();
    setCurrentOpaqueColor(MultiColor::fromRgbQColor(m_rgbColorSpace, myQColor), m_hsvSpinBox);
}

/** @brief Reads the decimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readRgbNumericValues()
{
    const QList<double> rgbValues = m_rgbSpinBox->sectionValues();
    const MultiColor myMulti = MultiColor::fromRgbQColor( //
        m_rgbColorSpace,
        QColor::fromRgbF( // …from the RGB spinbox values.
            rgbValues.at(0) / 255.0,
            rgbValues.at(1) / 255.0,
            rgbValues.at(2) / 255.0));
    setCurrentOpaqueColor(myMulti, m_rgbSpinBox);
}

/** @brief Reads the color of the @ref WheelColorPicker in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readWheelColorPickerValues()
{
    setCurrentOpaqueColor( //
        MultiColor::fromLch(m_rgbColorSpace, m_wheelColorPicker->currentColor()),
        m_wheelColorPicker);
}

/** @brief Reads the color of the @ref ChromaHueDiagram in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readChromaHueDiagramValue()
{
    setCurrentOpaqueColor( //
        MultiColor::fromLch(m_rgbColorSpace, m_chromaHueDiagram->currentColor()),
        m_chromaHueDiagram);
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
        setCurrentOpaqueColor(MultiColor::fromRgbQColor(m_rgbColorSpace, rgb), m_rgbLineEdit);
    } else {
        m_isDirtyRgbLineEdit = true;
    }
}

/** @brief Updates the RGB Hex widget to @ref m_currentOpaqueColor.
 *
 * @post The @ref m_rgbLineEdit gets the value of @ref m_currentOpaqueColor.
 * During this operation, all signals of @ref m_rgbLineEdit are blocked. */
void ColorDialog::ColorDialogPrivate::updateRgbHexButBlockSignals()
{
    QSignalBlocker mySignalBlocker(m_rgbLineEdit);

    // m_currentOpaqueColor is supposed to be always in-gamut. However,
    // because of rounding issues, a conversion to an unbounded RGB
    // color could result in an invalid color. Therefore, we must
    // use a conversion to a _bounded_ RGB color.
    const QColor rgbColor = m_currentOpaqueColor.toRgbQColor();
    // We cannot use QColor.name() directly because this function seems
    // to use floor() instead of round(), which does not make sense in
    // our dialog, and it would be inconsistend with the other widgets
    // of the dialog. Therefore, we have to round explicitly (to integers):
    // This format string provides a non-localized format!
    // Format of the numbers:
    // 1) The number itself
    // 2) The minimal field width (2 digits)
    // 3) The base of the number representation (16, hexadecimal)
    // 4) The fill character (leading zero)
    const QString hexString = QStringLiteral(u"#%1%2%3")
                                  .arg(qRound(rgbColor.redF() * 255), 2, 16, QLatin1Char('0'))
                                  .arg(qRound(rgbColor.greenF() * 255), 2, 16, QLatin1Char('0'))
                                  .arg(qRound(rgbColor.blueF() * 255), 2, 16, QLatin1Char('0'))
                                  .toUpper(); // Convert to upper case
    m_rgbLineEdit->setText(hexString);
}

/** @brief Basic initialization.
 *
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * Code that is shared between the various overloaded constructors. */
void ColorDialog::ColorDialogPrivate::initialize(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
{
    // initialize color space
    m_rgbColorSpace = colorSpace;

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
    m_lchLightnessSelector->setAccessibleName(tr("Lightness"));
    m_chromaHueDiagram = new ChromaHueDiagram(m_rgbColorSpace);
    QHBoxLayout *tempLightnesFirstLayout = new QHBoxLayout();
    tempLightnesFirstLayout->addWidget(m_lchLightnessSelector);
    tempLightnesFirstLayout->addWidget(m_chromaHueDiagram);
    m_lightnessFirstWidget = new QWidget();
    m_lightnessFirstWidget->setLayout(tempLightnesFirstLayout);
    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(tempHueFirstWidget, tr("&Hue-based"));
    m_tabWidget->addTab(m_lightnessFirstWidget, tr("&Lightness-based"));

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
    m_alphaGradientSlider = new GradientSlider(m_rgbColorSpace, Qt::Orientation::Horizontal);
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
    const QString opacityLabel = tr("O&pacity:");
    m_alphaLabel = new QLabel(opacityLabel);
    m_alphaLabel->setBuddy(m_alphaSpinBox);
    m_alphaGradientSlider->setAccessibleName(opacityLabel);
    m_alphaLayout->addWidget(m_alphaLabel);
    m_alphaLayout->addWidget(m_alphaGradientSlider);
    m_alphaLayout->addWidget(m_alphaSpinBox);

    // Create the default buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox,                            // sender
            &QDialogButtonBox::accepted,            // signal
            q_pointer,                              // receiver
            &PerceptualColor::ColorDialog::accept); // slot
    connect(m_buttonBox,                            // sender
            &QDialogButtonBox::rejected,            // signal
            q_pointer,                              // receiver
            &PerceptualColor::ColorDialog::reject); // slot

    // Create the main layout
    QVBoxLayout *tempMainLayout = new QVBoxLayout();
    tempMainLayout->addWidget(m_colorPatch);
    tempMainLayout->addLayout(m_selectorLayout);
    tempMainLayout->addLayout(m_alphaLayout);
    tempMainLayout->addWidget(m_buttonBox);
    q_pointer->setLayout(tempMainLayout);

    // initialize signal-slot-connections
    connect(m_rgbSpinBox,                             // sender
            &MultiSpinBox::sectionValuesChanged,      // signal
            this,                                     // receiver
            &ColorDialogPrivate::readRgbNumericValues // slot
    );
    connect(m_rgbLineEdit,                        // sender
            &QLineEdit::textChanged,              // signal
            this,                                 // receiver
            &ColorDialogPrivate::readRgbHexValues // slot
    );
    connect(m_rgbLineEdit,                                   // sender
            &QLineEdit::editingFinished,                     // signal
            this,                                            // receiver
            &ColorDialogPrivate::updateRgbHexButBlockSignals // slot
    );
    connect(m_hsvSpinBox,                             // sender
            &MultiSpinBox::sectionValuesChanged,      // signal
            this,                                     // receiver
            &ColorDialogPrivate::readHsvNumericValues // slot
    );
    connect(m_hlcSpinBox,                             // sender
            &MultiSpinBox::sectionValuesChanged,      // signal
            this,                                     // receiver
            &ColorDialogPrivate::readHlcNumericValues // slot
    );
    connect(m_hlcSpinBox,                                 // sender
            &MultiSpinBox::editingFinished,               // signal
            this,                                         // receiver
            &ColorDialogPrivate::updateHlcButBlockSignals // slot
    );
    connect(m_lchLightnessSelector,                 // sender
            &GradientSlider::valueChanged,          // signal
            this,                                   // receiver
            &ColorDialogPrivate::readLightnessValue // slot
    );
    connect(m_wheelColorPicker,                             // sender
            &WheelColorPicker::currentColorChanged,         // signal
            this,                                           // receiver
            &ColorDialogPrivate::readWheelColorPickerValues // slot
    );
    connect(m_chromaHueDiagram,                            // sender
            &ChromaHueDiagram::currentColorChanged,        // signal
            this,                                          // receiver
            &ColorDialogPrivate::readChromaHueDiagramValue // slot
    );
    connect(m_alphaGradientSlider,                // sender
            &GradientSlider::valueChanged,        // signal
            this,                                 // receiver
            &ColorDialogPrivate::updateColorPatch // slot
    );
    connect(m_alphaGradientSlider,            // sender
            &GradientSlider::valueChanged,    // signal
            this,                             // receiver
            [this](const qreal newFraction) { // lambda
                const QSignalBlocker blocker(m_alphaSpinBox);
                m_alphaSpinBox->setValue(newFraction * 100);
            });
    connect(m_alphaSpinBox,                                       // sender
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), // signal
            this,                                                 // receiver
            [this](const double newValue) {                       // lambda
                // m_alphaGradientSlider has range [0, 1], while the signal
                // has range [0, 100]. This has to be adapted:
                m_alphaGradientSlider->setValue(newValue / 100);
            });

    // Initialize the options
    q_pointer->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);

    // Initialize the window title.
    // We use QColorDialog::tr() instead of tr(). The string will end
    // up nevertheless in the translation file of this library. We use
    // "Select Color" as string, just as QColorDialog does. When our
    // library is running:
    // 1.) Our own translation has priority
    // 2.) If we do not find an own translation, it will fallback
    //     automatically to Qt translation files, which are likely
    //     to provide a translation for this string.
    // TODO Test this!
    q_pointer->setWindowTitle(QColorDialog::tr("Select Color"));

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
    RefreshIconEngine *myIconEngine = new RefreshIconEngine;
    myIconEngine->setReferenceWidget(m_hlcSpinBox);
    // myIcon takes ownership of myIconEngine, therefore we won’t
    // delete myIconEngine manually.
    QIcon myIcon = QIcon(myIconEngine);
    QAction *myAction = new QAction(
        // Icon:
        myIcon,
        // Text:
        QLatin1String(),
        // The q_pointer’s object is still not fully initialized at this point,
        // but it’s base class constructor has fully run; this should be enough
        // to use functionality based on QWidget. So: Parent object:
        q_pointer);
    m_hlcSpinBox->addActionButton(myAction, QLineEdit::ActionPosition::TrailingPosition);
    connect(myAction,                                     // sender
            &QAction::triggered,                          // signal
            this,                                         // receiver
            &ColorDialogPrivate::updateHlcButBlockSignals // slot
    );
}

/** @brief Updates the HLC spin box to @ref m_currentOpaqueColor.
 *
 * @post The @ref m_hlcSpinBox gets the value of @ref m_currentOpaqueColor.
 * During this operation, all signals of @ref m_hlcSpinBox are blocked. */
void ColorDialog::ColorDialogPrivate::updateHlcButBlockSignals()
{
    QSignalBlocker mySignalBlocker(m_hlcSpinBox);
    m_hlcSpinBox->setSectionValues(m_currentOpaqueColor.toHlc());
}

/** @brief Reads the HLC numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialog::ColorDialogPrivate::readHlcNumericValues()
{
    QList<double> hlcValues = m_hlcSpinBox->sectionValues();
    LchDouble lch;
    lch.h = hlcValues.at(0);
    lch.l = hlcValues.at(1);
    lch.c = hlcValues.at(2);
    setCurrentOpaqueColor(   //
        MultiColor::fromLch( //
            m_rgbColorSpace,
            // TODO Would it be better to adapt all 3 axis instead of only
            // adapting C and L?
            m_rgbColorSpace->nearestInGamutColorByAdjustingChromaLightness(lch)),
        // widget that will ignored during updating:
        m_hlcSpinBox);
}

/** @brief Initialize the numeric input widgets of this dialog.
 * @returns A pointer to a new widget that has the other, numeric input
 * widgets as child widgets. */
QWidget *ColorDialog::ColorDialogPrivate::initializeNumericPage()
{
    // Setup
    constexpr int decimals = 0;
    MultiSpinBoxSectionConfiguration mySection;
    mySection.setDecimals(decimals);

    // Create RGB MultiSpinBox
    m_rgbSpinBox = new MultiSpinBox();
    QList<MultiSpinBoxSectionConfiguration> rgbSections;
    mySection.setMinimum(0);
    mySection.setMaximum(255);
    mySection.setSuffix(QStringLiteral(u" "));
    rgbSections.append(mySection);
    mySection.setPrefix(QStringLiteral(u" "));
    rgbSections.append(mySection);
    mySection.setSuffix(QString());
    rgbSections.append(mySection);
    m_rgbSpinBox->setSectionConfigurations(rgbSections);
    m_rgbSpinBox->setWhatsThis( //
        richTextMarker() + tr("<p>Red, green, blue: 0–255</p>"));

    // Create widget for the hex style color representation
    m_rgbLineEdit = new QLineEdit();
    m_rgbLineEdit->setMaxLength(7);
    QRegularExpression tempRegularExpression(QStringLiteral(u"#?[0-9A-Fa-f]{0,6}"));
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(tempRegularExpression, q_pointer);
    m_rgbLineEdit->setValidator(validator);
    m_rgbLineEdit->setWhatsThis( //
        richTextMarker() +
        tr("<p>Hexadecimal color code, as used in HTML</p>"
           "<p>#RRGGBB</p>"
           "<ul>RR: two-digit code for red</ul>"
           "<ul>GG: two-digit code for green</ul>"
           "<ul>BB: two-digit code for blue</ul>"
           "<p>Range for each color: 00–FF</p>"));

    // Create HSV spin box
    m_hsvSpinBox = new MultiSpinBox();
    QList<MultiSpinBoxSectionConfiguration> hsvSections;
    mySection.setPrefix(QString());
    mySection.setMinimum(0);
    mySection.setMaximum(360);
    mySection.setWrapping(true);
    mySection.setSuffix(QStringLiteral(u"° "));
    hsvSections.append(mySection);
    mySection.setPrefix(QStringLiteral(u" "));
    mySection.setMaximum(255);
    mySection.setWrapping(false);
    mySection.setSuffix(QStringLiteral(u" "));
    hsvSections.append(mySection);
    mySection.setSuffix(QString());
    hsvSections.append(mySection);
    m_hsvSpinBox->setSectionConfigurations(hsvSections);
    m_hsvSpinBox->setWhatsThis(richTextMarker() +
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
    if (!m_rgbColorSpace->profileInfoDescription().isEmpty()     //
        || !m_rgbColorSpace->profileInfoManufacturer().isEmpty() //
        || !m_rgbColorSpace->profileInfoModel().isEmpty()        //
    ) {
        QStringList profileInfo;
        profileInfo.append( //
            richTextMarker() + tr("<b>Currently used RGB color space</b>"));
        const QString description = //
            m_rgbColorSpace->profileInfoDescription().toHtmlEscaped();
        if (!description.isEmpty()) {
            profileInfo.append(tr("Description: <i>%1</i>").arg(description));
        }
        const QString manufacturer = //
            m_rgbColorSpace->profileInfoManufacturer().toHtmlEscaped();
        if (!manufacturer.isEmpty()) {
            profileInfo.append(tr("Manufacturer: <i>%1</i>").arg(manufacturer));
        }
        const QString model = //
            m_rgbColorSpace->profileInfoModel().toHtmlEscaped();
        if (!model.isEmpty()) {
            profileInfo.append(tr("Model: <i>%1</i>").arg(model));
        }
        // Set help text.
        rgbGroupBox->setWhatsThis(profileInfo.join(QStringLiteral("<br/>")));
    }

    // Create widget for the HLC color representation
    QList<MultiSpinBoxSectionConfiguration> hlcSections;
    m_hlcSpinBox = new MultiSpinBox;
    mySection.setMinimum(0);
    mySection.setMaximum(360);
    mySection.setPrefix(QLatin1String());
    mySection.setSuffix(QStringLiteral(u"° "));
    mySection.setWrapping(true);
    hlcSections.append(mySection);
    mySection.setMaximum(100);
    mySection.setPrefix(QStringLiteral(u" "));
    mySection.setSuffix(QStringLiteral(u"% "));
    mySection.setWrapping(false);
    hlcSections.append(mySection);
    mySection.setMaximum(LchValues::humanMaximumChroma);
    mySection.setPrefix(QStringLiteral(u" "));
    mySection.setSuffix(QLatin1String());
    mySection.setWrapping(false);
    hlcSections.append(mySection);
    m_hlcSpinBox->setSectionConfigurations(hlcSections);
    m_hlcSpinBox->setWhatsThis(richTextMarker() +
                               tr("<p>Hue: 0°–360°</p>"
                                  "<p>Lightness: 0%–100%</p>"
                                  "<p>Chroma: 0–%1</p>")
                                   .arg(LchValues::humanMaximumChroma));

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
 * @param on the new value of the option */
void ColorDialog::setOption(PerceptualColor::ColorDialog::ColorDialogOption option, bool on)
{
    QColorDialog::ColorDialogOptions temp = d_pointer->m_options;
    temp.setFlag(option, on);
    setOptions(temp);
}

/** @brief Setter for @ref options
 * @param newOptions the new options
 * @post <em>All</em> options of the widget have the same state
 * (enabled/disabled) as in the given parameter. */
void ColorDialog::setOptions(PerceptualColor::ColorDialog::ColorDialogOptions newOptions)
{
    if (newOptions == d_pointer->m_options) {
        return;
    }

    // Save the new options
    d_pointer->m_options = newOptions;
    // Correct QColorDialog::ColorDialogOption::DontUseNativeDialog
    // which must be always on
    d_pointer->m_options.setFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog, true);

    // Apply the new options (alpha value)
    const bool alphaVisibility = d_pointer->m_options.testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel);
    d_pointer->m_alphaLabel->setVisible(alphaVisibility);
    d_pointer->m_alphaGradientSlider->setVisible(alphaVisibility);
    d_pointer->m_alphaSpinBox->setVisible(alphaVisibility);

    // Apply the new options (buttons)
    d_pointer->m_buttonBox->setVisible(!d_pointer->m_options.testFlag(QColorDialog::ColorDialogOption::NoButtons));

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
bool ColorDialog::testOption(PerceptualColor::ColorDialog::ColorDialogOption option) const
{
    return d_pointer->m_options.testFlag(option);
}

/** @brief Pops up a modal color dialog, lets the user choose a color, and
 *  returns that color.
 *
 * @param colorSpace The color space within which this widget should operate.
 * @param initial    initial value for currentColor()
 * @param parent     parent widget of the dialog (or 0 for no parent)
 * @param title      window title (or an empty string for the default window
 *                   title)
 * @param options    the options() for customizing the look and feel of the
 *                   dialog
 * @returns          selectedColor(): The color the user has selected; or an
 *                   invalid color if the user has canceled the dialog. */
QColor ColorDialog::getColor(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, const QColor &initial, QWidget *parent, const QString &title, QColorDialog::ColorDialogOptions options)
{
    ColorDialog temp(colorSpace, parent);
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
 * it’s @ref selectedColor value is cleared.
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
        disconnect(this,                                           // sender
                   SIGNAL(colorSelected(QColor)),                  // signal
                   d_pointer->m_receiverToBeDisconnected,          // receiver
                   d_pointer->m_memberToBeDisconnected.constData() // slot
        );
        d_pointer->m_receiverToBeDisconnected = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
ColorDialog::DialogLayoutDimensions ColorDialog::layoutDimensions() const
{
    return d_pointer->m_layoutDimensions;
}

/** @brief Setter for property @ref layoutDimensions
 * @param newLayoutDimensions the new layout dimensions */
void ColorDialog::setLayoutDimensions(const ColorDialog::DialogLayoutDimensions newLayoutDimensions)
{
    if (newLayoutDimensions == d_pointer->m_layoutDimensions) {
        return;
    }
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
        // That’s roughly the same as the default maximum sizes for
        // a QDialog.
        effectivelyAvailableScreenWidth = qRound(QGuiApplication::primaryScreen()->availableSize().width() * 0.7);

        // Now we calculate the space we need for displaying the
        // graphical selectors and the numerical selector at their
        // preferred size in an expanded layout.
        // Start with the size of the graphical selectors.
        widthThreeshold = qMax(m_wheelColorPicker->sizeHint().width(), m_lightnessFirstWidget->sizeHint().width());
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

} // namespace PerceptualColor
