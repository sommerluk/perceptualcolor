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
#include "PerceptualColor/wheelcolorpicker.h"
#include "chromalightnessdiagram.h"
#include "fallbackiconengine.h"
#include "lchvalues.h"
#include "rgbcolorspace.h" // TODO We should include the factory method instead!

#include <QApplication>
#include <QStyleFactory>

// TODO Enable codechecks and clang-format and scripts for tools/*.cpp ?

using namespace PerceptualColor;

static void screenshot(QWidget *widget, const QString &comment = QLatin1String())
{
    // Get fully qualified class name
    QString className = QString::fromUtf8(widget->metaObject()->className());
    // Strip all the qualifiers
    className = className.split(QStringLiteral("::")).last();
    widget->grab().save(
        // file name:
        className + comment + QStringLiteral(".png"),
        // file format:
        nullptr, // Means: file format will be chosen from file name’s suffix.
        // compression: 0 means good, slow compression and small file size
        // 100 means: bad, fast compression and big file size
        0);
}

int main(int argc, char *argv[])
{
    // TODO Do not speak about logical pixel (this is only for fonts)!
    // TODO Own page for coding style in Doxygen (instead of MarkDown)

    // Prepare configuratin before instanciating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // Instanciate the application object
    QApplication app(argc, argv);
    // The actual image size depends on the device pixel ratio of the
    // machine that is running this program. It would be better if
    // the result would be the same at all platforms, but the question
    // is: How?. The following lines are apparently useless:
    //
    // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    // qreal scaleFactor = 1.0;
    // const QString sf = QString::number(scaleFactor, 'f', 2);
    // qputenv("QT_SCALE_FACTOR", sf.toLatin1());
    // Possible styles (not all available in all setups):
    // "Breeze", "dsemilight", "dsemidark", "dlight", "ddark", "kvantum-dark",
    // "kvantum", "cleanlooks", "gtk2", "cde", "motif", "plastique", "Oxygen",
    // "QtCurve", "Windows", "Fusion"
    QStyle *style = nullptr;
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Breeze"));
    }
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Oxygen"));
    }
    if (style == nullptr) {
        style = QStyleFactory::create(QStringLiteral("Fusion"));
    }
    QApplication::setStyle(style); // This call is save even if style==nulltrp.
    app.setApplicationName(QObject::tr(u8"Perceptual color picker"));
    app.setLayoutDirection(Qt::LeftToRight);
    QLocale::setDefault(QLocale::English);

    // Variables
    QSharedPointer<RgbColorSpace> m_colorSpace {new RgbColorSpace};
    const QColor defaultInitialColor = m_colorSpace->colorRgbBound(
        // Choose the same initial color as many widgets
        LchValues::srgbVersatileInitialColor);
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
    PerceptualColor::MultiSpinBox::SectionData mySection;
    QList<PerceptualColor::MultiSpinBox::SectionData> hsvSections;
    mySection.decimals = 1;
    mySection.prefix = QString();
    mySection.minimum = 0;
    mySection.value = 310;
    mySection.isWrapping = true;
    mySection.maximum = 360;
    mySection.suffix = QStringLiteral(u"° ");
    hsvSections.append(mySection);
    mySection.prefix = QStringLiteral(u" ");
    mySection.minimum = 0;
    mySection.value = 200;
    mySection.maximum = 255;
    mySection.isWrapping = false;
    mySection.suffix = QStringLiteral(u" ");
    hsvSections.append(mySection);
    mySection.value = 100;
    mySection.suffix = QString();
    hsvSections.append(mySection);
    m_multiSpinBox.setSections(hsvSections);
    screenshot(&m_multiSpinBox);
    // Refresh button for the HLC spin box
    FallbackIconEngine *myIconEngine = new FallbackIconEngine;
    myIconEngine->setReferenceWidget(&m_multiSpinBox);
    // myIcon takes ownership of myIconEngine, therefore we won’t
    // delete myIconEngine manually.
    QIcon myIcon = QIcon(myIconEngine);
    QAction *myAction = new QAction(myIcon,          // icon
                                    QLatin1String(), // text
                                    &m_multiSpinBox  // parent object
    );
    MultiSpinBox m_multiSpinBoxWithButton;
    m_multiSpinBoxWithButton.setSections(hsvSections);
    m_multiSpinBoxWithButton.addActionButton(myAction, QLineEdit::ActionPosition::TrailingPosition);
    screenshot(&m_multiSpinBoxWithButton, QStringLiteral("WithButton"));

    WheelColorPicker m_wheelColorPicker(m_colorSpace);
    screenshot(&m_wheelColorPicker);
}
