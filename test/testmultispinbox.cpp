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

#include "PerceptualColor/multispinbox.h"

#include <QLabel>
#include <QLineEdit>

namespace PerceptualColor {

class TestMultiSpinBox : public QObject
{

    Q_OBJECT

public:
    TestMultiSpinBox(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    MultiSpinBox::SectionList myConfiguration;
    static void voidMessageHandler(
        QtMsgType,
        const QMessageLogContext &,
        const QString &
    ) {
        // dummy message handler that does not print messages
    }

private Q_SLOTS:
    void initTestCase() {
        // Called before the first test function is executed
        MultiSpinBox::Section mySection;
        mySection.minimum = 0;
        mySection.maximum = 360;
        mySection.prefix = "";
        mySection.suffix = "°";
        myConfiguration.append(mySection);
        mySection.maximum = 100;
        mySection.prefix = "  ";
        mySection.suffix = "%";
        myConfiguration.append(mySection);
        mySection.maximum = 255;
        mySection.prefix = "  ";
        mySection.suffix = "";
        myConfiguration.append(mySection);
    }

    void cleanupTestCase() {
        // Called after the last test function was executed
    }

    void init() {
        // Called before each test function is executed
    }

    void cleanup() {
        // Called after every test function
    }

    void testConstructor() {
        // Test the the constructor does not crash
        PerceptualColor::MultiSpinBox myMulti;
        // Test basic constructor results
        QVERIFY2(
            myMulti.m_configuration.length() > 0,
            "Make sure the default configuration has at least 1 section."
        );
    }

    void testCurrentSectionIndex() {
        MultiSpinBox test;
        // Test default index
        QCOMPARE(test.m_currentSectionIndex, 0);
        
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if setting negative value is ignored
        QVERIFY_EXCEPTION_THROWN(test.setCurrentSectionIndex(-1), int);
        QCOMPARE(test.m_currentSectionIndex, 0);
        QVERIFY_EXCEPTION_THROWN(test.setCurrentSectionIndex(-100), int);
        QCOMPARE(test.m_currentSectionIndex, 0);
        // Test setting too high values is ignored
        QVERIFY_EXCEPTION_THROWN(test.setCurrentSectionIndex(100), int);
        QCOMPARE(test.m_currentSectionIndex, 0);
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
        
        // Test if correct sections are stored correctly
        MultiSpinBox::SectionList mySectionList;
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        test.setConfiguration(mySectionList);
        test.setCurrentSectionIndex(2);
        QCOMPARE(test.m_currentSectionIndex, 2);
    }
    
    void testSetConfiguration() {
        // Correct configurations should be applied as-is.
        MultiSpinBox::SectionList mySectionList;
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        MultiSpinBox test;
        test.setConfiguration(mySectionList);
        QCOMPARE(test.configuration().count(), 3);

        // Empty configurations shall be ignored
        test.setConfiguration(MultiSpinBox::SectionList());
        QCOMPARE(test.configuration().count(), 3);

        // Invalid values should be adapted
        mySectionList.clear();
        MultiSpinBox::Section myInvalidSection;
        myInvalidSection.minimum = 50;
        myInvalidSection.value = 40;
        myInvalidSection.maximum = 30;
        mySectionList.append(myInvalidSection);
        test.setConfiguration(mySectionList);
        QVERIFY2(
            test.m_configuration.at(0).minimum
                <= test.m_configuration.at(0).maximum,
            "minimum <= maximum"
        );
        QVERIFY2(
            test.m_configuration.at(0).minimum
                <= test.m_configuration.at(0).value,
            "minimum <= value"
        );
        QVERIFY2(
            test.m_configuration.at(0).value
                <= test.m_configuration.at(0).maximum,
            "value <= maximum"
        );

        // Invalid values should be adapted
        mySectionList.clear();
        myInvalidSection.minimum = -50;
        myInvalidSection.value = -60;
        myInvalidSection.maximum = -70;
        mySectionList.append(myInvalidSection);
        test.setConfiguration(mySectionList);
        QVERIFY2(
            test.m_configuration.at(0).minimum
                <= test.m_configuration.at(0).maximum,
            "minimum <= maximum"
        );
        QVERIFY2(
            test.m_configuration.at(0).minimum
                <= test.m_configuration.at(0).value,
            "minimum <= value"
        );
        QVERIFY2(
            test.m_configuration.at(0).value
                <= test.m_configuration.at(0).maximum,
            "value <= maximum"
        );
    }

    void testApplySectionConfiguration() {
        QDoubleSpinBox myDouble;
        myDouble.setMinimum(10);
        myDouble.setValue(20);
        myDouble.setMaximum(30);
        myDouble.setPrefix("old");
        myDouble.setSuffix("old");
        PerceptualColor::MultiSpinBox::Section section;
        section.minimum = 110;
        section.value = 120;
        section.maximum = 130;
        section.prefix = "new";
        section.suffix = "new";
        PerceptualColor::MultiSpinBox::applySectionConfiguration(
            section,
            &myDouble
        );
        QCOMPARE(myDouble.minimum(), 110); // Should have been applied.
        QCOMPARE(myDouble.value(), 120); // Should have been applied.
        QCOMPARE(myDouble.maximum(), 130); // Should have been applied.
        QCOMPARE(myDouble.prefix(), "old"); // Should not have been applied.
        QCOMPARE(myDouble.suffix(), "old"); // Should not have been applied.
    }

    void testApplySectionConfigurationToFormatSpinbox() {
        PerceptualColor::MultiSpinBox myMulti;
        myMulti.m_formatSpinBoxForCurrentValue.setMinimum(10);
        myMulti.m_formatSpinBoxForCurrentValue.setValue(20);
        myMulti.m_formatSpinBoxForCurrentValue.setMaximum(30);
        myMulti.m_formatSpinBoxForCurrentValue.setPrefix("old");
        myMulti.m_formatSpinBoxForCurrentValue.setSuffix("old");
        PerceptualColor::MultiSpinBox::Section section;
        section.minimum = 110;
        section.value = 120;
        section.maximum = 130;
        section.prefix = "new";
        section.suffix = "new";
        PerceptualColor::MultiSpinBox::SectionList myList;
        myList.append(section);
        assert(myList.length() == 1);
        myMulti.m_configuration = myList;
        myMulti.applySectionConfigurationToFormatSpinbox(0);
        QCOMPARE(myMulti.m_formatSpinBoxForCurrentValue.minimum(), 110);
        QCOMPARE(myMulti.m_formatSpinBoxForCurrentValue.value(), 120);
        QCOMPARE(myMulti.m_formatSpinBoxForCurrentValue.maximum(), 130);
        QCOMPARE(myMulti.m_formatSpinBoxForCurrentValue.prefix(), "old");
        QCOMPARE(myMulti.m_formatSpinBoxForCurrentValue.suffix(), "old");
        
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        QVERIFY_EXCEPTION_THROWN(
            myMulti.applySectionConfigurationToFormatSpinbox(5),
            int
        );
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }
    
    void testMinimalSizeHint() {
        PerceptualColor::MultiSpinBox myMulti;
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
        myMulti.setConfiguration(myConfiguration);
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
    }
    
    void testInitializeQDoubleSpinBox() {
        QDoubleSpinBox myDouble;
        PerceptualColor::MultiSpinBox myMulti;
        myDouble.setDecimals(8);
        myDouble.show();
        myMulti.initializeQDoubleSpinBox(&myDouble);
        QVERIFY2(
            myDouble.decimals() != 8,
            "Verify that the strange value of 8 has been changed during "
                "the configuration."
        );
        QVERIFY2(
            myDouble.isHidden(),
            "Verify that the widget is hidden."
        );
        QVERIFY2(
            !myDouble.isVisible(),
            "Verify that the widget is not visible."
        );
    }

    void testSizeHint() {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.maximum = 9;
        section.prefix = "abcdefghij";
        section.suffix = "abcdefghij";
        config.append(section);
        myMulti.setConfiguration(config);
        const int referenceWidth = myMulti.sizeHint().width();

        // Now test various configurations that should lead to bigger sizes…
        
        section.minimum = -1;
        section.maximum = 9;
        section.prefix = "abcdefghij";
        section.suffix = "abcdefghij";
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = 1;
        section.maximum = 19;
        section.prefix = "abcdefghij";
        section.suffix = "abcdefghij";
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = -1;
        section.maximum = 9;
        section.prefix = "abcdefghijh";
        section.suffix = "abcdefghij";
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = -1;
        section.maximum = 9;
        section.prefix = "abcdefghij";
        section.suffix = "abcdefghijh";
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);
    }

