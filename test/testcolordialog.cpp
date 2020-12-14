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

#include <QtTest/QtTest>
#include "PerceptualColor/colordialog.h"

namespace PerceptualColor {

class TestColorDialogHelperClass : public QWidget {
Q_OBJECT
void testSnippet05() {
//! [ColorDialog Open]
PerceptualColor::ColorDialog *m_dialog = new PerceptualColor::ColorDialog;
m_dialog->open(this, SLOT(mySlot(QColor)));
//! [ColorDialog Open]
delete m_dialog;
}
};


class TestColorDialog : public QObject
{
    Q_OBJECT

public:
    TestColorDialog(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    QPointer<PerceptualColor::ColorDialog> m_perceptualDialog;
    QPointer<PerceptualColor::ColorDialog> m_perceptualDialog2;
    QPointer<QColorDialog> m_qDialog;
    QPointer<QColorDialog> m_qDialog2;
    QColor m_color;


void testSnippet01() {
//! [ColorDialog Get color with alpha channel]
QColor myColor = PerceptualColor::ColorDialog::getColor(
    Qt::green,          // current color at widget startup
    nullptr,            // parent widget (or nullptr for no parent)
    tr("Window title"), // window title (or an empty string for default title)
    PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel
);
//! [ColorDialog Get color with alpha channel]
Q_UNUSED(myColor);
}

void testSnippet04() {
//! [ColorDialog Get color]
QColor myColor = PerceptualColor::ColorDialog::getColor();
//! [ColorDialog Get color]
Q_UNUSED(myColor);
}

    static void voidMessageHandler(
        QtMsgType,
        const QMessageLogContext &,
        const QString &
    ) {
        // dummy message handler that does not print messages
    }
    
    void helperProvideQColors() {
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);

        QTest::addColumn<QColor>("color");
        QTest::newRow("RGB 1 2 3") << QColor(1, 2, 3);
        QTest::newRow("RGBA 1 2 3 4") << QColor(1, 2, 3, 4);
        QTest::newRow("RGB 1 2 300") << QColor(1, 2, 300);
        QTest::newRow("RGBA 1 2 300 4") << QColor(1, 2, 300, 4);

        QTest::newRow("RGB 0.1 0.2 0.3") << QColor::fromRgbF(0.1, 0.2, 0.3);
        QTest::newRow("RGBA 0.1 0.2 0.3 0.4")
            << QColor::fromRgbF(0.1, 0.2, 0.3, 0.4);
        QTest::newRow("RGB 0.1 6.2 0.300")
            << QColor::fromRgbF(0.1, 6.2, 0.300);
        QTest::newRow("RGBA 0.1 6.2 0.300 0.4")
            << QColor::fromRgbF(0.1, 6.2, 0.300, 0.4);

        QTest::newRow("CMYK 1 2 3 4") << QColor::fromCmyk(1, 2, 3, 4);
        QTest::newRow("CMYK 1 2 3 4 5") << QColor::fromCmyk(1, 2, 3, 4, 5);
        QTest::newRow("CMYK 1 2 300 4") << QColor::fromCmyk(1, 2, 300, 4);
        QTest::newRow("CMYK 1 2 300 4 5")
            << QColor::fromCmyk(1, 2, 300, 4, 5);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4")
            << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4 0.6495217645")
            << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4")
            << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 0.6495217645")
            << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4, 0.6495217645);
        
        QTest::newRow("HSL 2 3 4") << QColor::fromHsl(2, 3, 4);
        QTest::newRow("HSL 2 3 4 5") << QColor::fromHsl(2, 3, 4, 5);
        QTest::newRow("HSL 2 300 4") << QColor::fromHsl(2, 300, 4);
        QTest::newRow("HSL 2 300 4 5") << QColor::fromHsl(2, 300, 4, 5);
        QTest::newRow("HSL 0.2 0.300 0.4")
            << QColor::fromHslF(0.2, 0.300, 0.4);
        QTest::newRow("HSL 0.2 0.300 0.4 0.6495217645")
            << QColor::fromHslF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSL 6.2 0.300 0.4")
            << QColor::fromHslF(6.2, 0.300, 0.4);
        QTest::newRow("HSL 6.2 0.300 0.4 0.6495217645")
            << QColor::fromHslF(6.2, 0.300, 0.4, 0.6495217645);
        
