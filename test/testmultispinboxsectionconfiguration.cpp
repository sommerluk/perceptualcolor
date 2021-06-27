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
#include "PerceptualColor/multispinboxsectionconfiguration.h"
// Second, the private implementation.
#include "multispinboxsectionconfiguration_p.h"

#include <QDoubleSpinBox>
#include <QtTest>

namespace PerceptualColor
{
class TestMultiSpinBoxSectionConfiguration : public QObject
{
    Q_OBJECT

public:
    TestMultiSpinBoxSectionConfiguration(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    MultiSpinBoxSectionConfiguration helperGetUnusualConfig()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(1);
        myConfig.setWrapping(true);
        myConfig.setMaximum(3);
        myConfig.setMinimum(2);
        myConfig.setPrefix(QStringLiteral("a"));
        myConfig.setSingleStep(4);
        myConfig.setSuffix(QStringLiteral("b"));
        return myConfig;
    }

private Q_SLOTS:
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

    void testDefaultValues()
    {
        // The default values should be the same as for QDoubleSpinBox
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Test default values of the configuration object:
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());
        QCOMPARE(myConfig.isWrapping(), myDoubleSpinBox.wrapping());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.prefix(), myDoubleSpinBox.prefix());
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());
        QCOMPARE(myConfig.suffix(), myDoubleSpinBox.suffix());
    }

    void testCopy()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(1);
        myConfig.setWrapping(true);
        myConfig.setMaximum(3);
        myConfig.setMinimum(2);
        myConfig.setPrefix(QStringLiteral("a"));
        myConfig.setSingleStep(4);
        myConfig.setSuffix(QStringLiteral("b"));

        // Test copy constructor
        MultiSpinBoxSectionConfiguration copyContructed(myConfig);
        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.singleStep(), 4);
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));

        // Test copy assignment operator
        MultiSpinBoxSectionConfiguration copyAssigned;
        copyAssigned.setMaximum(9); // Change arbitrary some value
        copyAssigned = myConfig;
        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.singleStep(), 4);
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
    }

    void testMoveConstructor()
    {
        MultiSpinBoxSectionConfiguration testObjectToMove;
        testObjectToMove.setDecimals(1);
        testObjectToMove.setWrapping(true);
        testObjectToMove.setMaximum(3);
        testObjectToMove.setMinimum(2);
        testObjectToMove.setPrefix(QStringLiteral("a"));
        testObjectToMove.setSingleStep(4);
        testObjectToMove.setSuffix(QStringLiteral("b"));
        MultiSpinBoxSectionConfiguration myConfig(
            // Trigger the move constructor
            std::move(testObjectToMove));
        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.singleStep(), 4);
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
    }

    void testMoveAssignment()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setMaximum(9);              // change arbitrarily some value
        myConfig = helperGetUnusualConfig(); // move assignment
        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.singleStep(), 4);
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
    }

    void testDecimals()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setDecimals(5);
        QCOMPARE(myConfig.decimals(), 5);
    }

    void testIsWrapping()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setWrapping(true);
        QCOMPARE(myConfig.isWrapping(), true);
        myConfig.setWrapping(false);
        QCOMPARE(myConfig.isWrapping(), false);
    }

    void testMaximum()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setMaximum(5);
        QCOMPARE(myConfig.maximum(), 5);
    }

    void testMinimum()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setMinimum(5);
        QCOMPARE(myConfig.minimum(), 5);
    }

    void testPrefix()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setPrefix(QStringLiteral("a"));
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
    }

    void testSingleStep()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setSingleStep(5);
        QCOMPARE(myConfig.singleStep(), 5);
    }

    void testSuffix()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        myConfig.setSuffix(QStringLiteral("a"));
        QCOMPARE(myConfig.suffix(), QStringLiteral("a"));
    }

    void testComplianceDecimals()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(50);
        myDoubleSpinBox.setDecimals(50);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(500);
        myDoubleSpinBox.setDecimals(500);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());
    }

    void testComplianceSingleStep()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        myConfig.setSingleStep(5);
        myDoubleSpinBox.setSingleStep(5);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(50);
        myDoubleSpinBox.setSingleStep(50);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(500);
        myDoubleSpinBox.setSingleStep(500);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(0);
        myDoubleSpinBox.setSingleStep(0);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(-1);
        myDoubleSpinBox.setSingleStep(-1);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());
    }

    void testComplianceMinimumTooBig()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a minimum that is bigger than the current maximum
        myConfig.setMaximum(50);
        myDoubleSpinBox.setMaximum(50);
        myConfig.setMinimum(60);
        myDoubleSpinBox.setMinimum(60);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Afterwards, change back the maximum to a value that would
        // have allowed to the original minimum
        myConfig.setMaximum(70);
        myDoubleSpinBox.setMaximum(70);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }

    void testComplianceMaximumTooSmall()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a maximum that is smaller than the current minimum
        myConfig.setMinimum(50);
        myDoubleSpinBox.setMinimum(50);
        myConfig.setMaximum(40);
        myDoubleSpinBox.setMaximum(40);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Afterwards, change back the minimum to a value that would
        // have allowed to the original maximum
        myConfig.setMinimum(30);
        myDoubleSpinBox.setMinimum(30);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }

    void testComplianceMinimumDecimals()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a minimum with more decimals than configured.
        // Check compliance of the rounding behaviour.
        myConfig.setDecimals(1);
        myConfig.setMinimum(12.3456);
        myDoubleSpinBox.setDecimals(1);
        myDoubleSpinBox.setMinimum(12.3456);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());

        // Check compliance when changing the decimals _after_ setting
        // the minimum value. This seems to allow accessing to the
        // original value of minimum in QDoubleSpinBox!
        myConfig.setDecimals(2);
        myDoubleSpinBox.setDecimals(2);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(3);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(4);
        myDoubleSpinBox.setDecimals(4);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(6);
        myDoubleSpinBox.setDecimals(6);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
    }

    void testComplianceMaximumDecimals()
    {
        MultiSpinBoxSectionConfiguration myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a maximum with more decimals than configured.
        // Check compliance of the rounding behaviour.
        myConfig.setDecimals(1);
        myConfig.setMaximum(12.3456);
        myDoubleSpinBox.setDecimals(1);
        myDoubleSpinBox.setMaximum(12.3456);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Check compliance when changing the decimals _after_ setting
        // the maximum value. This seems to allow accessing to the
        // original value of maximum in QDoubleSpinBox!
        myConfig.setDecimals(2);
        myDoubleSpinBox.setDecimals(2);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(3);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(4);
        myDoubleSpinBox.setDecimals(4);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(6);
        myDoubleSpinBox.setDecimals(6);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiSpinBoxSectionConfiguration)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinboxsectionconfiguration.moc"