    void testUpdatePrefixValueSuffixText() {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = "abc";
        section.suffix = "def";
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = "ghi";
        section.suffix = "jkl";
        config.append(section);
        myMulti.setConfiguration(config);
        myMulti.m_currentSectionIndex = 1;
        myMulti.updatePrefixValueSuffixText();
        QCOMPARE(
            myMulti.m_currentSectionTextBeforeValue,
            "abc8defghi"
        );
        QCOMPARE(
            myMulti.m_currentSectionTextOfTheValue,
            "80"
        );
        QCOMPARE(
            myMulti.m_currentSectionTextAfterValue,
            "jkl"
        );
    }
    
    void testSetCurrentSectionIndexWithoutSelectingText() {
        PerceptualColor::MultiSpinBox myMulti;
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = "abc";
        section.suffix = "def";
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = "ghi";
        section.suffix = "jkl";
        config.append(section);
        myMulti.setConfiguration(config);
        myMulti.setCurrentSectionIndex(1);
        QCOMPARE(
            myMulti.m_currentSectionIndex,
            1
        );
        QVERIFY2(
            !myMulti.lineEdit()->hasSelectedText(),
            "No text should be selected."
        );
    }
    
    void testSetCurrentSectionIndex() {
        PerceptualColor::MultiSpinBox myMulti;
        myMulti.setConfiguration(myConfiguration);
        myMulti.setCurrentSectionIndex(2);
        QCOMPARE(myMulti.m_currentSectionIndex, 2);
        myMulti.setCurrentSectionIndex(0);
        QCOMPARE(myMulti.m_currentSectionIndex, 0);
        myMulti.setCurrentSectionIndex(1);
        QCOMPARE(myMulti.m_currentSectionIndex, 1);
        myMulti.setCurrentSectionIndex(2);
        QCOMPARE(myMulti.m_currentSectionIndex, 2);
        myMulti.setCurrentSectionIndex(0);
        QCOMPARE(myMulti.m_currentSectionIndex, 0);
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = "abc";
        section.suffix = "def";
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = "ghi";
        section.suffix = "jkl";
        config.append(section);
        myMulti.setConfiguration(config);
        myMulti.setCurrentSectionIndex(1);
        QCOMPARE(
            myMulti.m_currentSectionIndex,
            1
        );
        QVERIFY2(
            !myMulti.lineEdit()->hasSelectedText(),
            "No text should be selected because invisible widgets "
                "have no focus."
        );
    }

