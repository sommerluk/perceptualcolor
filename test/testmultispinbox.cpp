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
#include "PerceptualColor/multispinbox.h"
// Second, the private implementation.
#include "multispinbox_p.h"

#include <QtTest>

#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

static void snippet02()
{
    //! [MultiSpinBox Basic example]
    PerceptualColor::MultiSpinBox *myHsvSpinBox = new PerceptualColor::MultiSpinBox();
    PerceptualColor::MultiSpinBoxSectionConfiguration myConfiguration;
    QList<PerceptualColor::MultiSpinBoxSectionConfiguration> hsvConfigurations;
    QList<double> hsvValues;

    myConfiguration.setDecimals(1);

    myConfiguration.setPrefix(QString());
    myConfiguration.setMinimum(0);
    myConfiguration.setWrapping(true);
    myConfiguration.setMaximum(360);
    myConfiguration.setSuffix(QStringLiteral(u"° "));
    hsvConfigurations.append(myConfiguration);
    hsvValues.append(310);

    myConfiguration.setPrefix(QStringLiteral(u" "));
    myConfiguration.setMinimum(0);
    myConfiguration.setMaximum(255);
    myConfiguration.setWrapping(false);
    myConfiguration.setSuffix(QStringLiteral(u" "));
    hsvConfigurations.append(myConfiguration);
    hsvValues.append(310);

    myConfiguration.setSuffix(QString());
    hsvConfigurations.append(myConfiguration);
    hsvValues.append(100);

    myHsvSpinBox->setSectionConfigurations(hsvConfigurations);
    myHsvSpinBox->setSectionValues(hsvValues);
    // Initial content is:  310,0°  200,0  100,0
    //! [MultiSpinBox Basic example]
    delete myHsvSpinBox;
}

class testSnippet02 : public PerceptualColor::MultiSpinBox
{
    Q_OBJECT

    //! [MultiSpinBox Full-featured interface]
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int sectionCount READ sectionCount NOTIFY sectionCountChanged)

public:
    void addSection(PerceptualColor::MultiSpinBoxSectionConfiguration newSection);
    void addSections(QList<PerceptualColor::MultiSpinBoxSectionConfiguration> newSections);
    void append(PerceptualColor::MultiSpinBoxSectionConfiguration newSection);
    void append(QList<PerceptualColor::MultiSpinBoxSectionConfiguration> newSections);
    QString cleanText(int index) const; // See also “cleanText”
    void clearSections();
    int currentIndex() const;
    PerceptualColor::MultiSpinBoxSectionConfiguration currentSection() const;
    PerceptualColor::MultiSpinBoxSectionConfiguration firstSection() const;
    void insertSection(int index, PerceptualColor::MultiSpinBoxSectionConfiguration newSection);
    void insertSection(int index, QList<PerceptualColor::MultiSpinBoxSectionConfiguration> newSections);
    PerceptualColor::MultiSpinBoxSectionConfiguration lastSection() const;
    void moveSection(int from, int to);
    void prependSection(PerceptualColor::MultiSpinBoxSectionConfiguration newSection);
    void prependSections(QList<PerceptualColor::MultiSpinBoxSectionConfiguration> newSections);
    void removeFirstSection();
    void removeLastSection();
    void removeSection(int index);
    void replaceSection(int index, PerceptualColor::MultiSpinBoxSectionConfiguration newSection);
    PerceptualColor::MultiSpinBoxSectionConfiguration sectionAt(int index) const;
    int sectionCount() const; // Somewhat redundant with MultiSpinBox::sections().count()
    QList<PerceptualColor::MultiSpinBoxSectionConfiguration> sectionConfigurations() const;
    QString sectionText(int index) const; // See also “cleanText”
    void setSelectedSection(int index);   // A better name might be “selectSection”
    void setSectionConfigurations(const QList<PerceptualColor::MultiSpinBoxSectionConfiguration> &newSections);
    void swapSections(int i, int j);

    // What about these functions? They…
    // …are public in QDoubleSpinBox
    // …are protected in QSpinBox
    // …do not exist in QDateTimeEdit
    // …do not exist in QAbstractSpinBox:
    QString textFromValue(double value) const;
    double valueFromText(const QString &text) const;

Q_SIGNALS:
    void currentIndexChanged(int newCurrentIndex);
    void sectionCountChanged(int newSectionCount);

    // The following signal in Qt 5.15 seems to not alwyas be emitted
    // when actually text changes, but instead only if actually the value
    // changes. So if the text changes from  “0.1” to “0.10” this signal
    // is not emitted because the value itself did not change. This is
    // couter-intuitive because the behaviour does not correspond to the
    // name of the signal. It’s therefore better not no implement this
    // signal.
    void textChanged(const QString &newText);

    // The following signal is emitted always when the value changes.
    // If we separate the section configuration from the section value,
    // we could provide the new value of tpye QList<double> as an
    // argument of this signal and also declare a property corresponding
    // to this signal.
    void valueChanged();

