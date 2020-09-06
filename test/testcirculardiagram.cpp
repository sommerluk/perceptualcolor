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

#include <PerceptualColor/circulardiagram.h>

class TestCircularDiagram : public QObject
{
    Q_OBJECT

public:
    TestCircularDiagram(QObject *parent = nullptr) : QObject(parent) {
    }

private Q_SLOTS:
    void initTestCase() {
        // Called before the first testfunction is executed
    }

    void cleanupTestCase() {
        // Called after the last testfunction was executed
    }

    void init() {
        // Called before each testfunction is executed
    }

    void cleanup() {
        // Called after every testfunction
    }

    void testHasHeighForWidth() {
        PerceptualColor::CircularDiagram myCircularDiagram;
        QCOMPARE(myCircularDiagram.hasHeightForWidth(), true);
    }

    void testHeighForWidth() {
        PerceptualColor::CircularDiagram myCircularDiagram;
        QCOMPARE(myCircularDiagram.heightForWidth(1), 1);
        QCOMPARE(myCircularDiagram.heightForWidth(15), 15);
        QCOMPARE(myCircularDiagram.heightForWidth(100), 100);
        QCOMPARE(myCircularDiagram.heightForWidth(1000), 1000);
    }

    void testSizePolicy() {
        PerceptualColor::CircularDiagram myCircularDiagram;
        QCOMPARE(
            myCircularDiagram.sizePolicy().horizontalPolicy(),
            QSizePolicy::Expanding
        );
        QCOMPARE(
            myCircularDiagram.sizePolicy().verticalPolicy(),
            QSizePolicy::Expanding
        );
        QVERIFY2(
            myCircularDiagram.sizePolicy().hasHeightForWidth(),
            "Size policy should have hasHeightForWidth"
        );
    }
};

QTEST_MAIN(TestCircularDiagram)
#include "testcirculardiagram.moc" // necessary because we do not use a header file