    void testStepEnabled() {
        PerceptualColor::MultiSpinBox myMulti;
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = "abc";
        section.suffix = "def";
        config.append(section);
        myMulti.setConfiguration(config);
        QAbstractSpinBox::StepEnabled flags = myMulti.stepEnabled();
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be enabled"
        );
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be enabled"
        );
        section.value = 9;
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        flags = myMulti.stepEnabled();
        QVERIFY2(
            !flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be disabled"
        );
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be enabled"
        );
        section.value = 10;
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        flags = myMulti.stepEnabled();
        QVERIFY2(
            !flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be disabled"
        );
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be enabled"
        );
        section.value = 1;
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        flags = myMulti.stepEnabled();
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be enabled"
        );
        QVERIFY2(
            !flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be disabled"
        );
        section.value = 0;
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        flags = myMulti.stepEnabled();
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be enabled"
        );
        QVERIFY2(
            !flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be disabled"
        );
        section.value = -1;
        config.clear();
        config.append(section);
        myMulti.setConfiguration(config);
        flags = myMulti.stepEnabled();
        QVERIFY2(
            flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            "Step up should be enabled"
        );
        QVERIFY2(
            !flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            "Step down should be disabled"
        );
    }
    
    void testConfiguration() {
        PerceptualColor::MultiSpinBox myMulti;
        MultiSpinBox::SectionList config;
        MultiSpinBox::Section section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = "abc";
        section.suffix = "def";
        config.append(section);
        myMulti.setConfiguration(config);
        QCOMPARE(myMulti.configuration().count(), 1);
        QCOMPARE(myMulti.configuration().at(0).minimum, 1);
        QCOMPARE(myMulti.configuration().at(0).value, 8);
        QCOMPARE(myMulti.configuration().at(0).maximum, 9);
        QCOMPARE(myMulti.configuration().at(0).prefix, "abc");
        QCOMPARE(myMulti.configuration().at(0).suffix, "def");
    }

    void testFocusIntegrationTab() {
        QWidget *parentWidget = new QWidget();
        QSpinBox *widget1 =
            new QSpinBox(parentWidget);
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget);
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setConfiguration(myConfiguration);
        QSpinBox *widget3 =
            new QSpinBox(parentWidget);
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel("&Test", parentWidget);
        label2->setBuddy(widget2);
        widget1->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget);
        // Assert that the setup is okay.
        if (!widget1->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget3->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (QApplication::focusWidget() != widget1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->m_configuration.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start testing
        
        // Apparently it isn’t possible to call simply the key click
        // on the parent widget. This code fails sometimes:
        // QTest::keyClick(parentWidget, Qt::Key::Key_Tab);
        // Therefore, we call QTest::keyClick() on QApplication::focusWidget()
        
        // Move focus from widget1 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 1);
        // Move focus from widget2/section1 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 2);
        // Move focus from widget2/section2 to widget3
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        QCOMPARE(widget2->m_currentSectionIndex, 0);
        // Move focus from widget3 to widget2/section2
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 2);
        // Move focus from widget2/section2 to widget2/section1
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 1);
        // Move focus from widget2/section1 to widget2/section0
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 0);
        // Move focus from widget2/section0 to widget1
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget1);
        QCOMPARE(widget2->m_currentSectionIndex, 0);
        // Move focus from widget1 to widget2/section0
qDebug() << "start with" << widget2->m_currentSectionIndex;
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_T,
            Qt::KeyboardModifier::AltModifier
        );

qDebug() << "stop with" << widget2->m_currentSectionIndex;
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->m_currentSectionIndex, 1);
        // Make sure that MultiSpinBox does not react on some focus policies
        widget2->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        widget1->setFocus();
        if (QApplication::focusWidget() != widget1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        // Make sure that MultiSpinBox does not react on some focus policies
        widget2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        widget1->setFocus();
        if (QApplication::focusWidget() != widget1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        
        // Delete the widgets (including child widgets)
        delete parentWidget;
    }

};

}

QTEST_MAIN(PerceptualColor::TestMultiSpinBox)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinbox.moc"
