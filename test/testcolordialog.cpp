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

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/colordialog.h"
// Second, the private implementation.
#include "colordialog_p.h"

#include <QPointer>
#include <QScopedPointer>
#include <QtTest>

#include "PerceptualColor/multispinbox.h"
#include "rgbcolorspace.h"

class TestColorDialogSnippetClass : public QWidget
{
    Q_OBJECT
public Q_SLOTS:
    void mySlot(QColor color)
    {
        Q_UNUSED(color)
    }

public:
    // A constructor that is clazy-conform
    TestColorDialogSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }

    void testSnippet05()
    {
        //! [ColorDialog Open]
        PerceptualColor::ColorDialog *m_dialog = new PerceptualColor::ColorDialog;
        m_dialog->open(this, SLOT(mySlot(QColor)));
        //! [ColorDialog Open]
        delete m_dialog;
    }
};

static void snippet01()
{
    // This function will not be called in the unit tests because getColor()
    // does not return without user interaction!
    //! [ColorDialog Get color with alpha channel]
    QColor myColor = PerceptualColor::ColorDialog::getColor(
        // Current color at widget startup:
        Qt::green,
        // Parent widget (or nullptr for no parent):
        nullptr,
        // Window title (or an empty string for default title):
        QStringLiteral("Window title"),
        // Options:
        PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel);
    //! [ColorDialog Get color with alpha channel]
    Q_UNUSED(myColor);
}

static void snippet02()
{
    //! [setOptionsWithLocalEnum]
    PerceptualColor::ColorDialog *myDialog = new PerceptualColor::ColorDialog();
    myDialog->setOption(PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel, false);
    //! [setOptionsWithLocalEnum]
    QCOMPARE(myDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), false);
    delete myDialog;
}

static void snippet03()
{
    //! [setOptionsWithQColorDialogEnum]
    PerceptualColor::ColorDialog *myDialog = new PerceptualColor::ColorDialog();
    myDialog->setOption(QColorDialog::ShowAlphaChannel, false);
    //! [setOptionsWithQColorDialogEnum]
    QCOMPARE(myDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), false);
    delete myDialog;
}

static void snippet04()
{
    // This function will not be called in the unit tests because getColor()
    // does not return without user interaction!
    //! [ColorDialog Get color]
    // Show a modal color dialog and get the color that the user has chosen
    QColor myColor = PerceptualColor::ColorDialog::getColor();
    //! [ColorDialog Get color]
    Q_UNUSED(myColor);
}

namespace PerceptualColor
{
class TestColorDialog : public QObject
{
    Q_OBJECT

public:
    TestColorDialog(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QScopedPointer<PerceptualColor::ColorDialog> m_perceptualDialog;
    QScopedPointer<PerceptualColor::ColorDialog> m_perceptualDialog2;
    QScopedPointer<QColorDialog> m_qDialog;
    QScopedPointer<QColorDialog> m_qDialog2;
    QColor m_color;
    QSharedPointer<RgbColorSpace> m_srgbBuildinColorSpace = RgbColorSpaceFactory::createSrgb();

    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

    void helperProvideQColors()
    {
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);

        QTest::addColumn<QColor>("color");
        QTest::newRow("RGB 1 2 3") << QColor(1, 2, 3);
        QTest::newRow("RGBA 1 2 3 4") << QColor(1, 2, 3, 4);
        QTest::newRow("RGB 1 2 300") << QColor(1, 2, 300);
        QTest::newRow("RGBA 1 2 300 4") << QColor(1, 2, 300, 4);

        QTest::newRow("RGB 0.1 0.2 0.3") << QColor::fromRgbF(0.1, 0.2, 0.3);
        QTest::newRow("RGBA 0.1 0.2 0.3 0.4") << QColor::fromRgbF(0.1, 0.2, 0.3, 0.4);
        QTest::newRow("RGB 0.1 6.2 0.300") << QColor::fromRgbF(0.1, 6.2, 0.300);
        QTest::newRow("RGBA 0.1 6.2 0.300 0.4") << QColor::fromRgbF(0.1, 6.2, 0.300, 0.4);

        QTest::newRow("CMYK 1 2 3 4") << QColor::fromCmyk(1, 2, 3, 4);
        QTest::newRow("CMYK 1 2 3 4 5") << QColor::fromCmyk(1, 2, 3, 4, 5);
        QTest::newRow("CMYK 1 2 300 4") << QColor::fromCmyk(1, 2, 300, 4);
        QTest::newRow("CMYK 1 2 300 4 5") << QColor::fromCmyk(1, 2, 300, 4, 5);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4") << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4 0.6495217645") << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4") << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 0.6495217645") << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("HSL 2 3 4") << QColor::fromHsl(2, 3, 4);
        QTest::newRow("HSL 2 3 4 5") << QColor::fromHsl(2, 3, 4, 5);
        QTest::newRow("HSL 2 300 4") << QColor::fromHsl(2, 300, 4);
        QTest::newRow("HSL 2 300 4 5") << QColor::fromHsl(2, 300, 4, 5);
        QTest::newRow("HSL 0.2 0.300 0.4") << QColor::fromHslF(0.2, 0.300, 0.4);
        QTest::newRow("HSL 0.2 0.300 0.4 0.6495217645") << QColor::fromHslF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSL 6.2 0.300 0.4") << QColor::fromHslF(6.2, 0.300, 0.4);
        QTest::newRow("HSL 6.2 0.300 0.4 0.6495217645") << QColor::fromHslF(6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("HSV 2 3 4") << QColor::fromHsv(2, 3, 4);
        QTest::newRow("HSV 2 3 4 5") << QColor::fromHsv(2, 3, 4, 5);
        QTest::newRow("HSV 2 300 4") << QColor::fromHsv(2, 300, 4);
        QTest::newRow("HSV 2 300 4 5") << QColor::fromHsv(2, 300, 4, 5);
        QTest::newRow("HSV 0.2 0.300 0.4") << QColor::fromHsvF(0.2, 0.300, 0.4);
        QTest::newRow("HSV 0.2 0.300 0.4 0.6495217645") << QColor::fromHsvF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSV 6.2 0.300 0.4") << QColor::fromHsvF(6.2, 0.300, 0.4);
        QTest::newRow("HSV 6.2 0.300 0.4 0.6495217645") << QColor::fromHsvF(6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("invalid") << QColor();

        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void helperCompareDialog(PerceptualColor::ColorDialog *perceptualDialog, QColorDialog *qColorDialog)
    {
        // Compare the state of perceptualDialog (actual)
        // to qColorDialog (expected)
        QCOMPARE(perceptualDialog->selectedColor().name(), qColorDialog->selectedColor().name());
        QCOMPARE(perceptualDialog->selectedColor().alpha(), qColorDialog->selectedColor().alpha());
        QCOMPARE(perceptualDialog->selectedColor().spec(), qColorDialog->selectedColor().spec());
        QCOMPARE(perceptualDialog->currentColor().name(), qColorDialog->currentColor().name());
        QCOMPARE(perceptualDialog->currentColor().alpha(), qColorDialog->currentColor().alpha());
        QCOMPARE(perceptualDialog->currentColor().spec(), qColorDialog->currentColor().spec());
        QCOMPARE(perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), qColorDialog->testOption(QColorDialog::ColorDialogOption::NoButtons));
        QCOMPARE(perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), qColorDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel));
        QCOMPARE(perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::NoButtons), qColorDialog->options().testFlag(QColorDialog::ColorDialogOption::NoButtons));
        QCOMPARE(perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel), qColorDialog->options().testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel));
        QCOMPARE(perceptualDialog->isVisible(), qColorDialog->isVisible());
        QCOMPARE(perceptualDialog->isModal(), qColorDialog->isModal());
        QCOMPARE(perceptualDialog->result(), qColorDialog->result());
        QCOMPARE(perceptualDialog->parent(), qColorDialog->parent());
        QCOMPARE(perceptualDialog->parentWidget(), qColorDialog->parentWidget());
    }

