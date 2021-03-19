﻿// SPDX-License-Identifier: MIT
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

#include "PerceptualColor/colordialog.h"

#include "PerceptualColor/multispinbox.h"
#include "PerceptualColor/polarpointf.h"
#include "PerceptualColor/labdouble.h"
#include "PerceptualColor/lchdouble.h"
#include "PerceptualColor/rgbdouble.h"
#include "fallbackiconengine.h"

#include <lcms2.h>

#include <QApplication>

#include <QColorDialog>
#include <QAction>
#include <QDebug>
#include <QLineEdit>
#include <QImageReader>
#include <QPainter>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QColorDialog>
#include <memory>// Other includes
#include <QtMath>
#include <QLabel>

// Assert UTF-8 as execution character set as documented
// in perceptualcolorlib_global.h
static_assert(
    (static_cast<quint8>(*(("🖌")+0)) == 0xF0) &&
        (static_cast<quint8>(*(("🖌")+1)) == 0x9F) &&
        (static_cast<quint8>(*(("🖌")+2)) == 0x96) &&
        (static_cast<quint8>(*(("🖌")+3)) == 0x8C) &&
        (static_cast<quint8>(*(("🖌")+4)) == 0x00),
    "Compiler must use UTF8 as narrow execution character set. \n"
        "Example: gcc -fexec-charset=UTF-8"
);

// TODO Test the main function etc from KColorChooser to see if
// PerceptualColor::ColorDialog is really a drop-in replacement
// for QColorDialog.

// TODO Drop this executable in favor of KColorChooser?

int main(int argc, char *argv[])
{
    // Prepare configuratin before instanciating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // Instanciate the application object
    QApplication app(argc, argv);
    app.setApplicationName(
        QObject::tr(u8"Perceptual color picker")
    );
//     app.setLayoutDirection(Qt::RightToLeft);
//     QLocale::setDefault(QLocale::Bengali);
//     QLocale::setDefault(QLocale::German);

    // Initialize the color dialog
    PerceptualColor::ColorDialog m_colorDialog;
    m_colorDialog.setOption(
        QColorDialog::ColorDialogOption::ShowAlphaChannel,
        true
    );
QColor myColor = QColor(Qt::yellow);
myColor.setAlphaF(0.5);
m_colorDialog.setCurrentColor(myColor);
//     m_colorDialog.setOption(QColorDialog::ColorDialogOption::NoButtons);
//     m_colorDialog.setLayoutDimensions(
//         PerceptualColor
//             ::ColorDialog
//             ::DialogLayoutDimensions
//             ::screenSizeDependent
//     );
    m_colorDialog.show();

//     m_colorDialog.setStyleSheet(
//         "background: yellow; color: red; border: 15px solid #FF0000;"
//     );

    // Run
    return app.exec();
}
