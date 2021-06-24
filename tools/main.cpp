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
#include "PerceptualColor/lchdouble.h"
#include "PerceptualColor/multispinbox.h"
#include "PerceptualColor/rgbcolorspacefactory.h"
#include "constpropagatingrawpointer.h"
#include "polarpointf.h"
#include "refreshiconengine.h"
#include "rgbcolorspace.h"
#include "version.h"

#include <lcms2.h>
#include <math.h>
#include <memory>

#include <QAccessible>
#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QImageReader>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QStyleFactory>
#include <QVBoxLayout>
#include <QtMath>

// TODO Test the main function etc from KColorChooser to see if
// PerceptualColor::ColorDialog is really a drop-in replacement
// for QColorDialog.
// TODO Drop this executable in favor of KColorChooser?

// This is just a program for testing purpuses.
int main(int argc, char *argv[])
{
    // Prepare configuratin before instanciating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // Instanciate the application object
    QApplication app(argc, argv);
    app.setApplicationName(QObject::tr("Perceptual color picker"));
    // app.setLayoutDirection(Qt::RightToLeft);
    // QLocale::setDefault(QLocale::Bengali);
    // QLocale::setDefault(QLocale::German);

    // Initialize the color dialog
    PerceptualColor::ColorDialog m_colorDialog;
    // m_colorDialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
    QColor myColor = QColor(Qt::yellow);
    myColor.setAlphaF(0.5);
    m_colorDialog.setCurrentColor(myColor);
    // m_colorDialog.setOption(QColorDialog::ColorDialogOption::NoButtons);
    m_colorDialog.setLayoutDimensions(PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded);
    // m_colorDialog.setEnabled(false);
    // m_colorDialog.setStyleSheet("background: yellow; color: red; border: 15px solid #FF0000;");
    m_colorDialog.show();

    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("kvantum")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("gtk2")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("QtCurve")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("cde")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Cleanlooks")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("motif")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Plastique")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("dlight")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("ddark")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Oxygen")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Breeze")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Windows")));

    QDoubleSpinBox *mySpinBox = new QDoubleSpinBox;
    mySpinBox->setPrefix(QStringLiteral("Prefix"));
    mySpinBox->setSuffix(QStringLiteral("Suffix"));
    mySpinBox->setDecimals(3);
    qDebug() << mySpinBox->maximum();
    mySpinBox->setMinimum(7.125);
    mySpinBox->setMaximum(8.125);
    mySpinBox->setValue(7.126);
    qDebug() << mySpinBox->minimum() << mySpinBox->value() << mySpinBox->maximum();
    mySpinBox->setDecimals(2);
    qDebug() << mySpinBox->minimum() << mySpinBox->value() << mySpinBox->maximum();
    mySpinBox->setDecimals(3);
    qDebug() << mySpinBox->minimum() << mySpinBox->value() << mySpinBox->maximum();

    // PerceptualColor::MultiSpinBox *mySpinBox = new PerceptualColor::MultiSpinBox;
    // PerceptualColor::MultiSpinBoxSectionConfiguration myConfig;
    // myConfig.prefix = QStringLiteral("Prefix");
    // myConfig.suffix = QStringLiteral("Suffix");
    // myConfig.maximum = 9;
    // QList<PerceptualColor::MultiSpinBoxSectionConfiguration> myConfigs;
    // myConfigs.append(myConfig);
    // myConfigs.append(myConfig);
    // mySpinBox->setSectionConfigurations(myConfigs);

    // QDateTimeEdit *mySpinBox = new QDateTimeEdit;
    // mySpinBox->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));

    QWidget myWidget;
    QPushButton *myButton = new QPushButton;
    QHBoxLayout *myLayout = new QHBoxLayout;
    myLayout->addWidget(mySpinBox);
    myLayout->addWidget(myButton);
    myWidget.setLayout(myLayout);
    mySpinBox->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    // mySpinBox->setAlignment(Qt::AlignRight);
    // mySpinBox->setFrame(false);
    // mySpinBox->setReadOnly(true);
    // mySpinBox->setSpecialValueText(QStringLiteral("test"));
    // mySpinBox->setWrapping(true);
    mySpinBox->setAccelerated(true);
    // qDebug() << mySpinBox->isGroupSeparatorShown();
    mySpinBox->setGroupSeparatorShown(true);
    mySpinBox->setKeyboardTracking(true);
    // mySpinBox->setCorrectionMode(QAbstractSpinBox::CorrectionMode::CorrectToNearestValue);
    mySpinBox->setKeyboardTracking(false);
    qDebug() << mySpinBox->correctionMode();
    // QString myString = QString::fromUtf8("abcd");
    // int myInt = 2;
    // mySpinBox->validate(myString, myInt);
    mySpinBox->interpretText();
    // mySpinBox->fixup(myString);

    // myWidget.show();

    QAbstractButton::connect(                                 //
        myButton,                                             //
        &QAbstractButton::clicked,                            //
        mySpinBox,                                            //
        &QAbstractSpinBox::selectAll);                        //
    QAbstractSpinBox::connect(                                //
        mySpinBox,                                            //
        QOverload<double>::of(&QDoubleSpinBox::valueChanged), //
        mySpinBox,                                            //
        [mySpinBox]() { qDebug() << mySpinBox->value(); });   //

    // Run
    return app.exec();
}
