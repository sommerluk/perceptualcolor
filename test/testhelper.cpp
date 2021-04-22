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
#include "helper.h"

#include <QtTest>

#include <QPainter>

#include <rgbcolorspace.h>

namespace PerceptualColor
{
class TestHelper : public QObject
{
    Q_OBJECT

public:
    TestHelper(QObject *parent = nullptr)
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

    void testInRangeInt()
    {
        QCOMPARE(PerceptualColor::inRange<int>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::inRange<int>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::inRange<int>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::inRange<int>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::inRange<int>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::inRange<int>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::inRange<int>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::inRange<int>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::inRange<int>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::inRange<int>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::inRange<int>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::inRange<int>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::inRange<int>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::inRange<int>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::inRange<int>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::inRange<int>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::inRange<int>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::inRange<int>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::inRange<int>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::inRange<int>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::inRange<int>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::inRange<int>(-3, 0, -1), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::inRange<double>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::inRange<double>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::inRange<double>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::inRange<double>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::inRange<double>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::inRange<double>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::inRange<double>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::inRange<double>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::inRange<double>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::inRange<double>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::inRange<double>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::inRange<double>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::inRange<double>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::inRange<double>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::inRange<double>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::inRange<double>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::inRange<double>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::inRange<double>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::inRange<double>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::inRange<double>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::inRange<double>(-3, 0, -1), false);

        QCOMPARE(PerceptualColor::inRange<double>(-3.1, 0.2, -1.3), false);
    }

    void testGamutPrecision()
    {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        QVERIFY2(PerceptualColor::gamutPrecision < 1, "Verify that gamut precision value is not too high");
    }

    void testTransparencyBackground()
    {
        QImage temp = transparencyBackground(1);
        QVERIFY2(temp.size().width() > 0, "Width of image is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height of image is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
    }

    void testStandardWheelSteps()
    {
        QWheelEvent temp(QPointF(), QPointF(), QPoint(), QPoint(200, 120), 0, Qt::Orientation::Vertical, Qt::MouseButton::MiddleButton, Qt::KeyboardModifier::NoModifier);
        QCOMPARE(PerceptualColor::standardWheelSteps(&temp), 1);
    }

    void testLchConvertion()
    {
        // Check if round-trips work fine.
        // One sense
        cmsCIELCh startValue;
        startValue.L = 50.1;
        startValue.C = 20.1;
        startValue.h = 80.1;
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).L, startValue.L);
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).C, startValue.C);
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).h, startValue.h);
        // The other sense
        LchDouble startValue2;
        startValue2.l = 50.1;
        startValue2.c = 20.1;
        startValue2.h = 80.1;
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).l, startValue2.l);
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).c, startValue2.c);
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).h, startValue2.h);
    }

    void testSteps()
    {
        QVERIFY2(pageStepChroma > singleStepChroma, "Chroma page step is bigger than single step.");
        QVERIFY2(singleStepChroma > 0, "Chroma single step is positive.");
        QVERIFY2(pageStepHue > singleStepHue, "Hue page step is bigger than single step.");
        QVERIFY2(singleStepHue > 0, "Hue single step is positive.");
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