        QTest::newRow("HSV 2 3 4") << QColor::fromHsv(2, 3, 4);
        QTest::newRow("HSV 2 3 4 5") << QColor::fromHsv(2, 3, 4, 5);
        QTest::newRow("HSV 2 300 4") << QColor::fromHsv(2, 300, 4);
        QTest::newRow("HSV 2 300 4 5") << QColor::fromHsv(2, 300, 4, 5);
        QTest::newRow("HSV 0.2 0.300 0.4")  
        << QColor::fromHsvF(0.2, 0.300, 0.4);
        QTest::newRow("HSV 0.2 0.300 0.4 0.6495217645")
            << QColor::fromHsvF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSV 6.2 0.300 0.4")
            << QColor::fromHsvF(6.2, 0.300, 0.4);
        QTest::newRow("HSV 6.2 0.300 0.4 0.6495217645")
            << QColor::fromHsvF(6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("invalid") << QColor();

        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void helperCompareDialog(
        PerceptualColor::ColorDialog *perceptualDialog,
        QColorDialog *qColorDialog
    ) {
        // Compare the state of perceptualDialog (actual)
        // to qColorDialog (expected)
        QCOMPARE(
            perceptualDialog->selectedColor().name(),
            qColorDialog->selectedColor().name()
        );
        QCOMPARE(
            perceptualDialog->selectedColor().alpha(),
            qColorDialog->selectedColor().alpha()
        );
        QCOMPARE(
            perceptualDialog->selectedColor().spec(),
            qColorDialog->selectedColor().spec()
        );
        QCOMPARE(
            perceptualDialog->currentColor().name(),
            qColorDialog->currentColor().name()
        );
        QCOMPARE(
            perceptualDialog->currentColor().alpha(),
            qColorDialog->currentColor().alpha()
        );
        QCOMPARE(
            perceptualDialog->currentColor().spec(),
            qColorDialog->currentColor().spec()
        );
        QCOMPARE(
            perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            qColorDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            )
        );
        QCOMPARE(
            perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            qColorDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            )
        );
        QCOMPARE(
            perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            qColorDialog->options().testFlag(
                QColorDialog::ColorDialogOption::NoButtons
            )
        );
        QCOMPARE(
            perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            qColorDialog->options().testFlag(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            )
        );
        QCOMPARE(
            perceptualDialog->isVisible(),
            qColorDialog->isVisible()
        );
        QCOMPARE(
            perceptualDialog->isModal(),
            qColorDialog->isModal()
        );
        QCOMPARE(
            perceptualDialog->result(),
            qColorDialog->result()
        );
        QCOMPARE(
            perceptualDialog->parent(),
            qColorDialog->parent()
        );
        QCOMPARE(
            perceptualDialog->parentWidget(),
            qColorDialog->parentWidget()
        );
    }

private Q_SLOTS:
    void initTestCase() {
        // Called before the first test function is executed
    }
    void cleanupTestCase() {
        // Called after the last test function was executed
    }

    void init() {
        // Called before each test function is executed
    }

    void cleanup() {
        // Called after every test function
        if (m_perceptualDialog) {
            delete m_perceptualDialog;
        }
        if (m_perceptualDialog2) {
            delete m_perceptualDialog2;
        }
        if (m_qDialog) {
            delete m_qDialog;
        }
        if (m_qDialog2) {
            delete m_qDialog2;
        }
    }

    void testConstructorQWidget() {
        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        QWidget *tempWidget = new QWidget();
        m_perceptualDialog2 = new PerceptualColor::ColorDialog(tempWidget);
        // Test post-condition: currentColor() is Qt::white
        QCOMPARE(m_perceptualDialog->currentColor(), Qt::white);
        QCOMPARE(m_perceptualDialog2->currentColor(), Qt::white);
        QCOMPARE(m_perceptualDialog2->parentWidget(), tempWidget);
        QCOMPARE(m_perceptualDialog2->parent(), tempWidget);
    }
        
    void testConstructorQWidgetConformance() {
        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        QWidget *tempWidget = new QWidget();
        m_perceptualDialog2 = new PerceptualColor::ColorDialog(tempWidget);
        // Test if this coordinates is conform to QColorDialog
        m_qDialog = new QColorDialog();
        m_qDialog2 = new QColorDialog(tempWidget);
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        helperCompareDialog(m_perceptualDialog2, m_qDialog2);
        delete tempWidget;
    }
    
    void testConstructorQColorQWidget_data() {
        helperProvideQColors();
    }

