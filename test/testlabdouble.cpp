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

#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/labdouble.h"

#include <QtTest>

#include <lcms2.h>

namespace PerceptualColor {

class TestLabDouble : public QObject
{

    Q_OBJECT

public:
    TestLabDouble(QObject *parent = nullptr) : QObject(parent) {
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
    }

    void testConstructorDestructor() {
        // This should not crash.
        LabDouble test;
        test.L = 50;
        Q_UNUSED(test);
    }

    void testCopyConstructor() {
        // This should not crash.
        LabDouble test;
        test.L = 50;
        test.a = 25;
        test.b = 5;
        LabDouble copy(test);
        QCOMPARE(
            copy.L,
            50
        );
        QCOMPARE(
            copy.a,
            25
        );
        QCOMPARE(
            copy.b,
            5
        );
    }
    
void testLabDouble1() {
//! [Use LabDouble]
PerceptualColor::LabDouble test1;
test1.L = 50; // lightness: 50%
test1.a = 25; // red-green-axis: 25
test1.b = -5; // yellow-blue-axis: -5
//! [Use LabDouble]
Q_UNUSED(test1)
}

};

}

QTEST_MAIN(PerceptualColor::TestLabDouble)

// The following “include” is necessary because we do not use a header file:
#include "testlabdouble.moc"
