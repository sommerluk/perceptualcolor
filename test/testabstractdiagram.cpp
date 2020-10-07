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

namespace PerceptualColor {

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

    void testTransparencyBackground() {
        QImage temp = PerceptualColor::AbstractDiagram::transparencyBackground(1);
        QVERIFY2(
            temp.size().width() > 0,
            "Width of unscaled image is bigger than 0."
        );
        QVERIFY2(
            temp.size().height() > 0,
            "Height of unscaled image is bigger than 0."
        );
        QVERIFY2(
            temp.allGray(),
            "Unscaled image is neutral gray."
        );
        QImage temp2 = AbstractDiagram::transparencyBackground(1.25);
        QVERIFY2(
            temp2.size().width() > temp.size().width(),
            "Width of upscaled image is biggen than unscaled image."
        );
        QVERIFY2(
            temp2.size().height() > temp.size().height(),
            "Height of upscaled image is bigger than unscaled image."
        );
        QVERIFY2(
            temp.allGray(),
            "Upscaled image is neutral gray."
        );
    }
    
    void testFocusIndicatorColor() {
        QVERIFY2(
            AbstractDiagram().focusIndicatorColor().isValid(),
            "focusIndicatorColor() returns a valid color."
        );
    }
    
    void testPhysicalPixelSize() {
        AbstractDiagram temp;
        temp.show();
        qreal widthError =
            (temp.width() * temp.devicePixelRatioF())
            - temp.physicalPixelSize().width();
        QVERIFY2(
            qAbs(widthError) < 1,
            "Rounding width with error < 1."
        );
        qreal heightError =
            (temp.height() * temp.devicePixelRatioF())
            - temp.physicalPixelSize().height();
        QVERIFY2(
            qAbs(heightError) < 1,
            "Rounding height with error < 1."
        );
    }
    
    void testMarker() {
        AbstractDiagram temp;
        QVERIFY2(
            temp.markerRadius > 0,
            "Radius is positive."
        );
        QVERIFY2(
            temp.markerThickness > 0,
            "Thickness is positive."
        );
        QVERIFY2(
            temp.markerRadius > temp.markerThickness,
            "Radius is bigger than thickness. "
            "(Otherwise, there would be no hole in the middle.)"
        );
        QVERIFY2(
            std::is_floating_point<decltype(temp.markerRadius)>::value,
            "markerRadius is floating point (important for high-dpi support)"
        );
        QVERIFY2(
            std::is_floating_point<decltype(temp.markerThickness)>::value,
            "markerThickness is floating point (important for "
                "high-dpi support)"
        );
    }
    
    void testSteps() {
        AbstractDiagram temp;
        QVERIFY2(
            temp.pageStepChroma > temp.singleStepChroma,
            "Chroma page step is bigger than single step."
        );
        QVERIFY2(
            temp.singleStepChroma > 0,
            "Chroma single step is positive."
        );
        QVERIFY2(
            temp.pageStepHue > temp.singleStepHue,
            "Hue page step is bigger than single step."
        );
        QVERIFY2(
            temp.singleStepHue > 0,
            "Hue single step is positive."
        );
    }

};

}

QTEST_MAIN(PerceptualColor::TestAbstractDiagram)

#include "testabstractdiagram.moc" // necessary because we do not use a header file