public Q_SLOTS:
    void setCurrentIndex(int newIndex);
    //! [MultiSpinBox Full-featured interface]
};
int testSnippet02::currentIndex() const
{
    return 0;
}
void testSnippet02::setCurrentIndex(int newIndex)
{
    Q_UNUSED(newIndex)
}
int testSnippet02::sectionCount() const
{
    return 0;
}

namespace PerceptualColor
{
class TestMultiSpinBox : public QObject
{
    Q_OBJECT

public:
    TestMultiSpinBox(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QList<MultiSpinBoxSectionConfiguration> exampleConfigurations;
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executed
        MultiSpinBoxSectionConfiguration mySection;
        mySection.setDecimals(0);
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setPrefix(QLatin1String());
        mySection.setSuffix(QStringLiteral(u"°"));
        exampleConfigurations.append(mySection);
        mySection.setMaximum(100);
        mySection.setPrefix(QStringLiteral(u"  "));
        mySection.setSuffix(QStringLiteral(u"%"));
        exampleConfigurations.append(mySection);
        mySection.setMaximum(255);
        mySection.setPrefix(QStringLiteral(u"  "));
        mySection.setSuffix(QLatin1String());
        exampleConfigurations.append(mySection);
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

    void testDefaultValues()
    {
        // The default values should be the same as for QDoubleSpinBox
        MultiSpinBox myMulti;
        QDoubleSpinBox myDoubleSpinBox;

        // Test default section values
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test default values of the configuration directly in the widget
        QCOMPARE(myMulti.sectionConfigurations().at(0).decimals(), myDoubleSpinBox.decimals());
        QCOMPARE(myMulti.sectionConfigurations().at(0).isWrapping(), myDoubleSpinBox.wrapping());
        QCOMPARE(myMulti.sectionConfigurations().at(0).maximum(), myDoubleSpinBox.maximum());
        QCOMPARE(myMulti.sectionConfigurations().at(0).minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myMulti.sectionConfigurations().at(0).prefix(), myDoubleSpinBox.prefix());
        QCOMPARE(myMulti.sectionConfigurations().at(0).singleStep(), myDoubleSpinBox.singleStep());
        QCOMPARE(myMulti.sectionConfigurations().at(0).suffix(), myDoubleSpinBox.suffix());

        // Whitebox tests
        QCOMPARE(myMulti.sectionValues(), QList<double> {0});
        QCOMPARE(myMulti.d_pointer->m_sectionValues, QList<double> {0});
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);
    }

    void testConstructor()
    {
        // Test the the constructor does not crash
        PerceptualColor::MultiSpinBox myMulti;
        // Test basic constructor results
        QVERIFY2(myMulti.d_pointer->m_sectionConfigurations.length() > 0, "Make sure the default configuration has at least 1 section.");
    }

    void testInteraction()
    {
        // This test is suprisingly slow.
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        widget->show();
        widget->setSectionConfigurations(exampleConfigurations);
        // Assert that the setup is okay.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  0%  0"));
        // Go to begin of the line edit
        QTest::keyClick(widget.data(), Qt::Key_Home);
        // Select the first “0”:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        // Write “45”
        QTest::keyClicks(widget.data(), QStringLiteral(u"45"));
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        // Select “45”
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        // Copy to clipboard
        QTest::keyClick(widget.data(), Qt::Key_C, Qt::ControlModifier, 0);
        // Go to second section
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        // Select second section:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        QCOMPARE(widget->lineEdit()->selectedText(), QStringLiteral(u"0"));
        // Take “45” from clipboard
        QTest::keyClick(widget.data(), Qt::Key_V, Qt::ControlModifier, 0);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
    }

    void testCurrentSectionIndex()
    {
        MultiSpinBox test;
        // Test default index
        QCOMPARE(test.d_pointer->m_currentIndex, 0);

        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if setting negative value is ignored
        QVERIFY_EXCEPTION_THROWN(test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-1), int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        QVERIFY_EXCEPTION_THROWN(test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-100), int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // Test setting too high values is ignored
        QVERIFY_EXCEPTION_THROWN(test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(100), int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);

        // Test if correct sections are stored correctly
        QList<MultiSpinBoxSectionConfiguration> mySectionList;
        mySectionList.append(MultiSpinBoxSectionConfiguration());
        mySectionList.append(MultiSpinBoxSectionConfiguration());
        mySectionList.append(MultiSpinBoxSectionConfiguration());
        test.setSectionConfigurations(mySectionList);
        test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(test.d_pointer->m_currentIndex, 2);
    }

