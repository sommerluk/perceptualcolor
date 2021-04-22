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
#include "PerceptualColor/abstractdiagram.h"

#include <QtTest>

#include "helper.h"

#include <QPainter>
#include <QWidget>

class TestAbstractDiagramHelperClass : public PerceptualColor::AbstractDiagram
{
    Q_OBJECT
public:
    TestAbstractDiagramHelperClass(QWidget *parent = nullptr)
        : AbstractDiagram(parent)
    {
        // This constuctor exists only to satisfy Clazy code checker, which
        // expects constructors taking QWidget* as argument for all classes
        // that inherit from QWidget.
    }
    void testSnippet01()
    {
        //! [useTransparencyBackground]
        // Within a class derived from AbstractDiagram, you can use this code:

        QImage myImage(150, 200, QImage::Format_ARGB32_Premultiplied);

        QPainter myPainter(&myImage);

        // Fill the hole image with tiles made of transparencyBackground()
        myPainter.fillRect(0,
                           0,
                           150,
                           200,
                           // During painting, QBrush will ignore the
                           // device pixel ratio of the underlying
                           // transparencyBackground image!
                           QBrush(transparencyBackground()));

        // Paint semi-transparent red color above
        myPainter.fillRect(0, 0, 150, 200, QBrush(QColor(255, 0, 0, 128)));
        //! [useTransparencyBackground]
    }
};

namespace PerceptualColor
{
class TestAbstractDiagram : public QObject
{
    Q_OBJECT

public:
    TestAbstractDiagram(QObject *parent = nullptr)
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

    void testBasic()
    {
        // Just test if instanciation does not crash:
        PerceptualColor::AbstractDiagram myDiagram;
        myDiagram.show();
        QVERIFY2(myDiagram.isVisible(), "Test is diagram was shown correctly.");
    }

    void testSnippet01()
    {
        TestAbstractDiagramHelperClass helper;
        helper.testSnippet01();
    }

    void testTransparencyBackground()
    {
        PerceptualColor::AbstractDiagram myDiagram;
        QImage temp = myDiagram.transparencyBackground();
        QVERIFY2(temp.size().width() > 0, "Width of image is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height of image is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
    }

    void testFocusIndicatorColor()
    {
        QVERIFY2(AbstractDiagram().focusIndicatorColor().isValid(), "focusIndicatorColor() returns a valid color.");
    }

    void testPhysicalPixelSize()
    {
        AbstractDiagram temp;
        temp.show();
        qreal widthError = (temp.width() * temp.devicePixelRatioF()) - temp.physicalPixelSize().width();
        QVERIFY2(qAbs(widthError) < 1, "Rounding width with error < 1.");
        qreal heightError = (temp.height() * temp.devicePixelRatioF()) - temp.physicalPixelSize().height();
        QVERIFY2(qAbs(heightError) < 1, "Rounding height with error < 1.");
    }

    void testDiagramOffset()
    {
        AbstractDiagram myDiagram;
        myDiagram.resize(50, 50);
        QVERIFY2(inRange(49.0, myDiagram.maximumWidgetSquareSize(), 50.0),
                 "Verify that maximumWidgetSquareSize is within expected "
                 "rounding range.");
        // Next try: off by one.
        myDiagram.resize(51, 51);
        QVERIFY2(inRange(50.0, myDiagram.maximumWidgetSquareSize(), 51.0),
                 "Verify that maximumWidgetSquareSize is within expected "
                 "rounding range.");
    }

    void testHandle()
    {
        AbstractDiagram temp;
        QVERIFY2(temp.handleRadius() > 0, "Radius is positive.");
        QVERIFY2(temp.handleOutlineThickness() > 0, "Thickness is positive.");
        QVERIFY2(temp.handleRadius() > temp.handleOutlineThickness(),
                 "Radius is bigger than thickness. "
                 "(Otherwise, there would be no hole in the middle.)");
    }

    void testGradientThickness()
    {
        AbstractDiagram temp;
        QVERIFY(temp.gradientThickness() > 0);
    }

    void testGradientMinimumLength()
    {
        AbstractDiagram temp;
        QVERIFY(temp.gradientMinimumLength() > temp.gradientThickness());
    }

    void testHandleColorFromBackgroundLightness()
    {
        AbstractDiagram temp;
        QCOMPARE(temp.handleColorFromBackgroundLightness(-1), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(0), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(49), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(51), QColor(Qt::black));
        QCOMPARE(temp.handleColorFromBackgroundLightness(100), QColor(Qt::black));
        QCOMPARE(temp.handleColorFromBackgroundLightness(101), QColor(Qt::black));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAbstractDiagram)
// The following “include” is necessary because we do not use a header file:
#include "testabstractdiagram.moc"