    void testConstructorQColorQWidget() {
        QFETCH(QColor, color);
        QColor colorOpaque;
        if (color.isValid()) {
            colorOpaque = color.toRgb();
            colorOpaque.setAlpha(255);
        } else {
            colorOpaque = Qt::black;
        }

        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog = new PerceptualColor::ColorDialog(color);
        QWidget *tempWidget = new QWidget();
        m_perceptualDialog2 = new PerceptualColor::ColorDialog(
            color,
            tempWidget
        );
        // Test post-condition: currentColor() is color
        QCOMPARE(
            m_perceptualDialog->currentColor().name(),
            colorOpaque.name()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            colorOpaque.alpha()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().spec(),
            colorOpaque.spec()
        );
        QCOMPARE(
            m_perceptualDialog2->currentColor().name(),
            colorOpaque.name()
        );
        QCOMPARE(
            m_perceptualDialog2->currentColor().alpha(),
            colorOpaque.alpha()
        );
        QCOMPARE(
            m_perceptualDialog2->currentColor().spec(),
            colorOpaque.spec()
        );
        QCOMPARE(
            m_perceptualDialog2->parentWidget(),
            tempWidget
        );
        QCOMPARE(
            m_perceptualDialog2->parent(),
            tempWidget
        );
    }
    
    void testConstructorQColorQWidgetConformance_data() {
        helperProvideQColors();
    }

    void testConstructorQColorQWidgetConformance() {
        QFETCH(QColor, color);

        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog = new PerceptualColor::ColorDialog(color);
        QWidget *tempWidget = new QWidget();
        m_perceptualDialog2 = new PerceptualColor::ColorDialog(
            color,
            tempWidget
        );
        // Test if this coordinates is conform to QColorDialog
        m_qDialog = new QColorDialog(color);
        m_qDialog2 = new QColorDialog(color, tempWidget);
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        helperCompareDialog(m_perceptualDialog2, m_qDialog2);
        delete tempWidget;
    }

    void testConformanceWithQColorDialog_data() {
        QTest::addColumn<QColor>("initialColor");
        QTest::addColumn<QColor>("secondColor");
        QTest::addColumn<bool>("showAlphaChannel");
        QTest::addColumn<bool>("noButtons");

        QVector<QPair<QString, QColor>> colorList;

        colorList.append(
            QPair<QString, QColor>(
                QStringLiteral("redOpaque"),
                QColor(255, 0, 0)
            )
        );
        colorList.append(
            QPair<QString, QColor>(
                QStringLiteral("greenHalf"),
                QColor(0, 255, 0, 128)
            )
        );
        colorList.append(
            QPair<QString, QColor>(
                QStringLiteral("greenTransparent"),
                QColor(255, 0, 255, 0)
            )
        );
        colorList.append(
            QPair<QString, QColor>(
                QStringLiteral("invalid"),
                QColor()
            )
        );

        for (int i = 0; i < colorList.size(); ++i) {
            for (int j = 0; j < colorList.size(); ++j) {
                QTest::newRow(
                    (QString(colorList.at(i).first)
                        + QStringLiteral("/")
                        + QString(colorList.at(j).first)
                        + QStringLiteral("/ShowAlphaChannel/NoButtons")
                    ).toLatin1()
                )
                    << colorList.at(i).second
                    << colorList.at(j).second
                    << true
                    << true;
                QTest::newRow(
                    (QString(colorList.at(i).first)
                        + QStringLiteral("/")
                        + QString(colorList.at(j).first)
                        + QStringLiteral("/ShowAlphaChannel")
                    ).toLatin1()
                )
                    << colorList.at(i).second
                    << colorList.at(j).second
                    << true
                    << false;
                QTest::newRow(
                    (QString(colorList.at(i).first)
                        + QStringLiteral("/")
                        + QString(colorList.at(j).first)
                        + QStringLiteral("/NoButtons")
                    ).toLatin1()
                )
                    << colorList.at(i).second
                    << colorList.at(j).second
                    << false
                    << true;
                QTest::newRow(
                    (QString(colorList.at(i).first)
                        + QStringLiteral("/")
                        + QString(colorList.at(j).first)
                        + QLatin1String()
                    ).toLatin1()
                )
                    << colorList.at(i).second
                    << colorList.at(j).second
                    << false
                    << false;
            }
        }
    }
    
