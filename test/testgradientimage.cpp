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

#include "perceptualcolorlib_internal.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "gradientimage.h"

#include <QtTest>

class TestGradientSnippetClass : public QWidget
{
    Q_OBJECT
public:
    // A constructor that is clazy-conform
    TestGradientSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }
    void testSnippet01()
    {
        //! [GradientImage HiDPI usage]
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace {
            new PerceptualColor::RgbColorSpace};
        PerceptualColor::GradientImage test(myColorSpace);
        // The function setImageSize() expects an int
        // value. static_cast<int> will round down, which
        // is the desired behaviour here. (Rounding up
        // would mean one more pixel, and on some Qt
        // styles this would fail.)
        test.setGradientLength(static_cast<int>(100 * devicePixelRatioF()));
        test.setGradientThickness(static_cast<int>(100 * devicePixelRatioF()));
        PerceptualColor::LchaDouble firstColor;
        firstColor.h = 10;
        firstColor.l = 20;
        firstColor.c = 30;
        firstColor.a = 0.4;
        test.setFirstColor(firstColor);
        PerceptualColor::LchaDouble secondColor;
        secondColor.h = 50;
        secondColor.l = 60;
        secondColor.c = 25;
        secondColor.a = 0.9;
        test.setFirstColor(secondColor);
        test.setDevicePixelRatioF(devicePixelRatioF());
        QImage myImage = test.getImage();
        //! [GradientImage HiDPI usage]
        Q_UNUSED(myImage)
    }
};

namespace PerceptualColor
{
class TestGradientImage : public QObject
{
    Q_OBJECT

public:
    TestGradientImage(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace {
        new RgbColorSpace()};

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

    void testConstructorDestructor()
    {
        // Constructor and destructor do not crash.
        GradientImage myGradient(m_rgbColorSpace);
    }

    void testCompletlyNormalizedAndBounded()
    {
        GradientImage myGradient(m_rgbColorSpace);
        LchaDouble lchaTestValue;

        // Test values that are too high
        lchaTestValue.l = 500;
        lchaTestValue.c = 20;
        lchaTestValue.h = 361;
        lchaTestValue.a = 5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l,
                 100);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h, 1);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 1);

        // Test value that are too low
        lchaTestValue.l = -500;
        lchaTestValue.c = -20;
        lchaTestValue.h = -1;
        lchaTestValue.a = -5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c,
                 20 // Normaized to positive value (hue is changed by 180°)
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h,
                 179 // Changed by 180° because of the negatif chroma value
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 0);

        // Test value that much too low
        lchaTestValue.l = 50;
        lchaTestValue.c = 20;
        lchaTestValue.h = -361;
        lchaTestValue.a = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h,
                 359);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a,
                 0.5);

        // Test that hue is preserved also if chroma is zero
        lchaTestValue.l = 50;
        lchaTestValue.c = 0;
        lchaTestValue.h = 50;
        lchaTestValue.a = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a,
                 0.5);
    }

    void testSetFirstColor()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        myGradient.getImage();
        QCOMPARE(myGradient.m_image.isNull(), false);
        LchaDouble lchaTestValue(50, 20, 30, 0.5);
        myGradient.setFirstColor(lchaTestValue);
        lchaTestValue.l = 60;
        myGradient.setFirstColor(lchaTestValue);
        QCOMPARE(myGradient.m_image.isNull(), true);
    }

    void testSetSecondColor()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        myGradient.getImage();
        QCOMPARE(myGradient.m_image.isNull(), false);
        LchaDouble lchaTestValue(50, 20, 30, 0.5);
        myGradient.setSecondColor(lchaTestValue);
        lchaTestValue.l = 60;
        myGradient.setSecondColor(lchaTestValue);
        QCOMPARE(myGradient.m_image.isNull(), true);
    }

    void testUpdateSecondColor()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.m_firstColorCorrected = LchaDouble(50, 0, 30, 0.5);
        myGradient.m_secondColorCorrectedAndAltered =
            LchaDouble(50, 0, 40, 0.5);
        myGradient.updateSecondColor();
        qreal absoluteDifference =
            qAbs(myGradient.m_firstColorCorrected.h -
                 myGradient.m_secondColorCorrectedAndAltered.h);
        QVERIFY2(absoluteDifference <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered =
            LchaDouble(50, 0, 240, 0.5);
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h -
                      myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered =
            LchaDouble(50, 0, 540, 0.5);
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h -
                      myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered =
            LchaDouble(50, 0, -240, 0.5);
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h -
                      myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
    }

    void testGetImage()
    {
        GradientImage myGradient(m_rgbColorSpace);
        // Should not crash also when values are not initialized.
        myGradient.getImage();
        // Returned image should be valid if size is bigger than 0
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        QCOMPARE(myGradient.getImage().isNull(), false);
    }

    void testColorFromValue()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.m_firstColorCorrected = LchaDouble(50, 0, 30, 0.5);
        myGradient.m_secondColorCorrectedAndAltered =
            LchaDouble(60, 10, 20, 0.4);
        LchaDouble middleColor = myGradient.colorFromValue(0.5);
        QCOMPARE(middleColor.l, 55);
        QCOMPARE(middleColor.c, 5);
        QCOMPARE(middleColor.h, 25);
        QCOMPARE(middleColor.a, 0.45);
    }

    void testSetDevicelPixelRatioF()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        myGradient.getImage();
        QCOMPARE(myGradient.m_image.isNull(), false);
        myGradient.setDevicePixelRatioF(1.25);
        myGradient.setDevicePixelRatioF(1.5);
        QCOMPARE(myGradient.m_image.isNull(), true);
    }

    void testSetGradientLength()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        myGradient.getImage();
        QCOMPARE(myGradient.m_image.isNull(), false);
        myGradient.setGradientLength(15);
        myGradient.setGradientLength(18);
        QCOMPARE(myGradient.m_image.isNull(), true);
    }

    void testSetGradientThickness()
    {
        GradientImage myGradient(m_rgbColorSpace);
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        myGradient.getImage();
        QCOMPARE(myGradient.m_image.isNull(), false);
        myGradient.setGradientThickness(15);
        myGradient.setGradientThickness(18);
        QCOMPARE(myGradient.m_image.isNull(), true);
    }

    void testSnippet01()
    {
        TestGradientSnippetClass mySnippets;
        mySnippets.testSnippet01();
    }
};

}

QTEST_MAIN(PerceptualColor::TestGradientImage)

// The following “include” is necessary because we do not use a header file:
#include "testgradientimage.moc"
