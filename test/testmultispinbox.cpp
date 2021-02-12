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

#include "perceptualcolorlib_qtconfiguration.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/multispinbox.h"
// Second, the private implementation.
#include "multispinbox_p.h"

#include <QtTest>

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

namespace PerceptualColor {

class TestMultiSpinBox : public QObject
{

    Q_OBJECT

public:
    TestMultiSpinBox(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    QList<MultiSpinBox::SectionData> myConfiguration;
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
        MultiSpinBox::SectionData mySection;
        mySection.minimum = 0;
        mySection.maximum = 360;
        mySection.prefix = QLatin1String();
        mySection.suffix = QStringLiteral(u"°");
        myConfiguration.append(mySection);
        mySection.maximum = 100;
        mySection.prefix = QStringLiteral(u"  ");
        mySection.suffix = QStringLiteral(u"%");
        myConfiguration.append(mySection);
        mySection.maximum = 255;
        mySection.prefix = QStringLiteral(u"  ");
        mySection.suffix = QLatin1String();
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
            myMulti.d_pointer->m_sections.length() > 0,
            "Make sure the default configuration has at least 1 section."
        );
    }

    void testInteraction() {
        // This test is suprisingly slow.
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        widget->show();
        widget->setSections (myConfiguration);
        // Assert that the setup is okay.
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"0°  0%  0")
        );
        // Go to begin of the line edit
        QTest::keyClick(widget.data(), Qt::Key_Home);
        // Select the first “0”:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        // Write “45”
        QTest::keyClicks(widget.data(), QStringLiteral(u"45"));
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  0%  0")
        );
        // Select “45”
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        // Copy to clipboard
        QTest::keyClick(widget.data(), Qt::Key_C, Qt::ControlModifier, 0);
        // Go to second section
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  0%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  0%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  0%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  0%  0")
        );
        // Select second section:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        QCOMPARE(
            widget->lineEdit()->selectedText(),
            QStringLiteral(u"0")
        );
        // Take “45” from clipboard
        QTest::keyClick(widget.data(), Qt::Key_V, Qt::ControlModifier, 0);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  45%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  45%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  45%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  45%  0")
        );
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"45°  45%  0")
        );
    }

    void testCurrentSectionIndex() {
        MultiSpinBox test;
        // Test default index
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if setting negative value is ignored
        QVERIFY_EXCEPTION_THROWN(
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-1),
            int
        );
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        QVERIFY_EXCEPTION_THROWN(
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-100),
            int
        );
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // Test setting too high values is ignored
        QVERIFY_EXCEPTION_THROWN(
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(100),
            int
        );
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
        
        // Test if correct sections are stored correctly
        QList<MultiSpinBox::SectionData> mySectionList;
        mySectionList.append(MultiSpinBox::SectionData());
        mySectionList.append(MultiSpinBox::SectionData());
        mySectionList.append(MultiSpinBox::SectionData());
        test.setSections (mySectionList);
        test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(test.d_pointer->m_currentIndex, 2);
    }
    
    void testSetConfiguration() {
        // Correct configurations should be applied as-is.
        QList<MultiSpinBox::SectionData> mySectionList;
        mySectionList.append(MultiSpinBox::SectionData());
        mySectionList.append(MultiSpinBox::SectionData());
        mySectionList.append(MultiSpinBox::SectionData());
        MultiSpinBox test;
        test.setSections (mySectionList);
        QCOMPARE(test.sections().count(), 3);

        // Empty configurations shall be ignored
        test.setSections (QList<MultiSpinBox::SectionData>());
        QCOMPARE(test.sections().count(), 3);

        // Invalid values should be adapted
        mySectionList.clear();
        MultiSpinBox::SectionData myInvalidSection;
        myInvalidSection.minimum = 50;
        myInvalidSection.value = 40;
        myInvalidSection.maximum = 30;
        mySectionList.append(myInvalidSection);
        test.setSections (mySectionList);
        QVERIFY2(
            test.d_pointer->m_sections.at(0).minimum
                <= test.d_pointer->m_sections.at(0).maximum,
            "minimum <= maximum"
        );
        QVERIFY2(
            test.d_pointer->m_sections.at(0).minimum
                <= test.d_pointer->m_sections.at(0).value,
            "minimum <= value"
        );
        QVERIFY2(
            test.d_pointer->m_sections.at(0).value
                <= test.d_pointer->m_sections.at(0).maximum,
            "value <= maximum"
        );

        // Invalid values should be adapted
        mySectionList.clear();
        myInvalidSection.minimum = -50;
        myInvalidSection.value = -60;
        myInvalidSection.maximum = -70;
        mySectionList.append(myInvalidSection);
        test.setSections (mySectionList);
        QVERIFY2(
            test.d_pointer->m_sections.at(0).minimum
                <= test.d_pointer->m_sections.at(0).maximum,
            "minimum <= maximum"
        );
        QVERIFY2(
            test.d_pointer->m_sections.at(0).minimum
                <= test.d_pointer->m_sections.at(0).value,
            "minimum <= value"
        );
        QVERIFY2(
            test.d_pointer->m_sections.at(0).value
                <= test.d_pointer->m_sections.at(0).maximum,
            "value <= maximum"
        );
    }

    void testMinimalSizeHint() {
        PerceptualColor::MultiSpinBox myMulti;
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
        myMulti.setSections (myConfiguration);
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
    }

    void testSizeHint() {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abcdefghij");
        section.suffix = QStringLiteral(u"abcdefghij");
        config.append(section);
        myMulti.setSections (config);
        const int referenceWidth = myMulti.sizeHint().width();

        // Now test various configurations that should lead to bigger sizes…
        
        section.minimum = -1;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abcdefghij");
        section.suffix = QStringLiteral(u"abcdefghij");
        config.clear();
        config.append(section);
        myMulti.setSections (config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = 1;
        section.maximum = 19;
        section.prefix = QStringLiteral(u"abcdefghij");
        section.suffix = QStringLiteral(u"abcdefghij");
        config.clear();
        config.append(section);
        myMulti.setSections (config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = -1;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abcdefghijh");
        section.suffix = QStringLiteral(u"abcdefghij");
        config.clear();
        config.append(section);
        myMulti.setSections (config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.minimum = -1;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abcdefghij");
        section.suffix = QStringLiteral(u"abcdefghijh");
        config.clear();
        config.append(section);
        myMulti.setSections (config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);
    }

    void testUpdatePrefixValueSuffixText() {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abc");
        section.suffix = QStringLiteral(u"def");
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = QStringLiteral(u"ghi");
        section.suffix = QStringLiteral(u"jkl");
        config.append(section);
        myMulti.setSections (config);
        myMulti.d_pointer->m_currentIndex = 1;
        myMulti.d_pointer->updatePrefixValueSuffixText();
        QCOMPARE(
            myMulti.d_pointer->m_textBeforeCurrentValue,
            QStringLiteral(u"abc8defghi")
        );
        QCOMPARE(
            myMulti.d_pointer->m_textOfCurrentValue,
            QStringLiteral(u"80")
        );
        QCOMPARE(
            myMulti.d_pointer->m_textAfterCurrentValue,
            QStringLiteral(u"jkl")
        );
    }
    
    void testSetCurrentSectionIndexWithoutSelectingText() {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abc");
        section.suffix = QStringLiteral(u"def");
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = QStringLiteral(u"ghi");
        section.suffix = QStringLiteral(u"jkl");
        config.append(section);
        myMulti.setSections (config);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(
            myMulti.d_pointer->m_currentIndex,
            1
        );
        QVERIFY2(
            !myMulti.lineEdit()->hasSelectedText(),
            "No text should be selected."
        );
    }
    
    void testSetCurrentSectionIndex() {
        PerceptualColor::MultiSpinBox myMulti;
        myMulti.setSections (myConfiguration);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 2);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 2);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abc");
        section.suffix = QStringLiteral(u"def");
        config.append(section);
        section.minimum = 10;
        section.value = 80;
        section.maximum = 90;
        section.prefix = QStringLiteral(u"ghi");
        section.suffix = QStringLiteral(u"jkl");
        config.append(section);
        myMulti.setSections (config);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(
            myMulti.d_pointer->m_currentIndex,
            1
        );
        QVERIFY2(
            !myMulti.lineEdit()->hasSelectedText(),
            "No text should be selected because invisible widgets "
                "have no focus."
        );
    }

    void testStepEnabledSimple() {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abc");
        section.suffix = QStringLiteral(u"def");
        config.append(section);
        myMulti.setSections (config);
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
        myMulti.setSections (config);
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
        myMulti.setSections (config);
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
        myMulti.setSections (config);
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
        myMulti.setSections (config);
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
        myMulti.setSections (config);
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

    void testStepEnabledAndSectionIndex_data() {
        QTest::addColumn<int>("cursorPosition");
        QTest::addColumn<int>("sectionIndex");
        QTest::addColumn<int>("minimum");
        QTest::addColumn<int>("value");
        QTest::addColumn<int>("maximum");
        QTest::addColumn<bool>("StepUpEnabled");
        QTest::addColumn<bool>("StepDownEnabled");

        QTest::newRow("0") << 0 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("1") << 1 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("2") << 2 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("4") << 4 << 1 << 0 << 5 << 100 << true << true ;
        QTest::newRow("5") << 5 << 1 << 0 << 5 << 100 << true << true ;
        QTest::newRow("6") << 6 << 1 << 0 << 5 << 100 << true << true ;
        QTest::newRow("8") << 8 << 2 << 0 << 0 << 255 << true << false;
        QTest::newRow("9") << 9 << 2 << 0 << 0 << 255 << true << false;
    }

    void testStepEnabledAndSectionIndex() {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        QList<MultiSpinBox::SectionData> specialConfiguration =
            myConfiguration;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        specialConfiguration[sampleSectionNumber].value = sampleValue;
        widget->setSections (specialConfiguration);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(
            sampleSectionNumber
        );
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sections().at(sampleSectionNumber).value
            != sampleValue
        ) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        QAbstractSpinBox::StepEnabled flags;

        // Actual testing
        QFETCH(int, cursorPosition);
        QFETCH(int, sectionIndex);
        QFETCH(int, minimum);
        QFETCH(int, value);
        QFETCH(int, maximum);
        QFETCH(bool, StepUpEnabled);
        QFETCH(bool, StepDownEnabled);
        widget->lineEdit()->setCursorPosition(cursorPosition);
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        flags = widget->stepEnabled();
        QCOMPARE(
            widget->d_pointer->m_currentIndex,
            sectionIndex
        );
        QCOMPARE(
            widget
                ->d_pointer
                ->m_sections
                .at(widget->d_pointer->m_currentIndex)
                .minimum,
            minimum
        );
        QCOMPARE(
            widget
                ->d_pointer
                ->m_sections
                .at(widget->d_pointer->m_currentIndex)
                .value,
            value
        );
        QCOMPARE(
            widget
                ->d_pointer
                ->m_sections
                .at(widget->d_pointer->m_currentIndex)
                .maximum,
            maximum
        );
        QCOMPARE(
            flags.testFlag(QAbstractSpinBox::StepUpEnabled),
            StepUpEnabled
        );
        QCOMPARE(
            flags.testFlag(QAbstractSpinBox::StepDownEnabled),
            StepDownEnabled
        );
    }
    
    void testConfiguration() {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBox::SectionData> config;
        MultiSpinBox::SectionData section;
        section.minimum = 1;
        section.value = 8;
        section.maximum = 9;
        section.prefix = QStringLiteral(u"abc");
        section.suffix = QStringLiteral(u"def");
        config.append(section);
        myMulti.setSections (config);
        QCOMPARE(myMulti.sections().count(), 1);
        QCOMPARE(myMulti.sections().at(0).minimum, 1);
        QCOMPARE(myMulti.sections().at(0).value, 8);
        QCOMPARE(myMulti.sections().at(0).maximum, 9);
        QCOMPARE(
            myMulti.sections().at(0).prefix,
            QStringLiteral(u"abc")
        );
        QCOMPARE(
            myMulti.sections().at(0).suffix,
            QStringLiteral(u"def")
        );
    }

    void testFocusIntegrationForwardTab() {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 =
            new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSections (myConfiguration);
        QSpinBox *widget3 =
            new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(
            QStringLiteral(u"&Test"),
            parentWidget.data()
        );
        label2->setBuddy(widget2);
        widget1->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget.data());
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
        if (widget2->d_pointer->m_sections.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing
        
        // Apparently it isn’t possible to call simply the key click
        // on the parent widget. This code fails sometimes:
        // QTest::keyClick(parentWidget, Qt::Key::Key_Tab);
        // Therefore, we call QTest::keyClick() on
        // QApplication::focusWidget()
        
        // Move focus from widget1 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 2);
        // Move focus from widget2/section2 to widget3
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
    }

    void testFocusIntegrationBackwardTab() {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 =
            new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSections (myConfiguration);
        QSpinBox *widget3 =
            new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(
            QStringLiteral(u"&Test"),
            parentWidget.data()
        );
        label2->setBuddy(widget2);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget.data());
        // Assert that the setup is okay.
        if (widget1->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (!widget3->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (QApplication::focusWidget() != widget3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->d_pointer->m_sections.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing
        // Move focus from widget3 to widget2/section2
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 2);
        // Move focus from widget2/section2 to widget2/section1
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget2/section0
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget1
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_Tab,
            Qt::KeyboardModifier::ShiftModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget1);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
    }

    void testFocusIntegrationOther() {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 =
            new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSections (myConfiguration);
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QSpinBox *widget3 =
            new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(
            QStringLiteral(u"&Test"),
            parentWidget.data()
        );
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(
            QStringLiteral(u"&Other widget"),
            parentWidget.data()
        );
        label3->setBuddy(widget3);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget.data());
        // Assert that the setup is okay.
        if (widget1->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (!widget3->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (QApplication::focusWidget() != widget3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->d_pointer->m_sections.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->d_pointer->m_currentIndex != 1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing
        // Move focus from widget3 to widget2/section0
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_T,
            Qt::KeyboardModifier::AltModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget3
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_O,
            Qt::KeyboardModifier::AltModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget3);
        // Move focus from widget3 to widget2/section0
        // This has to move to section0 (even if before this event, the last
        // selected section of widget2 was NOT section0.
        QTest::keyClick(
            QApplication::focusWidget(),
            Qt::Key::Key_T,
            Qt::KeyboardModifier::AltModifier
        );
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
    }

    void testFocusIntegrationFocusPolicy() {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 =
            new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSections (myConfiguration);
        QSpinBox *widget3 =
            new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(
            QStringLiteral(u"&Test"),
            parentWidget.data()
        );
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(
            QStringLiteral(u"&Other widget"),
            parentWidget.data()
        );
        label3->setBuddy(widget3);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget.data());
        // Assert that the setup is okay.
        if (widget1->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (!widget3->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (QApplication::focusWidget() != widget3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->d_pointer->m_sections.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing        
        // Make sure that MultiSpinBox does not react on incoming tab focus
        // events if the current focus policy does not allow tab focus.
        widget2->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        widget1->setFocus();
        if (QApplication::focusWidget() != widget1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        widget2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        widget1->setFocus();
        if (QApplication::focusWidget() != widget1) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
    }

    void testStepBy() {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        widget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget->setSections (myConfiguration);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(0);
        widget->stepBy(13);
        QCOMPARE(
            widget->sections().at(0).value,
            13
        );
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(1);
        widget->stepBy(130);
        QCOMPARE(
            widget->sections().at(1).value,
            100
        );
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(2);
        widget->stepBy(-260);
        QCOMPARE(
            widget->sections().at(2).value,
            0
        );
    }

    void testUpdateValueFromText1() {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        widget->setSections (myConfiguration);
        const quint8 sampleSectionNumber = 1;
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(
            sampleSectionNumber
        );
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  0%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        widget->d_pointer->updateCurrentValueFromText(
            QStringLiteral(u"0°  9%  0")
        );
        QCOMPARE(
            widget->sections().at(sampleSectionNumber).value,
            9
        );
    }

    void testUpdateValueFromText2() {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        QList<MultiSpinBox::SectionData> specialConfiguration =
            myConfiguration;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        specialConfiguration[sampleSectionNumber].value = sampleValue;
        widget->setSections (specialConfiguration);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(
            sampleSectionNumber
        );
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sections().at(sampleSectionNumber).value
            != sampleValue
        ) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Execute the tested function (with an invalid argument)
        widget->d_pointer->updateCurrentValueFromText(
            QStringLiteral(u"abcdef")
        );
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
        // The original value should not have changed.
        QCOMPARE(
            widget->sections().at(sampleSectionNumber).value,
            sampleValue
        );
    }

    void testUpdateSectionFromCursorPosition() {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        QList<MultiSpinBox::SectionData> specialConfiguration =
            myConfiguration;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        specialConfiguration[sampleSectionNumber].value = sampleValue;
        widget->setSections (specialConfiguration);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(
            sampleSectionNumber
        );
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sections().at(sampleSectionNumber).value
            != sampleValue
        ) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        
        // Do testing
        widget->lineEdit()->setCursorPosition(0);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(1);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(2);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(4);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(5);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(6);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(8);
        QCOMPARE(widget->d_pointer->m_currentIndex, 2);
        widget->lineEdit()->setCursorPosition(9);
        QCOMPARE(widget->d_pointer->m_currentIndex, 2);
    }

    void testInitialLineEditValue() {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        QList<MultiSpinBox::SectionData> specialConfiguration =
            myConfiguration;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        specialConfiguration[sampleSectionNumber].value = sampleValue;
        widget->setSections (specialConfiguration);
        // Assert that the initial content of the line edit is okay
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"0°  5%  0")
        );
    }

    void testLocalizationAndInternationalization() {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        QList<MultiSpinBox::SectionData> mySectionList;
        MultiSpinBox::SectionData mySection;
        mySection.decimals = 1;
        mySection.minimum = 0;
        mySection.maximum = 100;
        mySection.value = 50;
        mySectionList.append(mySection);
        widget->setSections(mySectionList);
        widget->show();
        
        // Begin testing

        widget->setLocale(QLocale::English);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        // TODO Maybe processing QEvent::LocaleChange might help? If so,
        // wouldn't it be mandatory to also process QEvent::LanguageChange
        // and further other events?
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"50.0")
        );
        
        widget->setLocale(QLocale::German);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"50,0")
        );
        
        widget->setLocale(QLocale::Bengali);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"৫০.০")
        );
    }

    void testArrowKeys() {
        QScopedPointer<QWidget> parentWidget(new QWidget());
        PerceptualColor::MultiSpinBox *widget2 =
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSections (myConfiguration);
        widget2->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        // The following statement make focus and widget events working.
        QApplication::setActiveWindow(parentWidget.data());
        // Assert that the setup is okay.
        if (!widget2->hasFocus()) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (QApplication::focusWidget() != widget2) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->d_pointer->m_sections.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget2->lineEdit()->text() != QStringLiteral(u"0°  0%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Up);
        QCOMPARE(
            widget2->sections().at(1).value,
            1
        );
        QCOMPARE(
            widget2->lineEdit()->text(),
            QStringLiteral(u"0°  1%  0")
        );
    }

    void testSectionDataDebug() {
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);

        // Test if QDebug support does not make a crash.
        qDebug() << MultiSpinBox::SectionData();
        
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testFixedSection_data() {
        QTest::addColumn<double>("value");
        QTest::addColumn<double>("expectedOnIsWrappigFalse");
        QTest::addColumn<double>("expectedOnIsWrappigTrue");

        QTest::newRow(" -5") <<  -5. <<   0. << 355.;
        QTest::newRow("  0") <<   0. <<   0. <<   0.;
        QTest::newRow("  5") <<   5. <<   5. <<   5.;
        QTest::newRow("355") << 355. << 355. << 355.;
        QTest::newRow("360") << 360. << 360. <<   0.;
        QTest::newRow("365") << 365. << 360. <<   5.;
        QTest::newRow("715") << 715. << 360. << 355.;
        QTest::newRow("720") << 720. << 360. <<   0.;
        QTest::newRow("725") << 725. << 360. <<   5.;
    }

    void testFixedSection() {
        MultiSpinBox::SectionData mySection;
        mySection.minimum = 0;
        mySection.maximum = 360;
        
        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);
        
        mySection.value = value;
        mySection.isWrapping = false;
        QCOMPARE(
            MultiSpinBox::MultiSpinBoxPrivate::fixedSection(mySection).value,
            expectedOnIsWrappigFalse
        );
        mySection.isWrapping = true;
        QCOMPARE(
            MultiSpinBox::MultiSpinBoxPrivate::fixedSection(mySection).value,
            expectedOnIsWrappigTrue
        );
    }

    void testFixedSectionOther_data() {
        QTest::addColumn<double>("value");
        QTest::addColumn<double>("expectedOnIsWrappigFalse");
        QTest::addColumn<double>("expectedOnIsWrappigTrue");

        QTest::newRow("-25") << -25. << -20. << 335.;
        QTest::newRow("-20") << -20. << -20. << -20.;
        QTest::newRow("-15") << -15. << -15. << -15.;
        QTest::newRow("335") << 335. << 335. << 335.;
        QTest::newRow("340") << 340. << 340. << -20.;
        QTest::newRow("345") << 345. << 340. << -15.;
        QTest::newRow("695") << 695. << 340. << 335.;
        QTest::newRow("700") << 700. << 340. << -20.;
        QTest::newRow("705") << 705. << 340. << -15.;
    }

    void testFixedSectionOther() {
        MultiSpinBox::SectionData mySection;
        mySection.minimum = -20;
        mySection.maximum = 340;
        
        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);
        
        mySection.value = value;
        mySection.isWrapping = false;
        QCOMPARE(
            MultiSpinBox::MultiSpinBoxPrivate::fixedSection(mySection).value,
            expectedOnIsWrappigFalse
        );
        mySection.isWrapping = true;
        QCOMPARE(
            MultiSpinBox::MultiSpinBoxPrivate::fixedSection(mySection).value,
            expectedOnIsWrappigTrue
        );
    }

    void testReturnKey() {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(
            new PerceptualColor::MultiSpinBox()
        );
        widget->show();
        widget->setSections (myConfiguration);
        // Assert that the setup is okay.
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"0°  0%  0")
        );
        
        // Go to begin of the line edit
        QTest::keyClick(widget.data(), Qt::Key_Home);
        // Input
        QTest::keyClick(widget.data(), Qt::Key_3);
        QTest::keyClick(widget.data(), Qt::Key_7);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"370°  0%  0")
        );
        // Trigger the value fix
        QTest::keyClick(widget.data(), Qt::Key_Return);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"360°  0%  0")
        );
        
        // Go to end of the line edit
        QTest::keyClick(widget.data(), Qt::Key_End);
        // Input
        QTest::keyClick(widget.data(), Qt::Key_9);
        QTest::keyClick(widget.data(), Qt::Key_9);
        QTest::keyClick(widget.data(), Qt::Key_9);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"360°  0%  0999")
        );
        // Trigger the value fix
        QTest::keyClick(widget.data(), Qt::Key_Return);
        QCOMPARE(
            widget->lineEdit()->text(),
            QStringLiteral(u"360°  0%  255")
        );
    }

