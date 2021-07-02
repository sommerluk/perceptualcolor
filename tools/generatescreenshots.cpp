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

#include "PerceptualColor/chromahuediagram.h"
#include "PerceptualColor/colordialog.h"
#include "PerceptualColor/colorpatch.h"
#include "PerceptualColor/colorwheel.h"
#include "PerceptualColor/gradientslider.h"
#include "PerceptualColor/multispinbox.h"
#include "PerceptualColor/rgbcolorspacefactory.h"
#include "PerceptualColor/wheelcolorpicker.h"
#include "chromalightnessdiagram.h"
#include "lchvalues.h"
#include "refreshiconengine.h"
#include "rgbcolorspace.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

using namespace PerceptualColor;

static void screenshot(QWidget *widget, const QString &comment = QLatin1String())
{
    // Get fully qualified class name
    QString className = QString::fromUtf8(widget->metaObject()->className());
    // Strip all the qualifiers
    className = className.split(QStringLiteral("::")).last();
    widget->grab().save(
        // File name:
        className + comment + QStringLiteral(".png"),
        // Ffile format: nullprt means: The file format will be chosen
        // from file name’s suffix.
        nullptr,
        // Compression:
        // 0 means: The compression is slow and results in a small file size.
        // 100 means: The compression is fast and results in a big file size.
        0);
}

// Creates a set of screenshots of the library and saves these
// screenshots as .png files in the working directory.
int main(int argc, char *argv[])
{
    // Prepare configuratin before instanciating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Instanciate the application object
    QApplication app(argc, argv);

    // The actual image size depends on the device pixel ratio of the
    // machine that is running this program. It would be better if
    // the result would be the same at all platforms, but the question
    // is: How?. The following lines are apparently useless:
    // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    // qreal scaleFactor = 1.0;
    // const QString sf = QString::number(scaleFactor, 'f', 2);
    // qputenv("QT_SCALE_FACTOR", sf.toLatin1());

    QStyle *style = nullptr;
    // We prefer the Fusion style because he is the most cross-platform
    // style, to generating the screenshots does not depend on the
    // current system.
    //
    // Possible styles (not all available in all setups):
    // "Breeze", "dsemilight", "dsemidark", "dlight", "ddark", "kvantum-dark",
    // "kvantum", "cleanlooks", "gtk2", "cde", "motif", "plastique", "Oxygen",
    // "QtCurve", "Windows", "Fusion"
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Fusion"));
    }
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Breeze"));
    }
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Oxygen"));
    }
    QApplication::setStyle(style); // This call is save even if style==nulltrp.

    // We use the Windows style’s palette instead of Fusion’s palette
    // because Fusion’s palette depends on the systems settings. But
    // we want something system-independant to make the screenshots
    // look always the same.
    QPalette standardPalette; // Default constructor initializes default palette
    QSharedPointer<QStyle> windowsStyle(QStyleFactory::create(QStringLiteral("Windows")));
    if (windowsStyle != nullptr) {
        standardPalette = windowsStyle->standardPalette();
    }
    QApplication::setPalette(standardPalette);

    // Other initializations
    app.setApplicationName(QStringLiteral("Perceptual color picker"));
    app.setLayoutDirection(Qt::LeftToRight);
    QLocale::setDefault(QLocale::English);

    // Variables
    QSharedPointer<RgbColorSpace> m_colorSpace = //
        RgbColorSpaceFactory::createSrgb();
    const QColor defaultInitialColor = m_colorSpace->toQColorRgbBound(
        // Choose the same initial color as many widgets
        LchValues::srgbVersatileInitialColor());
    QColor myColor;

    ChromaHueDiagram m_chromaHueDiagram(m_colorSpace);
    screenshot(&m_chromaHueDiagram);

    ChromaLightnessDiagram m_chromaLightnessDiagram(m_colorSpace);
    screenshot(&m_chromaLightnessDiagram);

    ColorDialog m_colorDialog;
    m_colorDialog.setLayoutDimensions(ColorDialog::DialogLayoutDimensions::expanded);
    screenshot(&m_colorDialog);
    m_colorDialog.setOption(ColorDialog::ColorDialogOption::ShowAlphaChannel);
    myColor = m_colorDialog.currentColor();
    myColor.setAlphaF(0.5);
    m_colorDialog.setCurrentColor(myColor);
    screenshot(&m_colorDialog, QStringLiteral("Alpha"));
    m_colorDialog.setLayoutDimensions(ColorDialog::DialogLayoutDimensions::expanded);
    screenshot(&m_colorDialog, QStringLiteral("Expanded"));
    m_colorDialog.setLayoutDimensions(ColorDialog::DialogLayoutDimensions::collapsed);
    screenshot(&m_colorDialog, QStringLiteral("Collapsed"));

    ColorPatch m_colorPatch;
    myColor = defaultInitialColor;
    m_colorPatch.setColor(myColor);
    screenshot(&m_colorPatch);
    myColor.setAlphaF(0.5);
    m_colorPatch.setColor(myColor);
    screenshot(&m_colorPatch, QStringLiteral("SemiTransparent"));
    m_colorPatch.setColor(QColor());
    screenshot(&m_colorPatch, QStringLiteral("Invalid"));

    ColorWheel m_colorWheel(m_colorSpace);
    screenshot(&m_colorWheel);

    GradientSlider m_gradientSlider(m_colorSpace);
    m_gradientSlider.setOrientation(Qt::Horizontal);
    screenshot(&m_gradientSlider);

    MultiSpinBox m_multiSpinBox;
    PerceptualColor::MultiSpinBoxSectionConfiguration mySection;
    QList<PerceptualColor::MultiSpinBoxSectionConfiguration> hsvSectionConfigurations;
    QList<double> values;

    mySection.setDecimals(1);
    mySection.setPrefix(QString());
    mySection.setMinimum(0);
    mySection.setWrapping(true);
    mySection.setMaximum(360);
    mySection.setSuffix(QStringLiteral(u"° "));
    hsvSectionConfigurations.append(mySection);
    values.append(310);

    mySection.setPrefix(QStringLiteral(u" "));
    mySection.setMinimum(0);
    mySection.setMaximum(255);
    mySection.setWrapping(false);
    mySection.setSuffix(QStringLiteral(u" "));
    hsvSectionConfigurations.append(mySection);
    values.append(200);

    mySection.setSuffix(QString());
    hsvSectionConfigurations.append(mySection);
    values.append(100);

    m_multiSpinBox.setSectionConfigurations(hsvSectionConfigurations);
    m_multiSpinBox.setSectionValues(values);
    screenshot(&m_multiSpinBox);

    // Refresh button for the HLC spin box
    RefreshIconEngine *myIconEngine = new RefreshIconEngine;
    myIconEngine->setReferenceWidget(&m_multiSpinBox);
    // myIcon takes ownership of myIconEngine, therefore we won’t
    // delete myIconEngine manually.
    QIcon myIcon = QIcon(myIconEngine);
    QAction *myAction = new QAction(myIcon,          // icon
                                    QLatin1String(), // text
                                    &m_multiSpinBox  // parent object
    );
    MultiSpinBox m_multiSpinBoxWithButton;
    m_multiSpinBoxWithButton.setSectionConfigurations(hsvSectionConfigurations);
    m_multiSpinBoxWithButton.setSectionValues(values);
    m_multiSpinBoxWithButton.addActionButton(myAction, QLineEdit::ActionPosition::TrailingPosition);
    screenshot(&m_multiSpinBoxWithButton, QStringLiteral("WithButton"));

    WheelColorPicker m_wheelColorPicker(m_colorSpace);
    screenshot(&m_wheelColorPicker);
}