    void testConformanceWithQColorDialog() {
        // Some conformance tests (without a particular systematic approach)
        QFETCH(QColor, initialColor);
        QFETCH(QColor, secondColor);
        QFETCH(bool, showAlphaChannel);
        QFETCH(bool, noButtons);

        m_perceptualDialog = new PerceptualColor::ColorDialog(initialColor);
        m_qDialog = new QColorDialog(initialColor);
        helperCompareDialog(m_perceptualDialog, m_qDialog);

        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            showAlphaChannel
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            showAlphaChannel
        );

        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            noButtons
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            noButtons
        );
        helperCompareDialog(m_perceptualDialog, m_qDialog);

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog, m_qDialog);

        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        helperCompareDialog(m_perceptualDialog, m_qDialog);

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog, m_qDialog);

        QTest::keyClick(m_perceptualDialog, Qt::Key_Escape);
        QTest::keyClick(m_qDialog, Qt::Key_Escape);
        helperCompareDialog(m_perceptualDialog, m_qDialog);
    }

    void testColorSelectedSignal() {
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        m_perceptualDialog->show();
        m_qDialog = new QColorDialog();
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog(
            m_perceptualDialog,
            &PerceptualColor::ColorDialog::colorSelected
        );
        QSignalSpy spyQDialog(m_qDialog, &QColorDialog::colorSelected);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog, Qt::Key_Escape);
        QTest::keyClick(m_qDialog, Qt::Key_Escape);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 2);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testPropertyConformance_data() {
        QTest::addColumn<QString>("propertyName");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.propertyCount(); ++i) {
            QTest::newRow(referenceClass.property(i).name())
                << referenceClass.property(i).name();
        }
    }

    void testPropertyConformance() {
        QFETCH(QString, propertyName);
        QMetaObject testClass =
            PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex =
            testClass.indexOfProperty(propertyName.toLatin1());
        int referenceClassIndex =
            referenceClass.indexOfProperty(propertyName.toLatin1());
        QMetaProperty referenceClassProperty =
            referenceClass.property(referenceClassIndex);
        QString message;
        message = QStringLiteral("Test if property \"")
            + referenceClassProperty.name()
            + QStringLiteral("\" of class \"")
            + referenceClass.className()
            + QStringLiteral("\" is also available in \"")
            + testClass.className()
            + QStringLiteral("\".");
        QVERIFY2(testClassIndex >= 0, message.toLatin1());
        QMetaProperty testClassProperty = testClass.property(testClassIndex);
        if (referenceClassProperty.hasNotifySignal()) {
            QVERIFY2(
                testClassProperty.hasNotifySignal(),
                "If the reference class has a notify signal, "
                    "the test class must have also a notify signal."
            );
        }
        QCOMPARE(
            testClassProperty.isConstant(),
            referenceClassProperty.isConstant()
        );
        QCOMPARE(
            testClassProperty.isDesignable(),
            referenceClassProperty.isDesignable()
        );
        QCOMPARE(
            testClassProperty.isEnumType(),
            referenceClassProperty.isEnumType()
        );
        if (referenceClassProperty.isEnumType()) {
            QCOMPARE(
                testClassProperty.enumerator().enumName(),
                referenceClassProperty.enumerator().enumName()
            );
            QCOMPARE(
                testClassProperty.enumerator().isFlag(),
                referenceClassProperty.enumerator().isFlag()
            );
            QCOMPARE(
                testClassProperty.enumerator().isScoped(),
                referenceClassProperty.enumerator().isScoped()
            );
            QCOMPARE(
                testClassProperty.enumerator().isValid(),
                referenceClassProperty.enumerator().isValid()
            );
            QCOMPARE(
                testClassProperty.enumerator().keyCount(),
                referenceClassProperty.enumerator().keyCount()
            );
            QCOMPARE(
                testClassProperty.enumerator().name(),
                referenceClassProperty.enumerator().name()
            );
            QCOMPARE(
                testClassProperty.enumerator().scope(),
                referenceClassProperty.enumerator().scope()
            );
        }
        QCOMPARE(
            testClassProperty.isFinal(),
            referenceClassProperty.isFinal()
        );
        QCOMPARE(
            testClassProperty.isFlagType(),
            referenceClassProperty.isFlagType()
        );
        QCOMPARE(
            testClassProperty.isReadable(),
            referenceClassProperty.isReadable()
        );
        QCOMPARE(
            testClassProperty.isResettable(),
            referenceClassProperty.isResettable()
        );
        QCOMPARE(
            testClassProperty.isScriptable(),
            referenceClassProperty.isScriptable()
        );
        QCOMPARE(
            testClassProperty.isStored(),
            referenceClassProperty.isStored()
        );
        QCOMPARE(
            testClassProperty.isUser(),
            referenceClassProperty.isUser()
        );
        QCOMPARE(
            testClassProperty.isValid(),
            referenceClassProperty.isValid()
        );
        QCOMPARE(
            testClassProperty.isWritable(),
            referenceClassProperty.isWritable()
        );
        QCOMPARE(
            testClassProperty.isWritable(),
            referenceClassProperty.isWritable()
        );
        QCOMPARE(
            testClassProperty.name(),
            referenceClassProperty.name()
        );
        if (referenceClassProperty.hasNotifySignal()) {
            QCOMPARE(
                testClassProperty.notifySignal().methodSignature(),
                referenceClassProperty.notifySignal().methodSignature()
            );
            QCOMPARE(
                testClassProperty.notifySignal().methodType(),
                referenceClassProperty.notifySignal().methodType()
            );
            QCOMPARE(
                testClassProperty.notifySignal().name(),
                referenceClassProperty.notifySignal().name()
            );
            QCOMPARE(
                testClassProperty.notifySignal().parameterCount(),
                referenceClassProperty.notifySignal().parameterCount()
            );
            QCOMPARE(
                testClassProperty.notifySignal().parameterNames(),
                referenceClassProperty.notifySignal().parameterNames()
            );
            QCOMPARE(
                testClassProperty.notifySignal().parameterTypes(),
                referenceClassProperty.notifySignal().parameterTypes()
            );
            QCOMPARE(
                testClassProperty.notifySignal().returnType(),
                referenceClassProperty.notifySignal().returnType()
            );
            QCOMPARE(
                testClassProperty.notifySignal().revision(),
                referenceClassProperty.notifySignal().revision()
            );
            QCOMPARE(
                testClassProperty.notifySignal().tag(),
                referenceClassProperty.notifySignal().tag()
            );
            QCOMPARE(
                testClassProperty.notifySignal().typeName(),
                referenceClassProperty.notifySignal().typeName()
            );
        }
        QCOMPARE(
            testClassProperty.type(),
            referenceClassProperty.type()
        );
        QCOMPARE(
            testClassProperty.typeName(),
            referenceClassProperty.typeName()
        );
        QCOMPARE(
            testClassProperty.userType(),
            referenceClassProperty.userType()
        );
    }

    void testMethodConformance_data() {
        QTest::addColumn<QByteArray>("methodSignature");
        QTest::addColumn<int>("referenceClassIndex");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.methodCount(); ++i) {
            if (referenceClass.method(i).access() != QMetaMethod::Private) {
                // Exclude private methods from conformance check
                QTest::newRow(referenceClass.method(i).name())
                    << QMetaObject::normalizedSignature(
                        referenceClass.method(i).methodSignature()
                    )
                    << i;
            }
        }
    }

    void testMethodConformance() {
        // We do only check QMetaObject::method() and
        // not QMetaObject::constructor because QColorDialog
        // does not provide its constructors to the
        // meta-object system.
        QFETCH(QByteArray, methodSignature);
        QFETCH(int, referenceClassIndex);
        QMetaObject testClass = PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex = testClass.indexOfMethod(methodSignature);
        QMetaMethod referenceClassMethod = referenceClass.method(
            referenceClassIndex
        );
        QString message;
        message = QStringLiteral("Test if method \"")
            + referenceClassMethod.methodSignature()
            + QStringLiteral("\" of class \"")
            + referenceClass.className()
            + QStringLiteral("\" is also available in \"")
            + testClass.className()
            + QStringLiteral("\".");
        QVERIFY2(
            testClassIndex >= 0,
            message.toLatin1()
        );
        QMetaMethod testClassMethod = testClass.method(testClassIndex);
        QCOMPARE(
            testClassMethod.access(),
            referenceClassMethod.access()
        );
        QCOMPARE(
            testClassMethod.isValid(),
            referenceClassMethod.isValid()
        );
        QCOMPARE(
            testClassMethod.methodSignature(),
            referenceClassMethod.methodSignature()
        );
        QCOMPARE(
            testClassMethod.methodType(),
            referenceClassMethod.methodType()
        );
        QCOMPARE(
            testClassMethod.name(),
            referenceClassMethod.name()
        );
        QCOMPARE(
            testClassMethod.parameterCount(),
            referenceClassMethod.parameterCount()
        );
        QCOMPARE(
            testClassMethod.parameterNames(),
            referenceClassMethod.parameterNames()
        );
        QCOMPARE(
            testClassMethod.parameterTypes(),
            referenceClassMethod.parameterTypes()
        );
        QCOMPARE(
            testClassMethod.returnType(),
            referenceClassMethod.returnType()
        );
        QCOMPARE(
            testClassMethod.revision(),
            referenceClassMethod.revision()
        );
        QCOMPARE(
            testClassMethod.tag(),
            referenceClassMethod.tag()
        );
        QCOMPARE(
            testClassMethod.typeName(),
            referenceClassMethod.typeName()
        );
    }

    void testRttiConformance() {
        QMetaObject testClass = PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        QString message;
        message = QStringLiteral("Test that \"")
            + testClass.className()
            + QStringLiteral("\" inherits from \"")
            + referenceClass.className()
            + QStringLiteral("\"'s superclass.");
        QVERIFY2(
            testClass.inherits(referenceClass.superClass()),
            message.toLatin1()
        );
    }
    
    void testCurrentColorChangedSignal() {
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        m_qDialog = new QColorDialog();
        m_perceptualDialog->show();
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog(
            m_perceptualDialog,
            &PerceptualColor::ColorDialog::currentColorChanged
        );
        QSignalSpy spyQDialog(m_qDialog, &QColorDialog::currentColorChanged);
        
        // Test that a simple “return key” click by the user
        // does not call this signal
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 0);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        
        // A different color should call the signal
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3));
        m_qDialog->setCurrentColor(QColor(1, 2, 3));
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        
        // The same color again should not call again the signal
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3));
        m_qDialog->setCurrentColor(QColor(1, 2, 3));
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        
        // The same RGB values, but defined in another color model, should not
        // emit a signal either.
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3).toHsl());
        m_qDialog->setCurrentColor(QColor(1, 2, 3).toHsl());
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        
        // Changing QColorDialog::ColorDialogOption::ShowAlphaChannel should
        // not emit a signal either
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testCurrentColorProperty_data() {
        helperProvideQColors();
    }
    
    void testCurrentColorProperty() {
        QFETCH(QColor, color);
        QColor correctedColor;
        if (color.isValid()) {
            correctedColor = color.toRgb();
        } else {
            correctedColor = Qt::black;
        }
        QColor opaqueColor = correctedColor;
        opaqueColor.setAlpha(255);
        
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;

        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            m_qDialog->currentColor().rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            m_qDialog->currentColor().alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(m_qDialog->currentColor().spec())
        );
        // Test post condition (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            correctedColor.rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            correctedColor.alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(correctedColor.spec())
        );

        // Test that changing QColorDialog::ColorDialogOption::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel, 
            false
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel, 
            false
        );
        // Test conformance (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            m_qDialog->currentColor().rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            m_qDialog->currentColor().alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(m_qDialog->currentColor().spec())
        );
        // Test post condition (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            correctedColor.rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            correctedColor.alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(correctedColor.spec())
        );

        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel, 
            false
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel, 
            false
        );
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            m_qDialog->currentColor().rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            m_qDialog->currentColor().alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(m_qDialog->currentColor().spec())
        );
        // Test post condition (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            opaqueColor.rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            opaqueColor.alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(opaqueColor.spec())
        );
        

        // Test that changing QColorDialog::ColorDialogOption::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        // Test conformance (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            m_qDialog->currentColor().rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            m_qDialog->currentColor().alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(m_qDialog->currentColor().spec())
        );
        // Test post condition (but only integer precision)
        QCOMPARE(
            m_perceptualDialog->currentColor().rgb(),
            opaqueColor.rgb()
        );
        QCOMPARE(
            m_perceptualDialog->currentColor().alpha(),
            opaqueColor.alpha()
        );
        QCOMPARE(
            static_cast<int>(m_perceptualDialog->currentColor().spec()),
            static_cast<int>(opaqueColor.spec())
        );
    }
    
    void helperReceiveSignals(QColor color) {
        m_color = color;
    }

    void testOpen() {
        // Test our reference (QColorDialog)
        m_color = Qt::black;
        m_qDialog = new QColorDialog;
        m_qDialog->setCurrentColor(Qt::white);
        m_qDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_qDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_qDialog->show();
        m_qDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);
        
        // Now test if PerceptualColor::ColorDialog does the same
        // thing as our reference
        m_color = Qt::black;
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_perceptualDialog->setCurrentColor(Qt::white);
        m_perceptualDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_perceptualDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_perceptualDialog->show();
        m_perceptualDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);
    }

    void testDefaultOptions() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            m_qDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            )
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            m_qDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            )
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            m_qDialog->options().testFlag(
                QColorDialog::ColorDialogOption::NoButtons
            )
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            m_qDialog->options().testFlag(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            )
        );
    }
    
    void testOptionDontUseNativeDialogAlwaysTrue() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::DontUseNativeDialog
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        m_perceptualDialog->setOptions(
            QColorDialog::ColorDialogOption::DontUseNativeDialog
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        m_perceptualDialog->setOptions(
            QColorDialog::ColorDialogOption::DontUseNativeDialog
                | QColorDialog::ColorDialogOption::NoButtons
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        m_perceptualDialog->setOptions(
            QColorDialog::ColorDialogOption::DontUseNativeDialog
                | QColorDialog::ColorDialogOption::ShowAlphaChannel
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        m_perceptualDialog->setOptions(
            QColorDialog::ColorDialogOption::DontUseNativeDialog
                | QColorDialog::ColorDialogOption::ShowAlphaChannel
                | QColorDialog::ColorDialogOption::NoButtons
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
        QCOMPARE(
            m_perceptualDialog->options().testFlag(
                QColorDialog::ColorDialogOption::DontUseNativeDialog
            ),
            true
        );
    }
    
    void testOptionShowAlpha() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel
        );
        QVERIFY2(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            "ShowAlphaChannel successfully set."
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel
        );
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        QColor tempColor = QColor(1, 101, 201, 155);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QVERIFY2(
            !m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            "ShowAlphaChannel successfully set."
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        tempColor = QColor(5, 105, 205, 133);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog, m_qDialog);
    }
    
    void testOptionNoButtons() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons
        );
        QVERIFY2(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            "NoButtons successfully set to true."
        );
        m_qDialog->setOption(QColorDialog::ColorDialogOption::NoButtons);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(
            m_perceptualDialog->isVisible(),
            "Should still visible after Return key pressed."
        );
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Escape);
        QTest::keyClick(m_qDialog, Qt::Key_Escape);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(
            !m_perceptualDialog->isVisible(),
            "Should no longer be visible after Escape key pressed."
        );
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        QVERIFY2(
            !m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            "NoButtons successfully set to false."
        );
        m_qDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog, m_qDialog);
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(
            !m_perceptualDialog->isVisible(),
            "Should no longer be visible after Return key pressed."
        );
        helperCompareDialog(m_perceptualDialog, m_qDialog);
    }

    void testSetOptionAndTestOptionInteraction() {
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // Test if the option changes as expected
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // Test if the option changes as expected
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // Test if the option changes as expected
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // Test if the option changes as expected
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            true
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            false
        );
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::NoButtons
            ),
            true
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // define an option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        // change some other option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        // test if first option is still unchanged
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // define an option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        // change some other option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            true
        );
        // test if first option is still unchanged
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // define an option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            true
        );
        // change some other option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        // test if first option is still unchanged
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            true
        );
        delete m_perceptualDialog;
        
        m_perceptualDialog = new PerceptualColor::ColorDialog();
        // define an option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::ShowAlphaChannel,
            false
        );
        // change some other option
        m_perceptualDialog->setOption(
            QColorDialog::ColorDialogOption::NoButtons,
            false
        );
        // test if first option is still unchanged
        QCOMPARE(
            m_perceptualDialog->testOption(
                QColorDialog::ColorDialogOption::ShowAlphaChannel
            ),
            false
        );
        delete m_perceptualDialog;
    }
    
    void testSelectedColorAndSetVisible() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setCurrentColor(QColor(Qt::blue));
        m_qDialog->setCurrentColor(QColor(Qt::blue));
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        // Still no valid selectedColor() because the dialog still wasn't shown
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog, Qt::Key_Escape);
        QTest::keyClick(m_qDialog, Qt::Key_Escape);
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setVisible(true);
        m_qDialog->setVisible(true);
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog, Qt::Key_Return);
        QTest::keyClick(m_qDialog, Qt::Key_Return);
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->hide();
        m_qDialog->hide();
        QCOMPARE(
            m_perceptualDialog->selectedColor(),
            m_qDialog->selectedColor()
        );
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        
    }
    
    void testAliases() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_qDialog = new QColorDialog;
        
        // Test setting QColorDialog syntax
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel);
        QCOMPARE(m_perceptualDialog->testOption(
            QColorDialog::ShowAlphaChannel),
            true
        );
        QCOMPARE(m_perceptualDialog->testOption(
            PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel),
            true
        );
        m_qDialog->setOption(QColorDialog::ShowAlphaChannel);
        QCOMPARE(m_qDialog->testOption(QColorDialog::ShowAlphaChannel), true);
        QCOMPARE(m_qDialog->testOption(
            PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel),
            true
        );
        
        // Test setting our alias syntax
        m_perceptualDialog->setOption(
            PerceptualColor::ColorDialog::ColorDialogOption::NoButtons
        );
        QCOMPARE(m_perceptualDialog->testOption(
            QColorDialog::NoButtons),
            true
        );
        QCOMPARE(m_perceptualDialog->testOption(
            PerceptualColor::ColorDialog::ColorDialogOption::NoButtons),
            true
        );
        m_qDialog->setOption(
            PerceptualColor::ColorDialog::ColorDialogOption::NoButtons
        );
        QCOMPARE(m_qDialog->testOption(QColorDialog::NoButtons), true);
        QCOMPARE(m_qDialog->testOption(
            PerceptualColor::ColorDialog::ColorDialogOption::NoButtons),
            true
        );
        
        // Test if ColorDialogOptions is compatible (at least for == operator)
        // Configure conformance with our dialog:
        m_qDialog->setOption(QColorDialog::DontUseNativeDialog);
        QCOMPARE(m_perceptualDialog->options(), m_qDialog->options());
    }

    void testSizeGrip() {
        // As this dialog can indeed be resized, the size grip should
        // be enabled. So, users can see the little triangle at the
        // right botton of the dialog (or the left bottom on a
        // right-to-left layout). So, the user will be aware
        // that he can indeet resize this dialog, which is
        // important as the users are used to the default
        // platform dialogs, which often do not allow resizing. Therefore,
        // by default, QDialog::isSizeGripEnabled() should be true.
        // NOTE: Some widget styles like Oxygen or Breeze leave the size grip
        // widget invisible; nevertheless it reacts on mouse events. Other
        // widget styles indeed show the size grip widget, like Fusion or
        // QtCurve.
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        QCOMPARE(
            m_perceptualDialog->isSizeGripEnabled(),
            true
        );
        m_perceptualDialog->show();
        QCOMPARE(
            m_perceptualDialog->isSizeGripEnabled(),
            true
        );
        m_perceptualDialog->hide();
        QCOMPARE(
            m_perceptualDialog->isSizeGripEnabled(),
            true
        );
        delete m_perceptualDialog;
    }

    void testLayoutDimensions() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        // Test default value
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::automatic
        );
        
        // Test if values are correctly stored before showing
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed
        );
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded
        );
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::automatic
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::automatic
        );
        
        // Test if values are correctly stored after showing
        m_perceptualDialog->show();
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed
        );
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded
        );
        m_perceptualDialog->setLayoutDimensions(
            PerceptualColor::ColorDialog::DialogLayoutDimensions::automatic
        );
        QCOMPARE(
            m_perceptualDialog->layoutDimensions(),
            PerceptualColor::ColorDialog::DialogLayoutDimensions::automatic
        );
        delete m_perceptualDialog;
    }

    void benchmarkCreateAndShowPerceptualDialog() {
        QBENCHMARK {
            m_perceptualDialog = new PerceptualColor::ColorDialog;
            m_perceptualDialog->show();
            m_perceptualDialog->repaint();
            delete m_perceptualDialog;
        }
    }

    void benchmarkCreateAndShowQColorDialog() {
        QBENCHMARK {
            m_qDialog = new QColorDialog;
            m_qDialog->show();
            m_qDialog->repaint();
            delete m_perceptualDialog;
        }
    }

    void benchmarkChangeColorPerceptual() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_perceptualDialog->show();
        QBENCHMARK {
            m_perceptualDialog->setCurrentColor(Qt::green);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::blue);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::yellow);
            m_perceptualDialog->repaint();
        }
    }

    void benchmarkChangeColorPerceptualSecondTab() {
        m_perceptualDialog = new PerceptualColor::ColorDialog;
        m_perceptualDialog->show();

        QTabWidget *theTabWidget =
            m_perceptualDialog->findChild<QTabWidget*>();
        if (!theTabWidget) {
            throw 0;
        }
        constexpr int myIndex = 1;
        if (theTabWidget->tabText(myIndex) != QStringLiteral("&Lightness first")) {
            throw 0;
        }
        theTabWidget->setCurrentIndex(myIndex);

        QBENCHMARK {
            m_perceptualDialog->setCurrentColor(Qt::green);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::blue);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::yellow);
            m_perceptualDialog->repaint();
        }
    }

    void benchmarkChangeColorQColorDialog() {
        m_qDialog = new QColorDialog;
        m_qDialog->show();
        QBENCHMARK {
            m_qDialog->setCurrentColor(Qt::green);
            m_qDialog->repaint();
            m_qDialog->setCurrentColor(Qt::blue);
            m_qDialog->repaint();
            m_qDialog->setCurrentColor(Qt::yellow);
            m_qDialog->repaint();
        }
    }

void testSnippet02() {
//! [ColorDialog Set options with local enum]
PerceptualColor::ColorDialog *myDialog = new PerceptualColor::ColorDialog();
myDialog->setOption(
    PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel,
    false
);
//! [ColorDialog Set options with local enum]
QCOMPARE(
    myDialog->testOption(
        PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel
    ),
    false
);
delete myDialog;
}

void testSnippet03() {
//! [ColorDialog Set options with QColorDialog enum]
PerceptualColor::ColorDialog *myDialog = new PerceptualColor::ColorDialog();
myDialog->setOption(
    QColorDialog::ShowAlphaChannel,
    false
);
//! [ColorDialog Set options with QColorDialog enum]
QCOMPARE(
    myDialog->testOption(
        PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel
    ),
    false
);
delete myDialog;
}

};

}

QTEST_MAIN(PerceptualColor::TestColorDialog)
#include "testcolordialog.moc" // necessary because we do not use a header file