void testSnippet02() {
//! [MultiSpinBox Basic example]
PerceptualColor::MultiSpinBox *myHsvSpinBox =
    new PerceptualColor::MultiSpinBox();
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

myHsvSpinBox->setSections(hsvSections);
// Initial content is:  310,0°  200,0  100,0
//! [MultiSpinBox Basic example]
QCOMPARE(
    myHsvSpinBox->lineEdit()->text(),
    QStringLiteral("310,0°  200,0  100,0")
);
delete myHsvSpinBox;
}

private:
class testSnippet02 {
//! [MultiSpinBox Full-featured interface]
void addSection(MultiSpinBox::SectionData newSection);
void addSections(QList<MultiSpinBox::SectionData> newSections);
void append(MultiSpinBox::SectionData newSection);
void append(QList<MultiSpinBox::SectionData> newSections);
void clearSections();
int currentIndex() const;
MultiSpinBox::SectionData currentSection() const;
MultiSpinBox::SectionData firstSection() const;
void insertSection(int index, MultiSpinBox::SectionData newSection);
void insertSection(int index, QList<MultiSpinBox::SectionData> newSections);
MultiSpinBox::SectionData lastSection() const;
void moveSection(int from, int to);
void prependSection(MultiSpinBox::SectionData newSection);
void prependSections(QList<MultiSpinBox::SectionData> newSections);
void removeFirstSection();
void removeLastSection();
void removeSection(int index);
void replaceSection(int index, MultiSpinBox::SectionData newSection);
MultiSpinBox::SectionData sectionAt(int index) const;
int sectionCount() const;
QList<MultiSpinBox::SectionData> sections() const;
void setCurrentIndex(int newIndex);
void setSections(const QList<MultiSpinBox::SectionData> &newSections);
void swapSections(int i, int j);
//! [MultiSpinBox Full-featured interface]
};

};

}

QTEST_MAIN(PerceptualColor::TestMultiSpinBox)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinbox.moc"
