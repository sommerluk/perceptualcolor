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

#include <QPainter>

#include <PerceptualColor/abstractdiagram.h>

class TestAbstractDiagramHelperClass : public PerceptualColor::AbstractDiagram
{
Q_OBJECT
public:
void testSnippet01() {
//! [AbstractDiagram Use transparency background]
QImage myImage(150, 200, QImage::Format_ARGB32_Premultiplied);

constexpr qreal myDevicePixelRatioF = 1.25;

QPainter myPainter(&myImage);

// Fill the hole image with tiles made of transparencyBackground()
myPainter.fillRect(
    0,
    0,
    150,
    200,
    // During painting, QBrush will ignore the device pixel ratio
    // of the underlying transparencyBackground image!
    QBrush(
        transparencyBackground(myDevicePixelRatioF)
    )
);

// Paint semi-transparent red color above
myPainter.fillRect(
    0,
    0,
    150,
    200,
    QBrush(
        QColor(255, 0, 0, 128)
    )
);

myImage.setDevicePixelRatio(myDevicePixelRatioF);
//! [AbstractDiagram Use transparency background]
}
};

class TestAbstractDiagram : public QObject
{
    Q_OBJECT

public:
    TestAbstractDiagram(QObject *parent = nullptr) : QObject(parent) {
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

    void testBasic() {
        // Just test if instanciation does not crash:
        PerceptualColor::AbstractDiagram myDiagram;
        myDiagram.show();
        QVERIFY2(
            myDiagram.isVisible(),
            "Test is diagram was shown correctly."
        );
    }

    void testSnippet01() {
        TestAbstractDiagramHelperClass helper;
        helper.testSnippet01();
    }
};

QTEST_MAIN(TestAbstractDiagram)
#include "testabstractdiagram.moc" // necessary because we do not use a header file