private Q_SLOTS:
    // This is just a helper slot, not an actual test.
    // It is neither called by QTest (because it has a parameter).
    void helperReceiveSignals(QColor color)
    {
        m_color = color;
    }

    void initTestCase()
    {
        // Called before the first test function is executed
    }
    void cleanupTestCase()
    {
        // Called after the last test function was executed
    }

    void init()
    {
        // Called before each test function is executed
    }

    void cleanup()
    {
        // Called after every test function
    }

    void testDefaultConstructorAndDestructor()
    {
        // This should not crash!
        PerceptualColor::ColorDialog test;
    }

    void testConstructorQWidget()
    {
        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        QScopedPointer<QWidget> tempWidget {new QWidget()};
        QScopedPointer<PerceptualColor::ColorDialog> tempPerceptualDialog2 {new PerceptualColor::ColorDialog(tempWidget.data())};
        QCOMPARE(tempPerceptualDialog2->parentWidget(), tempWidget.data());
        QCOMPARE(tempPerceptualDialog2->parent(), tempWidget.data());
    }

    void testConstructorQWidgetConformance()
    {
        // Test the constructor
        m_perceptualDialog.reset(             //
            new PerceptualColor::ColorDialog( //
                m_srgbBuildinColorSpace,      //
                QColor(Qt::white)             //
                )                             //
        );
        QScopedPointer<QWidget> tempWidget {new QWidget()};
        PerceptualColor::ColorDialog *tempPerceptualDialog2 = //
            new PerceptualColor::ColorDialog(                 //
                m_srgbBuildinColorSpace,
                QColor(Qt::white), //
                tempWidget.data()  //
            );
        // Test if this coordinates is conform to QColorDialog
        m_qDialog.reset(new QColorDialog());
        QColorDialog *tempQDialog2 = new QColorDialog(tempWidget.data());
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        helperCompareDialog(tempPerceptualDialog2, tempQDialog2);
    }

    void testConstructorQColorQWidget_data()
    {
        helperProvideQColors();
    }

    void testConstructorQColorQWidget()
    {
        QFETCH(QColor, color);
        QColor colorOpaque;
        if (color.isValid()) {
            colorOpaque = color.toRgb();
            colorOpaque.setAlpha(255);
        } else {
            colorOpaque = Qt::black;
        }

        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog.reset(new ColorDialog(m_srgbBuildinColorSpace, color));
        QScopedPointer<QWidget> tempWidget {new QWidget()};
        QScopedPointer<ColorDialog> tempPerceptualDialog2 {
            //
            new ColorDialog(             //
                m_srgbBuildinColorSpace, //
                color,                   //
                tempWidget.data()        //
                )                        //
        };
        // Test post-condition: currentColor() is color
        QCOMPARE(m_perceptualDialog->currentColor().name(), colorOpaque.name());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), colorOpaque.alpha());
        QCOMPARE(m_perceptualDialog->currentColor().spec(), colorOpaque.spec());
        QCOMPARE(tempPerceptualDialog2->currentColor().name(), colorOpaque.name());
        QCOMPARE(tempPerceptualDialog2->currentColor().alpha(), colorOpaque.alpha());
        QCOMPARE(tempPerceptualDialog2->currentColor().spec(), colorOpaque.spec());
        QCOMPARE(tempPerceptualDialog2->parentWidget(), tempWidget.data());
        QCOMPARE(tempPerceptualDialog2->parent(), tempWidget.data());
    }

    void testConstructorQColorQWidgetConformance_data()
    {
        helperProvideQColors();
    }

    void testConstructorQColorQWidgetConformance()
    {
        QFETCH(QColor, color);

        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog.reset(new ColorDialog(m_srgbBuildinColorSpace, color));
        QScopedPointer<QWidget> tempWidget {new QWidget()};
        ColorDialog *tempPerceptualDialog2 = new ColorDialog( //
            m_srgbBuildinColorSpace,
            color,
            tempWidget.data());
        // Test if this coordinates is conform to QColorDialog
        m_qDialog.reset(new QColorDialog(color));
        QColorDialog *tempQDialog2 = new QColorDialog(color, tempWidget.data());
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        helperCompareDialog(tempPerceptualDialog2, tempQDialog2);
    }

    void testConformanceWithQColorDialog_data()
    {
        QTest::addColumn<QColor>("initialColor");
        QTest::addColumn<QColor>("secondColor");
        QTest::addColumn<bool>("showAlphaChannel");
        QTest::addColumn<bool>("noButtons");

        QVector<QPair<QByteArray, QColor>> colorList;

        colorList.append(QPair<QByteArray, QColor>(QByteArrayLiteral("redOpaque"), QColor(255, 0, 0)));
        colorList.append(QPair<QByteArray, QColor>(QByteArrayLiteral("greenHalf"), QColor(0, 255, 0, 128)));
        colorList.append(QPair<QByteArray, QColor>(QByteArrayLiteral("greenTransparent"), QColor(255, 0, 255, 0)));
        colorList.append(QPair<QByteArray, QColor>(QByteArrayLiteral("invalid"), QColor()));

        for (int i = 0; i < colorList.size(); ++i) {
            for (int j = 0; j < colorList.size(); ++j) {
                QByteArray description = colorList.at(i).first + QByteArrayLiteral("/") + colorList.at(j).first;
                QTest::newRow(QByteArray(description + QByteArrayLiteral("/ShowAlphaChannel/NoButtons")).constData()) << colorList.at(i).second << colorList.at(j).second << true << true;
                QTest::newRow(QByteArray(description + QByteArrayLiteral("/ShowAlphaChannel")).constData()) << colorList.at(i).second << colorList.at(j).second << true << false;
                QTest::newRow(QByteArray(description + QByteArrayLiteral("/NoButtons")).constData()) << colorList.at(i).second << colorList.at(j).second << false << true;
                QTest::newRow(description.constData()) << colorList.at(i).second << colorList.at(j).second << false << false;
            }
        }
    }

    void testConformanceWithQColorDialog()
    {
        // Some conformance tests (without a particular systematic approach)
        QFETCH(QColor, initialColor);
        QFETCH(QColor, secondColor);
        QFETCH(bool, showAlphaChannel);
        QFETCH(bool, noButtons);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, initialColor));
        m_qDialog.reset(new QColorDialog(initialColor));
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, showAlphaChannel);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, showAlphaChannel);

        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, noButtons);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, noButtons);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testColorSelectedSignal()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        m_perceptualDialog->show();
        m_qDialog.reset(new QColorDialog());
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog(m_perceptualDialog.data(), &PerceptualColor::ColorDialog::colorSelected);
        QSignalSpy spyQDialog(m_qDialog.data(), &QColorDialog::colorSelected);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 2);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testPropertyConformance_data()
    {
        // We provide the property names as data. To get the property names,
        // this function is used:
        // const char *QMetaProperty::name() const
        // Now, the type “const char *” cannot be used with QTest::addColumn<>
        // because it is not known to Qt’s meta object system. The
        // meta object system requieres copy constructors for its known types.
        // This might get wired with pointer types that might go out of scope.
        // Therefore, we create a QByteArray from the data, which can be passed
        // without problems through the meta object system. The good thing
        // is that for the conversion we do not need to know anything
        // about the actual encoding of “const char *”.
        QTest::addColumn<QByteArray>("propertyName");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.propertyCount(); ++i) {
            QTest::newRow(referenceClass.property(i).name()) << QByteArray(referenceClass.property(i).name());
        }
    }

    void testPropertyConformance()
    {
        QFETCH(QByteArray, propertyName);
        QMetaObject testClass = PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex = testClass.indexOfProperty(propertyName.constData());
        int referenceClassIndex = referenceClass.indexOfProperty(propertyName.constData());
        QMetaProperty referenceClassProperty = referenceClass.property(referenceClassIndex);
        QByteArray message;
        message += "Test if property \"";
        message += referenceClassProperty.name();
        message += "\" of class \"";
        message += referenceClass.className();
        message += "\" is also available in \"";
        message += testClass.className();
        message += "\".";
        QVERIFY2(testClassIndex >= 0, message.constData());
        QMetaProperty testClassProperty = testClass.property(testClassIndex);
        if (referenceClassProperty.hasNotifySignal()) {
            QVERIFY2(testClassProperty.hasNotifySignal(),
                     "If the reference class has a notify signal, "
                     "the test class must have also a notify signal.");
        }
        QCOMPARE(testClassProperty.isConstant(), referenceClassProperty.isConstant());
        QCOMPARE(testClassProperty.isDesignable(), referenceClassProperty.isDesignable());
        QCOMPARE(testClassProperty.isEnumType(), referenceClassProperty.isEnumType());
        if (referenceClassProperty.isEnumType()) {
            QCOMPARE(testClassProperty.enumerator().enumName(), referenceClassProperty.enumerator().enumName());
            QCOMPARE(testClassProperty.enumerator().isFlag(), referenceClassProperty.enumerator().isFlag());
            QCOMPARE(testClassProperty.enumerator().isScoped(), referenceClassProperty.enumerator().isScoped());
            QCOMPARE(testClassProperty.enumerator().isValid(), referenceClassProperty.enumerator().isValid());
            QCOMPARE(testClassProperty.enumerator().keyCount(), referenceClassProperty.enumerator().keyCount());
            QCOMPARE(testClassProperty.enumerator().name(), referenceClassProperty.enumerator().name());
            QCOMPARE(testClassProperty.enumerator().scope(), referenceClassProperty.enumerator().scope());
        }
        QCOMPARE(testClassProperty.isFinal(), referenceClassProperty.isFinal());
        QCOMPARE(testClassProperty.isFlagType(), referenceClassProperty.isFlagType());
        QCOMPARE(testClassProperty.isReadable(), referenceClassProperty.isReadable());
        QCOMPARE(testClassProperty.isResettable(), referenceClassProperty.isResettable());
        QCOMPARE(testClassProperty.isScriptable(), referenceClassProperty.isScriptable());
        QCOMPARE(testClassProperty.isStored(), referenceClassProperty.isStored());
        QCOMPARE(testClassProperty.isUser(), referenceClassProperty.isUser());
        QCOMPARE(testClassProperty.isValid(), referenceClassProperty.isValid());
        QCOMPARE(testClassProperty.isWritable(), referenceClassProperty.isWritable());
        QCOMPARE(testClassProperty.isWritable(), referenceClassProperty.isWritable());
        QCOMPARE(testClassProperty.name(), referenceClassProperty.name());
        if (referenceClassProperty.hasNotifySignal()) {
            QCOMPARE(testClassProperty.notifySignal().methodSignature(), referenceClassProperty.notifySignal().methodSignature());
            QCOMPARE(testClassProperty.notifySignal().methodType(), referenceClassProperty.notifySignal().methodType());
            QCOMPARE(testClassProperty.notifySignal().name(), referenceClassProperty.notifySignal().name());
            QCOMPARE(testClassProperty.notifySignal().parameterCount(), referenceClassProperty.notifySignal().parameterCount());
            QCOMPARE(testClassProperty.notifySignal().parameterNames(), referenceClassProperty.notifySignal().parameterNames());
            QCOMPARE(testClassProperty.notifySignal().parameterTypes(), referenceClassProperty.notifySignal().parameterTypes());
            QCOMPARE(testClassProperty.notifySignal().returnType(), referenceClassProperty.notifySignal().returnType());
            QCOMPARE(testClassProperty.notifySignal().revision(), referenceClassProperty.notifySignal().revision());
            QCOMPARE(testClassProperty.notifySignal().tag(), referenceClassProperty.notifySignal().tag());
            QCOMPARE(testClassProperty.notifySignal().typeName(), referenceClassProperty.notifySignal().typeName());
        }
        QCOMPARE(testClassProperty.type(), referenceClassProperty.type());
        QCOMPARE(testClassProperty.typeName(), referenceClassProperty.typeName());
        QCOMPARE(testClassProperty.userType(), referenceClassProperty.userType());
    }

    void testMethodConformance_data()
    {
        QTest::addColumn<QByteArray>("methodSignature");
        QTest::addColumn<int>("referenceClassIndex");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.methodCount(); ++i) {
            if (referenceClass.method(i).access() != QMetaMethod::Private) {
                // Exclude private methods from conformance check
                QTest::newRow(referenceClass.method(i).name().data()) << QMetaObject::normalizedSignature(referenceClass.method(i).methodSignature().data()) << i;
            }
        }
    }

    void testMethodConformance()
    {
        // We do only check QMetaObject::method() and
        // not QMetaObject::constructor because QColorDialog
        // does not provide its constructors to the
        // meta-object system.
        QFETCH(QByteArray, methodSignature);
        QFETCH(int, referenceClassIndex);
        QMetaObject testClass = PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex = testClass.indexOfMethod(methodSignature.data());
        QMetaMethod referenceClassMethod = referenceClass.method(referenceClassIndex);
        QByteArray message;
        message += "Test if method \"";
        message += referenceClassMethod.methodSignature();
        message += "\" of class \"";
        message += referenceClass.className();
        message += "\" is also available in \"";
        message += testClass.className();
        message += "\".";
        QVERIFY2(testClassIndex >= 0, message.constData());
        QMetaMethod testClassMethod = testClass.method(testClassIndex);
        QCOMPARE(testClassMethod.access(), referenceClassMethod.access());
        QCOMPARE(testClassMethod.isValid(), referenceClassMethod.isValid());
        QCOMPARE(testClassMethod.methodSignature(), referenceClassMethod.methodSignature());
        QCOMPARE(testClassMethod.methodType(), referenceClassMethod.methodType());
        QCOMPARE(testClassMethod.name(), referenceClassMethod.name());
        QCOMPARE(testClassMethod.parameterCount(), referenceClassMethod.parameterCount());
        QCOMPARE(testClassMethod.parameterNames(), referenceClassMethod.parameterNames());
        QCOMPARE(testClassMethod.parameterTypes(), referenceClassMethod.parameterTypes());
        QCOMPARE(testClassMethod.returnType(), referenceClassMethod.returnType());
        QCOMPARE(testClassMethod.revision(), referenceClassMethod.revision());
        QCOMPARE(testClassMethod.tag(), referenceClassMethod.tag());
        QCOMPARE(testClassMethod.typeName(), referenceClassMethod.typeName());
    }

    void testRttiConformance()
    {
        QMetaObject testClass = PerceptualColor::ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        QVERIFY2(testClass.inherits(referenceClass.superClass()),
                 "Test that PerceptualColor::ColorDialog inherits "
                 "from QColorDialog’s superclass.");
    }

    void testCurrentColorChangedSignal()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        m_qDialog.reset(new QColorDialog());
        m_perceptualDialog->show();
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog(m_perceptualDialog.data(), &PerceptualColor::ColorDialog::currentColorChanged);
        QSignalSpy spyQDialog(m_qDialog.data(), &QColorDialog::currentColorChanged);

        // Test that a simple “return key” click by the user
        // does not call this signal
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
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
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testCurrentColorProperty_data()
    {
        helperProvideQColors();
    }

    void testCurrentColorProperty()
    {
        QFETCH(QColor, color);
        QColor correctedColor;
        if (color.isValid()) {
            correctedColor = color.toRgb();
        } else {
            correctedColor = Qt::black;
        }
        QColor opaqueColor = correctedColor;
        opaqueColor.setAlpha(255);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_qDialog.reset(new QColorDialog);

        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), correctedColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), correctedColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(correctedColor.spec()));

        // Test that changing QColorDialog::ColorDialogOption::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), correctedColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), correctedColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(correctedColor.spec()));

        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), opaqueColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), opaqueColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(opaqueColor.spec()));

        // Test that changing QColorDialog::ColorDialogOption::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), opaqueColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), opaqueColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), static_cast<int>(opaqueColor.spec()));
    }

    void testSetCurrentColor()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->show();
        m_perceptualDialog->setCurrentColor(Qt::yellow);

        // Get internal LCH value
        const LchDouble color = m_perceptualDialog->d_pointer->m_currentOpaqueColor.toLch();

        // The very same LCH value has to be found in all widgets using it.
        // (This is not trivial, because even coming from RGB, because of
        // rounding errors, you can get out-of-gamut LCH values when the
        // original RGB value was near to the border. And the child
        // widgets may change the LCH value that is given to them
        // to fit it into the gamut – each widget with a different
        // algorithm.)
        QVERIFY(color.hasSameCoordinates(m_perceptualDialog->d_pointer->m_wheelColorPicker->currentColor()));
        QVERIFY(color.hasSameCoordinates(m_perceptualDialog->d_pointer->m_chromaHueDiagram->currentColor()));
        // We do not also control this here for
        // m_perceptualDialog->d_pointer->m_hlcSpinBox because this
        // widget rounds the given value to the current decimal precicion
        // it’s using. Therefore, it’s pointless to control here
        // for rounding errors.
    }

    void testOpen()
    {
        // Test our reference (QColorDialog)
        m_color = Qt::black;
        m_qDialog.reset(new QColorDialog);
        m_qDialog->setCurrentColor(Qt::white);
        m_qDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_qDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_qDialog->show();
        m_qDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);

        // Now test if PerceptualColor::ColorDialog does the same
        // thing as our reference
        m_color = Qt::black;
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->setCurrentColor(Qt::white);
        m_perceptualDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_perceptualDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_perceptualDialog->show();
        m_perceptualDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);
    }

    void testDefaultOptions()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_qDialog.reset(new QColorDialog);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), m_qDialog->testOption(QColorDialog::ColorDialogOption::NoButtons));
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), m_qDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel));
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::NoButtons), false);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::NoButtons), m_qDialog->options().testFlag(QColorDialog::ColorDialogOption::NoButtons));
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel), m_qDialog->options().testFlag(QColorDialog::ColorDialogOption::ShowAlphaChannel));
    }

    void testOptionDontUseNativeDialogAlwaysTrue()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::DontUseNativeDialog);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        m_perceptualDialog->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        m_perceptualDialog->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog | QColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        m_perceptualDialog->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog | QColorDialog::ColorDialogOption::ShowAlphaChannel);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        m_perceptualDialog->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog | QColorDialog::ColorDialogOption::ShowAlphaChannel | QColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
        QCOMPARE(m_perceptualDialog->options().testFlag(QColorDialog::ColorDialogOption::DontUseNativeDialog), true);
    }

    void testOptionShowAlpha()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, QColor(Qt::white)));
        m_qDialog.reset(new QColorDialog);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
        QVERIFY2(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), "ShowAlphaChannel successfully set.");
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QColor tempColor = QColor(1, 101, 201, 155);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QVERIFY2(!m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), "ShowAlphaChannel successfully set.");
        m_qDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        tempColor = QColor(5, 105, 205, 133);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testOptionNoButtons()
    {
        m_perceptualDialog.reset(             //
            new PerceptualColor::ColorDialog( //
                m_srgbBuildinColorSpace,      //
                QColor(Qt::white)             //
                )                             //
        );
        m_qDialog.reset(new QColorDialog);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons);
        QVERIFY2(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), "NoButtons successfully set to true.");
        m_qDialog->setOption(QColorDialog::ColorDialogOption::NoButtons);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(m_perceptualDialog->isVisible(), "Should still visible after Return key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(!m_perceptualDialog->isVisible(), "Should no longer be visible after Escape key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        QVERIFY2(!m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), "NoButtons successfully set to false.");
        m_qDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(!m_perceptualDialog->isVisible(), "Should no longer be visible after Return key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testSetOptionAndTestOptionInteraction()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), true);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), false);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::NoButtons), true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // define an option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        // change some other option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        // test if first option is still unchanged
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // define an option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        // change some other option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, true);
        // test if first option is still unchanged
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // define an option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        // change some other option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        // test if first option is still unchanged
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        // define an option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
        // change some other option
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
        // test if first option is still unchanged
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ColorDialogOption::ShowAlphaChannel), false);
        m_perceptualDialog.reset(nullptr);
    }

    void testAlphaSpinbox()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
        m_perceptualDialog->d_pointer->m_alphaGradientSlider->setValue(0.504);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaGradientSlider->value(), 0.504);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaSpinBox->value(), 50);
        QTest::keyClick(m_perceptualDialog->d_pointer->m_alphaSpinBox, Qt::Key_Up);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaGradientSlider->value(), 0.51);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaSpinBox->value(), 51);
    }

    void testSelectedColorAndSetVisible()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_qDialog.reset(new QColorDialog);
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setCurrentColor(QColor(Qt::blue));
        m_qDialog->setCurrentColor(QColor(Qt::blue));
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // Still no valid selectedColor() because the dialog still wasn't shown
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setVisible(true);
        m_qDialog->setVisible(true);
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->hide();
        m_qDialog->hide();
        QCOMPARE(m_perceptualDialog->selectedColor(), m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
    }

    void testAliases()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_qDialog.reset(new QColorDialog);

        // Test setting QColorDialog syntax
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), true);
        QCOMPARE(m_perceptualDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), true);
        m_qDialog->setOption(QColorDialog::ShowAlphaChannel);
        QCOMPARE(m_qDialog->testOption(QColorDialog::ShowAlphaChannel), true);
        QCOMPARE(m_qDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), true);

        // Test setting our alias syntax
        m_perceptualDialog->setOption(PerceptualColor::ColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::NoButtons), true);
        QCOMPARE(m_perceptualDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::NoButtons), true);
        m_qDialog->setOption(PerceptualColor::ColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE(m_qDialog->testOption(QColorDialog::NoButtons), true);
        QCOMPARE(m_qDialog->testOption(PerceptualColor::ColorDialog::ColorDialogOption::NoButtons), true);

        // Test if ColorDialogOptions is compatible (at least for == operator)
        // Configure conformance with our dialog:
        m_qDialog->setOption(QColorDialog::DontUseNativeDialog);
        QCOMPARE(m_perceptualDialog->options(), m_qDialog->options());
    }

    void testReadLightnessValues()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        myDialog->d_pointer->m_lchLightnessSelector->setValue(0.6);
        myDialog->d_pointer->readLightnessValue();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().l, 60);
    }

    void testReadHlcNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        QList<double> myValues = myDialog->d_pointer->m_hlcSpinBox->sectionValues();

        // Test with a normal value
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_hlcSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHlcNumericValues();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().h, 10);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().l, 11);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().c, 12);

        // Test with an out-of-gamut value.
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_hlcSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHlcNumericValues();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().h, 10);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().l, 11);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.toLch().c, 12);
    }

    void testReadHsvNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        QList<double> myValues = myDialog->d_pointer->m_hsvSpinBox->sectionValues();
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_hsvSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHsvNumericValues();
        QCOMPARE(qRound(myDialog->currentColor().hueF() * 360), 10);
        QCOMPARE(qRound(myDialog->currentColor().saturationF() * 255), 11);
        QCOMPARE(qRound(myDialog->currentColor().valueF() * 255), 12);
    }

    void testReadRgbHexValues()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);

        // Test some value
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#010203"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 1);
        QCOMPARE(myDialog->currentColor().green(), 2);
        QCOMPARE(myDialog->currentColor().blue(), 3);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), QStringLiteral("#010203"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ff0000"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 255);
        QCOMPARE(myDialog->currentColor().green(), 0);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), QStringLiteral("#ff0000"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ef6c00"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 239);
        QCOMPARE(myDialog->currentColor().green(), 108);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), QStringLiteral("#ef6c00"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ffff00"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 255);
        QCOMPARE(myDialog->currentColor().green(), 255);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), QStringLiteral("#ffff00"));
    }

    void testReadRgbNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        QList<double> myValues = myDialog->d_pointer->m_rgbSpinBox->sectionValues();
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_rgbSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readRgbNumericValues();
        QCOMPARE(myDialog->currentColor().red(), 10);
        QCOMPARE(myDialog->currentColor().green(), 11);
        QCOMPARE(myDialog->currentColor().blue(), 12);
    }

    void testSetCurrentOpaqueColor()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        LchDouble myOpaqueColor;
        myOpaqueColor.l = 30;
        myOpaqueColor.c = 40;
        myOpaqueColor.h = 50;
        const MultiColor myMultiColor = MultiColor::fromLch( //
            myDialog->d_pointer->m_rgbColorSpace,
            myOpaqueColor);
        myDialog->d_pointer->setCurrentOpaqueColor(myMultiColor, nullptr);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor, myMultiColor);
        QList<double> myValues = myDialog->d_pointer->m_rgbSpinBox->sectionValues();
        QCOMPARE(qRound(myValues.at(0)), 113);
        QCOMPARE(qRound(myValues.at(1)), 53);
        QCOMPARE(qRound(myValues.at(2)), 23);
    }

    void testUpdateColorPatch()
    {
        QScopedPointer<ColorDialog> myDialog(new PerceptualColor::ColorDialog);
        myDialog->d_pointer->m_currentOpaqueColor = MultiColor::fromRgbQColor( //
            myDialog->d_pointer->m_rgbColorSpace,
            QColor(1, 2, 3));
        myDialog->d_pointer->updateColorPatch();
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().red(), 1);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().green(), 2);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().blue(), 3);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().alphaF(), //
                 myDialog->d_pointer->m_alphaGradientSlider->value());
    }

    void testSizeGrip()
    {
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
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
        m_perceptualDialog->show();
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
        m_perceptualDialog->hide();
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
    }

    void testLayoutDimensions()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        // Test default value
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);

        // Test if values are correctly stored before showing
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded);
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::screenSizeDependent);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::screenSizeDependent);

        // Test if values are correctly stored after showing
        m_perceptualDialog->show();
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded);
        m_perceptualDialog->setLayoutDimensions( //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::screenSizeDependent);
        QCOMPARE(                                   //
            m_perceptualDialog->layoutDimensions(), //
            PerceptualColor::ColorDialog::DialogLayoutDimensions::screenSizeDependent);
    }

    void testApplyLayoutDimensions()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        // Test default value
        QCOMPARE(m_perceptualDialog->layoutDimensions(), PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed);

        m_perceptualDialog->d_pointer->m_layoutDimensions = PerceptualColor::ColorDialog::DialogLayoutDimensions::collapsed;
        m_perceptualDialog->d_pointer->applyLayoutDimensions();
        int collapsedWidth = m_perceptualDialog->width();

        m_perceptualDialog->d_pointer->m_layoutDimensions = PerceptualColor::ColorDialog::DialogLayoutDimensions::expanded;
        m_perceptualDialog->d_pointer->applyLayoutDimensions();
        int expandedWidth = m_perceptualDialog->width();

        QVERIFY2(collapsedWidth < expandedWidth,
                 "Verify that collapsed width of the dialog is smaller than "
                 "the expanded width.");
    }

    void testLayoutDimensionsChanged()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->setLayoutDimensions(ColorDialog::DialogLayoutDimensions::collapsed);
        QSignalSpy spyPerceptualDialog(
            // QObject to spy:
            m_perceptualDialog.data(),
            // Signal to spy:
            &PerceptualColor::ColorDialog::layoutDimensionsChanged);
        // Setting a different DialogLayoutDimensions will emit a signal
        m_perceptualDialog->setLayoutDimensions(ColorDialog::DialogLayoutDimensions::expanded);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        // Setting the same DialogLayoutDimensions will not emit a signal again
        m_perceptualDialog->setLayoutDimensions(ColorDialog::DialogLayoutDimensions::expanded);
        QCOMPARE(spyPerceptualDialog.count(), 1);
    }

    void testRoundingErrors_data()
    {
        QTest::addColumn<QColor>("color");
        QTest::newRow("Qt::yellow") << QColor(Qt::yellow);
        qreal red = 1;
        qreal green = 1;
        qreal blue = 0;
        while (blue < 1) {
            QTest::newRow(QString(QStringLiteral("RGB %1 %2 %3")).arg(red).arg(green).arg(blue).toUtf8().data()) << QColor::fromRgbF(red, green, blue);
            blue += 0.1;
        }
    }

    void testRoundingErrors()
    {
        QFETCH(QColor, color);

        // Moving around between the widgets with the Tab key should
        // never trigger a value change. (There could be a value
        // change because of rounding errors if the value gets updated
        // after the focus leaves, even though no editing has been
        // done. This would not be correct, and this test controls this.)

        m_perceptualDialog.reset(new PerceptualColor::ColorDialog());
        m_perceptualDialog->setCurrentColor(color);
        m_perceptualDialog->show();
        QApplication::setActiveWindow(m_perceptualDialog.data());

        for (int i = 0; i < m_perceptualDialog->d_pointer->m_tabWidget->count(); i++) {
            m_perceptualDialog->d_pointer->m_tabWidget->setCurrentIndex(i);
            const QWidget *oldFocusWidget = QApplication::focusWidget();
            const QColor oldColor = m_perceptualDialog->currentColor();
            const MultiColor oldOpaqueLchColor = //
                m_perceptualDialog->d_pointer->m_currentOpaqueColor;
            do {
                QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
                QCOMPARE(oldColor, m_perceptualDialog->currentColor());
                QVERIFY(oldOpaqueLchColor == m_perceptualDialog->d_pointer->m_currentOpaqueColor);
            } while (oldFocusWidget != QApplication::focusWidget());
        };
    }

    void testYellowRounding()
    {
        // During development was observed a particular bug for which
        // we test here.

        // As we expect rounding errors, we define a tolerance range,
        // which is used both for the assertions and for the actual test.
        // This is necessary to guarantee that this test does not produce
        // false-positives just because the rounding behaviour of the
        // library has changed.
        const int toleranceRange = 1;

        // Create a ColorDialog
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);

        // Start with Qt::yellow as initial color.
        // If this RGB value is interpreted in the sRGB (LittleCMS build-in)
        // profile, it has a particular property. Because of the irregular
        // shape of the sRGB color space at this position, thinking in
        // LCH values, when reducing (!) the chroma step-by-step, we run
        // out-of-gamut, before going again in-gamut at even lower chroma
        // values.
        m_perceptualDialog->setCurrentColor(QColor(Qt::yellow));
        // The value is also converted to HLC 100°, 98%, 95 (rounded)
        // visible in the HLC spin box.
        QList<double> hlc = m_perceptualDialog->d_pointer->m_hlcSpinBox->sectionValues();
        QVERIFY(hlc.at(0) >= 100 - toleranceRange); // assertion
        QVERIFY(hlc.at(0) <= 100 + toleranceRange); // assertion
        QVERIFY(hlc.at(1) >= 98 - toleranceRange);  // assertion
        QVERIFY(hlc.at(1) <= 98 + toleranceRange);  // assertion
        QVERIFY(hlc.at(2) >= 95 - toleranceRange);  // assertion
        QVERIFY(hlc.at(2) <= 95 + toleranceRange);  // assertion
        // Now, the user clicks on the “Apply” button within the HLC spin box.
        // We simulate this by simply calling the slot that is connected
        // to this action:
        m_perceptualDialog->d_pointer->readHlcNumericValues();
        // Now, during development there was a bug observerd: The buggy
        // behaviour was that the chroma value was changed from 95 to 24.
        // The expected result was that the chroma value only changes
        // slightly because of rounding (or ideally not at all).
        hlc = m_perceptualDialog->d_pointer->m_hlcSpinBox->sectionValues();
        QVERIFY(hlc.at(2) >= 95 - toleranceRange);
        QVERIFY(hlc.at(2) <= 95 + toleranceRange);
    }

    void testBlackHSV()
    {
        // In the HSV color space, if V is 0 then the color is black.
        // Both, H and S are meaningless. When converting from other
        // color spaces, they get probably a default value. However,
        // when the user is editing the HSV spin box, we does not expect
        // that H or S change when switching from one section to another
        // or when the focus leaves. Make sure that H and S are preserved
        // during editing even if V becomes 0:

        // Create a ColorDialog
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);

        const QList<double> hsvTestData {201, 33, 0};
        m_perceptualDialog->d_pointer->m_hsvSpinBox->setSectionValues(hsvTestData);
        QCOMPARE(m_perceptualDialog->d_pointer->m_hsvSpinBox->sectionValues(), //
                 hsvTestData);
        m_perceptualDialog->d_pointer->readHsvNumericValues();
        QCOMPARE(m_perceptualDialog->d_pointer->m_hsvSpinBox->sectionValues(), //
                 hsvTestData);
    }

    void testRoundingMultipleError()
    {
        // This is a test for a bug discoverd during development.

        // Create a ColorDialog:
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);

        // The user puts into the HLC spin box the value 100° 98% 94:
        m_perceptualDialog->d_pointer->m_hlcSpinBox->setSectionValues( //
            QList<double> {100, 98, 94});
        // This is an out-of-gamut color which is not corrected until
        // the focus will leave the widget or the Return key is pressed.
        // The nearest in-gamut color is around 100° 97% 94; this color
        // is used internally to perform the conversion to RGB and other
        // color spaces. (It is however still not visible in the HLC
        // spin box.)
        //
        // The RGB spin box becomes 253, 255, 10:
        const QList<double> expectedRgbValues {253, 255, 10};
        QCOMPARE(m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues(), //
                 expectedRgbValues);
        // Now, the user finishes the editing process (the focus leaves
        // the widget or the Return key is pressed or the action button
        // is clicked):
        m_perceptualDialog->d_pointer->updateHlcButBlockSignals();
        // The buggy result during development phase was an RGB value
        // of 252 254 4. Why?
        // - The internal value was around 100° 97% 94, but not exactly.
        // - Now, the exact (!) value of 100° 97% 94 is applied, and this
        //   one, converted to RGB, triggers a different rounding.
        // The expected result is however still the very same RGB value
        // as above:
        QCOMPARE(m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues(), //
                 expectedRgbValues);
    }

    void testRgbHexRounding()
    {
        // This is a test for a bug discoverd during development.
        // QColor can produce a QString thas contains a hexadecimal
        // (integer) representation of the color, just as used in
        // HTML. Example: #0000FF is blue. When rounding to
        // integers, apparently it does not use round(), but floor().
        // That is not useful and not inconsistend with the rest of our
        // dialog. We want correct rounding!

        // Create a ColorDialog:
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);

        // Set a color that triggers the rounding error:
        LchDouble testColor;
        testColor.h = 100;
        testColor.l = 97;
        testColor.c = 94;
        m_perceptualDialog->d_pointer->setCurrentOpaqueColor(
            // Color:
            MultiColor::fromLch(m_perceptualDialog->d_pointer->m_rgbColorSpace, testColor),
            // Widget to ignore:
            nullptr);

        // Get the actual result
        QColor actualHex;
        actualHex.setNamedColor(m_perceptualDialog->d_pointer->m_rgbLineEdit->text());

        // Get the expected result (We assume our own RGB spin box rounds
        // correctly.)
        const QList<double> rgbList =
            // The the values from the MultiSpinBox:
            m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues();
        QColor expectedHex = QColor::fromRgb(
            // The MultiSpinBox might have decimal places, so we round
            // here again.
            qRound(rgbList.at(0)),
            qRound(rgbList.at(1)),
            qRound(rgbList.at(2)));

        // Compare
        QCOMPARE(actualHex, expectedHex);
    }

    void testSnippet02()
    {
        snippet02();
    }

    void testSnippet03()
    {
        snippet03();
    }

    void testSnippet05()
    {
        TestColorDialogSnippetClass mySnippets;
        mySnippets.testSnippet05();
    }

    void benchmarkCreateAndShowPerceptualDialog()
    {
        m_perceptualDialog.reset(nullptr);
        QBENCHMARK {
            m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
            m_perceptualDialog->show();
            m_perceptualDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkCreateAndShowMaximizedPerceptualDialog()
    {
        m_perceptualDialog.reset(nullptr);
        QBENCHMARK {
            m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
            m_perceptualDialog->showMaximized();
            m_perceptualDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkCreateAndShowQColorDialog()
    {
        m_qDialog.reset(nullptr);
        QBENCHMARK {
            m_qDialog.reset(new QColorDialog);
            m_qDialog->show();
            m_qDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkChangeColorPerceptualFirstTab()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
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

    void benchmarkChangeColorPerceptualSecondTab()
    {
        m_perceptualDialog.reset(new PerceptualColor::ColorDialog);
        m_perceptualDialog->show();

        QTabWidget *theTabWidget = m_perceptualDialog->findChild<QTabWidget *>();
        QVERIFY2(theTabWidget != nullptr, //
                 "Assert that theTabWidget has actually been found.");
        constexpr int myIndex = 1;
        // Assert that we got the correct tab widget:
        QCOMPARE(theTabWidget->tabText(myIndex), QStringLiteral("&Lightness-based"));
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

    void benchmarkChangeColorQColorDialog()
    {
        m_qDialog.reset(new QColorDialog);
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

private:
    void unused()
    {
        // These will not be called in the unit tests because getColor()
        // does not return without user interaction!
        // They are noted here to avoid a warning about “unused function”.
        snippet01();
        snippet04();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorDialog)
// The following “include” is necessary because we do not use a header file:
#include "testcolordialog.moc"
