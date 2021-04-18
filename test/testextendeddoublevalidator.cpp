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

#include "perceptualinternal.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "extendeddoublevalidator.h"

#include <QtTest>

namespace PerceptualColor
{
class TestExtendedDoubleValidator : public QObject
{
    Q_OBJECT

public:
    TestExtendedDoubleValidator(QObject *parent = nullptr)
        : QObject(parent)
    {
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

    void testConstructor()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
    }

    void testConstructor2()
    {
        QPointer<QObject> myObject(new QObject());
        QPointer<PerceptualColor::ExtendedDoubleValidator> myGuardedPointer = new ExtendedDoubleValidator(myObject);
        delete myObject;
        QVERIFY2(myGuardedPointer.isNull(),
                 "Verify that the guarded pointer “QPointer” becomes nullptr if "
                 "the parent is deleted.");
    }

    void testPrefix()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
        QCOMPARE(myValidator.prefix(), QLatin1String());
        const QString myString = QStringLiteral("abc");
        myValidator.setPrefix(myString);
        QCOMPARE(myValidator.prefix(), myString);
        myValidator.setRange(0, 1000);
        QString testString;
        int testPosition = 0;
        testString = QStringLiteral("abc1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("abc1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("ab1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("ab1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QLatin1String();
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
    }

    void testSuffix()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
        QCOMPARE(myValidator.suffix(), QLatin1String());
        const QString myString = QStringLiteral("abc");
        myValidator.setSuffix(myString);
        QCOMPARE(myValidator.suffix(), myString);
        myValidator.setRange(0, 1000);
        QString testString;
        int testPosition = 0;
        testString = QStringLiteral("1abc");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("1000abc");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("1ab");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000ab");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QLatin1String();
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
    }

    void testValidate()
    {
        ExtendedDoubleValidator myValidator;
        myValidator.setPrefix(QStringLiteral("abc"));
        myValidator.setSuffix(QStringLiteral("def"));
        myValidator.setRange(0, 1000);
        const QString originalInput = QStringLiteral("abc123def");
        QString myInput = originalInput;
        const int originalPos = 5;
        int myPos = originalPos;
        QValidator::State result = myValidator.validate(myInput, myPos);
        // The input should be considered valid.
        QCOMPARE(result, QValidator::State::Acceptable);
        // On simple cases of valid input, the string should not change.
        QCOMPARE(myInput, originalInput);
        // On simple cases of valid input, the position should not change.
        QCOMPARE(myPos, originalPos);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestExtendedDoubleValidator)

// The following “include” is necessary because we do not use a header file:
#include "testextendeddoublevalidator.moc"
