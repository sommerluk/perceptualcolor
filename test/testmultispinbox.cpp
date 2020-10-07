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

namespace PerceptualColor {

class TestMultiSpinBox : public QObject
{

    Q_OBJECT

public:
    TestMultiSpinBox(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    MultiSpinBox::SectionList myConfiguration;

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
        // Test that constructor does not crash.
        MultiSpinBox test;
    }
    
    void testCurrentSectionIndex() {
        MultiSpinBox test;
        // Test default index
        QCOMPARE(test.currentSectionIndex(), 0);
        // Test if setting negative value is ignored
        test.setCurrentSectionIndex(-1);
        QCOMPARE(test.currentSectionIndex(), 0);
        test.setCurrentSectionIndex(-100);
        QCOMPARE(test.currentSectionIndex(), 0);
        // Test setting too high values is ignored
        test.setCurrentSectionIndex(100);
        QCOMPARE(test.currentSectionIndex(), 0);
        // Test if correct sections are stored correctly
        MultiSpinBox::SectionList mySectionList;
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        mySectionList.append(MultiSpinBox::Section());
        test.setConfiguration(mySectionList);
        test.setCurrentSectionIndex(2);
        QCOMPARE(test.currentSectionIndex(), 2);
    }
    
    void testSetConfiguration() {
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
    }
    
    void testSetCurrentSectionIndex() {
        MultiSpinBox myMulti;
        myMulti.setConfiguration(myConfiguration);
        myMulti.setCurrentSectionIndex(2);
        QCOMPARE(myMulti.currentSectionIndex(), 2);
        myMulti.setCurrentSectionIndex(0);
        QCOMPARE(myMulti.currentSectionIndex(), 0);
        myMulti.setCurrentSectionIndex(1);
        QCOMPARE(myMulti.currentSectionIndex(), 1);
        myMulti.setCurrentSectionIndex(2);
        QCOMPARE(myMulti.currentSectionIndex(), 2);
        myMulti.setCurrentSectionIndex(0);
        QCOMPARE(myMulti.currentSectionIndex(), 0);
    }

};

}

QTEST_MAIN(PerceptualColor::TestMultiSpinBox)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinbox.moc"