    void testSetConfiguration()
    {
        // Correct configurations should be applied as-is.
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        MultiSpinBox test;
        QCOMPARE(test.sectionConfigurations().count(), 1);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        test.setSectionConfigurations(myConfigurations);
        QCOMPARE(test.sectionConfigurations().count(), 3);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);

        // Empty configurations shall be ignored
        test.setSectionConfigurations(QList<MultiSpinBoxSectionConfiguration>());
        QCOMPARE(test.sectionConfigurations().count(), 3);

        // Invalid values should be adapted
        myConfigurations.clear();
        MultiSpinBoxSectionConfiguration myInvalidSection;
        myInvalidSection.setMinimum(50);
        myInvalidSection.setMaximum(30);
        myConfigurations.append(myInvalidSection);
        test.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.clear();
        myValues.append(40);
        test.setSectionValues(myValues);
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "minimum <= maximum");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() <= test.d_pointer->m_sectionValues.at(0),
            // comment
            "minimum <= value");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionValues.at(0) <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "value <= maximum");

        // Invalid values should be adapted
        myConfigurations.clear();
        myInvalidSection.setMinimum(-50);
        myInvalidSection.setMaximum(-70);
        myConfigurations.append(myInvalidSection);
        myValues.clear();
        myValues.append(-60);
        test.setSectionConfigurations(myConfigurations);
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "minimum <= maximum");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() <= test.d_pointer->m_sectionValues.at(0),
            // comment
            "minimum <= value");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionValues.at(0) <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "value <= maximum");
    }

    void testMinimalSizeHint()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
        myMulti.setSectionConfigurations(exampleConfigurations);
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
    }

    void testSizeHint()
    {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBoxSectionConfiguration> config;
        MultiSpinBoxSectionConfiguration section;
        section.setMinimum(1);
        section.setMaximum(9);
        section.setPrefix(QStringLiteral(u"abcdefghij"));
        section.setSuffix(QStringLiteral(u"abcdefghij"));
        config.append(section);
        myMulti.setSectionConfigurations(config);
        const int referenceWidth = myMulti.sizeHint().width();

        // Now test various configurations that should lead to bigger sizes…

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setPrefix(QStringLiteral(u"abcdefghij"));
        section.setSuffix(QStringLiteral(u"abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(1);
        section.setMaximum(19);
        section.setPrefix(QStringLiteral(u"abcdefghij"));
        section.setSuffix(QStringLiteral(u"abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setPrefix(QStringLiteral(u"abcdefghijh"));
        section.setSuffix(QStringLiteral(u"abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setPrefix(QStringLiteral(u"abcdefghij"));
        section.setSuffix(QStringLiteral(u"abcdefghijh"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);
    }

    void testUpdatePrefixValueSuffixText()
    {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        MultiSpinBoxSectionConfiguration myConfiguration;
        QList<double> myValues;

        myConfiguration.setDecimals(0);
        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setPrefix(QStringLiteral(u"abc"));
        myConfiguration.setSuffix(QStringLiteral(u"def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setPrefix(QStringLiteral(u"ghi"));
        myConfiguration.setSuffix(QStringLiteral(u"jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);
        myMulti.d_pointer->m_currentIndex = 1;
        myMulti.d_pointer->updatePrefixValueSuffixText();
        QCOMPARE(myMulti.d_pointer->m_textBeforeCurrentValue, QStringLiteral(u"abc8defghi"));
        QCOMPARE(myMulti.d_pointer->m_textOfCurrentValue, QStringLiteral(u"80"));
        QCOMPARE(myMulti.d_pointer->m_textAfterCurrentValue, QStringLiteral(u"jkl"));
    }

    void testSetCurrentSectionIndexWithoutSelectingText()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        MultiSpinBoxSectionConfiguration myConfiguration;
        QList<double> myValues;

        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setPrefix(QStringLiteral(u"abc"));
        myConfiguration.setSuffix(QStringLiteral(u"def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setPrefix(QStringLiteral(u"ghi"));
        myConfiguration.setSuffix(QStringLiteral(u"jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        QVERIFY2(!myMulti.lineEdit()->hasSelectedText(), "No text should be selected.");
    }

    void testSetCurrentSectionIndex()
    {
        PerceptualColor::MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(exampleConfigurations);
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

        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        MultiSpinBoxSectionConfiguration myConfiguration;
        QList<double> myValues;

        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setPrefix(QStringLiteral(u"abc"));
        myConfiguration.setSuffix(QStringLiteral(u"def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setPrefix(QStringLiteral(u"ghi"));
        myConfiguration.setSuffix(QStringLiteral(u"jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);

        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        QVERIFY2(!myMulti.lineEdit()->hasSelectedText(),
                 "No text should be selected because invisible widgets "
                 "have no focus.");
    }

    void testStepEnabledSimple()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        MultiSpinBoxSectionConfiguration myConfiguration;
        QList<double> myValues;

        myConfigurations.clear();
        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setPrefix(QStringLiteral(u"abc"));
        myConfiguration.setSuffix(QStringLiteral(u"def"));
        myConfigurations.append(myConfiguration);
        myMulti.setSectionConfigurations(myConfigurations);
        myValues.clear();
        myValues.append(8);
        myMulti.setSectionValues(myValues);
        QAbstractSpinBox::StepEnabled flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be enabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be enabled");

        myValues.clear();
        myValues.append(9);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be disabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be enabled");

        myValues.clear();
        myValues.append(10);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be disabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be enabled");

        myValues.clear();
        myValues.append(1);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be disabled");

        myValues.clear();
        myValues.append(0);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be disabled");

        myValues.clear();
        myValues.append(-1);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), "Step down should be disabled");
    }

    void testStepEnabledAndSectionIndex_data()
    {
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
        QTest::newRow("4") << 4 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("5") << 5 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("6") << 6 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("8") << 8 << 2 << 0 << 0 << 255 << true << false;
        QTest::newRow("9") << 9 << 2 << 0 << 0 << 255 << true << false;
    }

    void testStepEnabledAndSectionIndex()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSectionConfiguration> specialConfigurations = exampleConfigurations;
        QList<double> myValues;
        while (myValues.count() < specialConfigurations.count()) {
            myValues.append(0);
        }
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfigurations);
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(sampleSectionNumber);

        // Assertions: Assert that the setup is okay.
        // Assert statements seem to be not always reliably within QTest.
        // Therefore we do some assertions here with QCOMPARE.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  5%  0"));
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), sampleValue);
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
        QCOMPARE(widget->d_pointer->m_currentIndex, sectionIndex);
        QCOMPARE(
            //
            widget->d_pointer->m_sectionConfigurations.at(widget->d_pointer->m_currentIndex).minimum(), //
            minimum);
        QCOMPARE(
            //
            widget->d_pointer->m_sectionValues.at(widget->d_pointer->m_currentIndex), //
            value);
        QCOMPARE(
            //
            widget->d_pointer->m_sectionConfigurations.at(widget->d_pointer->m_currentIndex).maximum(), //
            maximum);
        QCOMPARE(flags.testFlag(QAbstractSpinBox::StepUpEnabled), StepUpEnabled);
        QCOMPARE(flags.testFlag(QAbstractSpinBox::StepDownEnabled), StepDownEnabled);
    }

    void testConfiguration()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSectionConfiguration> config;
        MultiSpinBoxSectionConfiguration section;
        section.setMinimum(1);
        section.setMaximum(9);
        section.setPrefix(QStringLiteral(u"abc"));
        section.setSuffix(QStringLiteral(u"def"));
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QCOMPARE(myMulti.sectionConfigurations().count(), 1);
        QCOMPARE(myMulti.sectionConfigurations().at(0).minimum(), 1);
        QCOMPARE(myMulti.sectionConfigurations().at(0).maximum(), 9);
        QCOMPARE(myMulti.sectionConfigurations().at(0).prefix(), QStringLiteral(u"abc"));
        QCOMPARE(myMulti.sectionConfigurations().at(0).suffix(), QStringLiteral(u"def"));
    }

    void testFocusIntegrationForwardTab()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), parentWidget.data());
        label2->setBuddy(widget2);
        widget1->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests:
        parentWidget->show();
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
        if (widget2->d_pointer->m_sectionConfigurations.count() != 3) {
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

    void testFocusIntegrationBackwardTab()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), parentWidget.data());
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
        if (widget2->d_pointer->m_sectionConfigurations.count() != 3) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Start actual testing
        // Move focus from widget3 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 2);
        // Move focus from widget2/section2 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget1);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
    }

    void testFocusIntegrationOther()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), parentWidget.data());
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(QStringLiteral(u"&Other widget"), parentWidget.data());
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
        if (widget2->d_pointer->m_sectionConfigurations.count() != 3) {
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
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_T, Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget3
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_O, Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget3);
        // Move focus from widget3 to widget2/section0
        // This has to move to section0 (even if before this event, the last
        // selected section of widget2 was NOT section0.
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_T, Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
    }

    void testFocusIntegrationFocusPolicy()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), parentWidget.data());
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(QStringLiteral(u"&Other widget"), parentWidget.data());
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
        if (widget2->d_pointer->m_sectionConfigurations.count() != 3) {
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

    void testStepBy()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        widget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget->setSectionConfigurations(exampleConfigurations);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(0);
        widget->stepBy(13);
        QCOMPARE(widget->sectionValues().at(0), 13);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(1);
        widget->stepBy(130);
        QCOMPARE(widget->sectionValues().at(1), 100);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(2);
        widget->stepBy(-260);
        QCOMPARE(widget->sectionValues().at(2), 0);
    }

    void testStepUpDown()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        QCOMPARE(widget->sectionValues().at(0), 0);
        widget->stepUp();
        QCOMPARE(widget->sectionValues().at(0), 1);
        widget->stepUp();
        QCOMPARE(widget->sectionValues().at(0), 2);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 1);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 0);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 0);
    }

    void testUpdateValueFromText1()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        const quint8 sampleSectionNumber = 1;
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  0%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        widget->d_pointer->updateCurrentValueFromText(QStringLiteral(u"0°  9%  0"));
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), 9);
    }

    void testUpdateValueFromText2()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSectionConfiguration> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.count() < specialConfiguration.count()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sectionValues().at(sampleSectionNumber) != sampleValue) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Execute the tested function (with an invalid argument)
        widget->d_pointer->updateCurrentValueFromText(QStringLiteral(u"abcdef"));
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
        // The original value should not have changed.
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), sampleValue);
    }

    void testUpdateSectionFromCursorPosition()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSectionConfiguration> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.count() < specialConfiguration.count()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sectionValues().at(sampleSectionNumber) != sampleValue) {
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

    void testInitialLineEditValue()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSectionConfiguration> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.count() < specialConfiguration.count()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        // Assert that the initial content of the line edit is okay
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  5%  0"));
    }

    void testLocalizationAndInternationalization()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget(new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSectionConfiguration> mySectionList;
        MultiSpinBoxSectionConfiguration mySection;
        mySection.setDecimals(1);
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySectionList.append(mySection);
        widget->setSectionConfigurations(mySectionList);
        QList<double> myValues;
        myValues.append(50);
        widget->setSectionValues(myValues);

        // Begin testing

        widget->setLocale(QLocale::English);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"50.0"));

        widget->setLocale(QLocale::German);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"50,0"));

        widget->setLocale(QLocale::Bengali);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"৫০.০"));
    }

    void testArrowKeys()
    {
        QScopedPointer<QWidget> parentWidget(new QWidget());
        PerceptualColor::MultiSpinBox *widget2 = new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
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
        if (widget2->d_pointer->m_sectionConfigurations.count() != 3) {
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
        QCOMPARE(widget2->sectionValues().at(1), 1);
        QCOMPARE(widget2->lineEdit()->text(), QStringLiteral(u"0°  1%  0"));
    }

    void testSectionConfigurationDebug()
    {
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if QDebug support does not make a crash.
        qDebug() << MultiSpinBoxSectionConfiguration();
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testAddActionButton()
    {
        MultiSpinBox mySpinBox;
        int oldWidth = 0;
        QCOMPARE(mySpinBox.d_pointer->m_actionButtonCount, 0);
        oldWidth = mySpinBox.sizeHint().width();
        mySpinBox.addActionButton(new QAction(QStringLiteral(u"test"), &mySpinBox), QLineEdit::ActionPosition::TrailingPosition);
        QCOMPARE(mySpinBox.d_pointer->m_actionButtonCount, 1);
        QVERIFY2(mySpinBox.sizeHint().width() > oldWidth,
                 "Verify: After adding an action button, "
                 "the size hint has a bigger width than before.");
        oldWidth = mySpinBox.sizeHint().width();
        mySpinBox.addActionButton(new QAction(QStringLiteral(u"test"), &mySpinBox), QLineEdit::ActionPosition::TrailingPosition);
        QCOMPARE(mySpinBox.d_pointer->m_actionButtonCount, 2);
        QVERIFY2(mySpinBox.sizeHint().width() > oldWidth,
                 "Verify: After adding an action button, "
                 "the size hint has a bigger width than before.");
    }

    void testFixSectionValue_data()
    {
        QTest::addColumn<double>("value");
        QTest::addColumn<double>("expectedOnIsWrappigFalse");
        QTest::addColumn<double>("expectedOnIsWrappigTrue");

        QTest::newRow(" -5") << -05. << 000. << 355.;
        QTest::newRow("  0") << 000. << 000. << 000.;
        QTest::newRow("  5") << 005. << 005. << 005.;
        QTest::newRow("355") << 355. << 355. << 355.;
        QTest::newRow("360") << 360. << 360. << 000.;
        QTest::newRow("365") << 365. << 360. << 005.;
        QTest::newRow("715") << 715. << 360. << 355.;
        QTest::newRow("720") << 720. << 360. << 000.;
        QTest::newRow("725") << 725. << 360. << 005.;
    }

    void testFixSectionValue()
    {
        MultiSpinBox mySpinBox;

        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);

        MultiSpinBoxSectionConfiguration myConfiguration;
        myConfiguration.setMinimum(0);
        myConfiguration.setMaximum(360);
        myConfiguration.setWrapping(false);
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.append(value);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigFalse);

        myConfiguration.setWrapping(true);
        myConfigurations.clear();
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigTrue);
    }

    void testFixedSectionOther_data()
    {
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

    void testFixedSectionOther()
    {
        MultiSpinBox mySpinBox;

        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);

        MultiSpinBoxSectionConfiguration myConfiguration;
        myConfiguration.setMinimum(-20);
        myConfiguration.setMaximum(340);
        myConfiguration.setWrapping(false);
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.append(value);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigFalse);

        myConfiguration.setWrapping(true);
        myConfigurations.clear();
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigTrue);
    }

    void testValuesSetterAndConfigurationsSetter()
    {
        // Both, sectionValues() and sectionConfigurations() have a count()
        // that has to be identical. The count of sectionConfigurations() is
        // mandatory. Make sure that different setters let the count()s
        // in a correct state. Our reference for default values is
        // QDoubleSpinBox.
        MultiSpinBox myMulti;
        QDoubleSpinBox myDoubleSpinBox;
        QList<MultiSpinBoxSectionConfiguration> myConfigurations;
        QList<double> myValues;

        // Section count should be 1 (by default):
        QCOMPARE(myMulti.sectionConfigurations().count(), 1);
        QCOMPARE(myMulti.sectionValues().count(), 1);
        // Control that sections has default value:
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Raise the section count to 3:
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        myConfigurations.append(MultiSpinBoxSectionConfiguration());
        myMulti.setSectionConfigurations(myConfigurations);
        // Control that all the new sections got the default value:
        QCOMPARE(myMulti.sectionValues().at(1), myDoubleSpinBox.value());
        QCOMPARE(myMulti.sectionValues().at(2), myDoubleSpinBox.value());

        // Put specific values into each of the 3 sections:
        myValues.clear();
        myValues.append(10);
        myValues.append(11);
        myValues.append(12);
        myValues.append(13); // Too many values for current configuration count
        myMulti.setSectionValues(myValues);
        // Assert that the values have been applied correctly
        QCOMPARE(myMulti.sectionValues().at(0), 10);
        QCOMPARE(myMulti.sectionValues().at(1), 11);
        QCOMPARE(myMulti.sectionValues().at(2), 12);
        // The last value has to be ignored (as there are not so many sections):
        QCOMPARE(myMulti.sectionConfigurations().count(), 3);
        QCOMPARE(myMulti.sectionValues().count(), 3);

        // Apply a configuration with less sections
        myConfigurations.removeLast();
        QCOMPARE(myConfigurations.count(), 2); // Assertion
        myMulti.setSectionConfigurations(myConfigurations);
        QCOMPARE(myMulti.sectionConfigurations().count(), 2);
        QCOMPARE(myMulti.sectionValues().count(), 2);
        // The values that survive should not be changed:
        QCOMPARE(myMulti.sectionValues().at(0), 10);
        QCOMPARE(myMulti.sectionValues().at(1), 11);

        // Set sectionValues that has not enough values
        QCOMPARE(myMulti.sectionConfigurations().count(), 2); // Assertion
        QCOMPARE(myMulti.sectionValues().count(), 2);         // Assertion
        QCOMPARE(myMulti.sectionValues().at(0), 10);          // Assertion
        QCOMPARE(myMulti.sectionValues().at(1), 11);          // Assertion
        myValues.clear();
        myValues.append(20);
        // Apply a value list with only 1 value:
        myMulti.setSectionValues(myValues);
        QCOMPARE(myMulti.sectionValues().at(0), 20); // This values has been applied
        // Section count has not been altered:
        QCOMPARE(myMulti.sectionConfigurations().count(), 2);
        QCOMPARE(myMulti.sectionValues().count(), 2);
        // The last section, that got no paricular value assigned,
        // has been changed to the default value. (This behaviour
        // is not documented, so not part of the public API, but
        // is seems reasonable (and less confusing and more
        // predictable than just stay with the old value:
        QCOMPARE(myMulti.sectionValues().at(1), 0);
    }

    void testSectionValuesChangedSignalBasic()
    {
        // Initialize
        MultiSpinBox myMulti;
        MultiSpinBoxSectionConfiguration myConfig;
        QList<MultiSpinBoxSectionConfiguration> myConfigs;
        myConfigs.append(myConfig);
        myConfigs.append(myConfig);
        myMulti.setSectionConfigurations(myConfigs);
        myMulti.show();
        QSignalSpy spyMulti(&myMulti, //
                            &MultiSpinBox::sectionValuesChanged);
        QDoubleSpinBox myDouble;
        myDouble.show();
        QSignalSpy spyDouble(&myDouble,                                           //
                             QOverload<double>::of(&QDoubleSpinBox::valueChanged) //
        );

        // Set a value different from the default
        myMulti.setSectionValues(QList<double> {2, 2});
        myDouble.setValue(2);
        QCOMPARE(spyMulti.count(), 1);
        QCOMPARE(spyMulti.count(), spyDouble.count());

        // Setting the same value again should not call again the signal
        myMulti.setSectionValues(QList<double> {2, 2});
        myDouble.setValue(2);
        QCOMPARE(spyMulti.count(), 1);
        QCOMPARE(spyMulti.count(), spyDouble.count());

        // Setting a value list which has only one differet element triggers:
        myMulti.setSectionValues(QList<double> {2, 3});
        myDouble.setValue(3);
        QCOMPARE(spyMulti.count(), 2);
        QCOMPARE(spyMulti.count(), spyDouble.count());
    }

    void testSectionValuesChangedSignalKeyboardTracking()
    {
        // Initialize
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Use only one section to allow to compare easily
            // with QDoubleSpinBox
            QList<MultiSpinBoxSectionConfiguration> {MultiSpinBoxSectionConfiguration()});
        myMulti.show();
        QSignalSpy spyMulti(&myMulti, //
                            &MultiSpinBox::sectionValuesChanged);
        QDoubleSpinBox myDouble;
        myDouble.show();
        QSignalSpy spyDouble(&myDouble,                                           //
                             QOverload<double>::of(&QDoubleSpinBox::valueChanged) //
        );

        // Test with keyboard tracking
        myMulti.setKeyboardTracking(true);
        myDouble.setKeyboardTracking(true);

        // Get test data
        QApplication::setActiveWindow(&myMulti);
        QTest::keyClick(&myMulti, Qt::Key_Up); // Get text selection
        QTest::keyClick(&myMulti, Qt::Key::Key_5);
        QTest::keyClick(&myMulti, Qt::Key::Key_4);
        QCOMPARE(myMulti.sectionValues().at(0), 54); // Assertion

        // Get reference data
        QApplication::setActiveWindow(&myDouble);
        QTest::keyClick(&myDouble, Qt::Key_Up);
        QTest::keyClick(&myDouble, Qt::Key::Key_5);
        QTest::keyClick(&myDouble, Qt::Key::Key_4);
        QCOMPARE(myDouble.value(), 54); // Assertion

        // Test conformance of MultiSpinBox with QDoubleSpinBox’s behaviour
        QCOMPARE(spyMulti.count(), spyDouble.count());
        for (int i = 0; i < spyMulti.count(); ++i) {
            QCOMPARE(spyMulti                    // Get value of first section of MultiSpinBox…
                         .at(i)                  // Signal at position i
                         .at(0)                  // First argument of this signal
                         .value<QList<double>>() // Convert to original type
                         .at(0),                 // First section of the MultiSpinBox
                     spyDouble                   // Get value of QSpinBox…
                         .at(i)                  // Signal at position i
                         .at(0)                  // First argument of this signal
                         .toDouble()             // Convert to original type
            );
        }
    }

    void testRoundingBehaviourCompliance()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(5);
        myConfig.setMaximum(360);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSectionConfiguration> {myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(5);
        myDoubleSpinBox.setMaximum(360);

        myMulti.setSectionValues(QList<double> {-1});
        myDoubleSpinBox.setValue(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {0});
        myDoubleSpinBox.setValue(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down and stays too small
        myMulti.setSectionValues(QList<double> {4.1});
        myDoubleSpinBox.setValue(4.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that is too small, but rounds up to the minimum
        myMulti.setSectionValues(QList<double> {4.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(4.9);                 // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {5});
        myDoubleSpinBox.setValue(5);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down to the minimum
        myMulti.setSectionValues(QList<double> {5.1});
        myDoubleSpinBox.setValue(5.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value in the middle that rounds down
        myMulti.setSectionValues(QList<double> {72.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value in the middle that rounds up
        myMulti.setSectionValues(QList<double> {72.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that is in range and rounds down
        myMulti.setSectionValues(QList<double> {359.1});
        myDoubleSpinBox.setValue(359.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds up to the maximum
        myMulti.setSectionValues(QList<double> {359.9});
        myDoubleSpinBox.setValue(359.9);

        // Test with maximum
        myMulti.setSectionValues(QList<double> {360});
        myDoubleSpinBox.setValue(360);

        // Test with value that rounds down to maximum
        myMulti.setSectionValues(QList<double> {360.1});
        myDoubleSpinBox.setValue(360.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {361});
        myDoubleSpinBox.setValue(361);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingBehaviourComplianceWithRoundedRanges()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(4.8);
        myConfig.setMaximum(360.2);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSectionConfiguration> {myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(4.8);
        myDoubleSpinBox.setMaximum(360.2);

        myMulti.setSectionValues(QList<double> {-1});
        myDoubleSpinBox.setValue(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {0});
        myDoubleSpinBox.setValue(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down and stays too small
        myMulti.setSectionValues(QList<double> {4.1});
        myDoubleSpinBox.setValue(4.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {4.7});
        myDoubleSpinBox.setValue(4.7); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {4.8});
        myDoubleSpinBox.setValue(4.8); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {4.9});
        myDoubleSpinBox.setValue(4.9); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {5});
        myDoubleSpinBox.setValue(5);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {5.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(5.1);                 // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {72.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {72.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {359.1});
        myDoubleSpinBox.setValue(359.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {359.9});
        myDoubleSpinBox.setValue(359.9);

        myMulti.setSectionValues(QList<double> {360});
        myDoubleSpinBox.setValue(360);

        myMulti.setSectionValues(QList<double> {360.1});
        myDoubleSpinBox.setValue(360.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {360.2});
        myDoubleSpinBox.setValue(360.2);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {360.3});
        myDoubleSpinBox.setValue(360.3);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {360.9});
        myDoubleSpinBox.setValue(360.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {361});
        myDoubleSpinBox.setValue(361);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingBehaviourCornerCases()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(4.8);
        myConfig.setMaximum(359.8);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSectionConfiguration> {myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(4.8);
        myDoubleSpinBox.setMaximum(359.8);

        myMulti.setSectionValues(QList<double> {359});
        myDoubleSpinBox.setValue(359);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {359.7});
        myDoubleSpinBox.setValue(359.7);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {359.8});
        myDoubleSpinBox.setValue(359.8);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {359.9});
        myDoubleSpinBox.setValue(359.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double> {360});
        myDoubleSpinBox.setValue(360);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingAfterChangingDecimals()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        QList<MultiSpinBoxSectionConfiguration> myConfigs {// Initialize the list with one single, default section
                                                           MultiSpinBoxSectionConfiguration()};
        myConfigs[0].setDecimals(2);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(myConfigs);
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(2);
        const double initialTestValue = 12.34;
        myMulti.setSectionValues(QList<double> {initialTestValue});
        myDoubleSpinBox.setValue(initialTestValue);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(1);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(0);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(3);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(-1);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testMaximumWrappingRounding_data()
    {
        QTest::addColumn<double>("value");

        QTest::newRow("-360.1") << -360.1;
        QTest::newRow("-360") << -360.0;
        QTest::newRow("-359.9") << -359.9;
        QTest::newRow("-0.1") << -0.1;
        QTest::newRow("0") << 0.0;
        QTest::newRow("0.1") << 0.1;
        QTest::newRow("359.9") << 359.9;
        QTest::newRow("360") << 360.0;
        QTest::newRow("360.1") << 360.1;
        QTest::newRow("719.9") << 719.9;
        QTest::newRow("720") << 720.0;
        QTest::newRow("720.1") << 720.1;
    }

    void testMaximumWrappingRounding()
    {
        // When using wrapping, the MultiSpinBox is supposed to never
        // show “360”, but instead “0”. This should also be true when
        // rounding applies. And when being a magnitude higher or lower.

        // Get data
        QFETCH(double, value);

        // Initialization
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(0);
        myConfig.setMaximum(360);
        myConfig.setWrapping(true);
        MultiSpinBox mySpinBox;
        mySpinBox.setSectionConfigurations(
            // Create the QList on the fly…
            QList<MultiSpinBoxSectionConfiguration> {myConfig});

        mySpinBox.setSectionValues(
            // Create the QList on the fly…
            QList<double> {value});
        QCOMPARE(mySpinBox.text(), QStringLiteral("0"));

        mySpinBox.setSectionValues(
            // Create the QList on the fly…
            QList<double> {359.9});
        QCOMPARE(mySpinBox.text(), QStringLiteral("0"));
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(MultiSpinBoxSectionConfiguration());
    }

    void testMetaTypeDeclarationForPropertySectionValues()
    {
        // The data type QList<double> seems to be automatically declared
        // because it’s an instance of QList. This unit test controls this
        // assumption.
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(QList<double>());
    }

    void testSnippet02()
    {
        snippet02();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiSpinBox)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinbox.moc"
