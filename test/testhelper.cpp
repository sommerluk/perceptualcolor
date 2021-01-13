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
#include <PerceptualColor/helper.h>

#include <QtTest>

#include <QPainter>
#include <PerceptualColor/rgbcolorspace.h>

namespace PerceptualColor {

class TestHelper : public QObject
{
    Q_OBJECT

public:
    TestHelper(QObject *parent = nullptr) : QObject(parent) {
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

    void testInRangeInt() {
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<int>(-3, 0, -1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::Helper::inRange<double>(-3, 0, -1), false);
        
        QCOMPARE(
            PerceptualColor::Helper::inRange<double>(-3.1, 0.2, -1.3),
            false
        );
    }

    void testGamutPrecision() {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        QVERIFY2(
            PerceptualColor::Helper::gamutPrecision < 1,
            "Verify that gamut precision value is not too high"
        );
    }

    void testLchDefaults() {
        // Is the value as documented?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultChroma
            ),
            0
        );
        // Is the value as documented?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultHue
            ),
            0
        );
        // Is the value as documented?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultLightness
            ),
            50
        );

        PerceptualColor::RgbColorSpace temp;
        cmsCIELCh color;
        qreal presicion = 0.1;

        // Test if maxSrgbChroma is big enough
        qreal precisionDegreeMaxSrgbChroma =
            presicion
                / 360
                * 2
                * M_PI
                * PerceptualColor::Helper::LchDefaults::maxSrgbChroma;
        color.C = PerceptualColor::Helper::LchDefaults::maxSrgbChroma;
        for (
            qreal hue = 0;
            hue <= 360;
            hue += precisionDegreeMaxSrgbChroma
        ) {
            color.h = hue;
            for (
                qreal lightness = 0;
                lightness < 100;
                lightness += presicion
            ) {
                color.L = lightness;
                QVERIFY2(
                    !temp.inGamut(color),
                    "Test if maxSrgbChroma is big enough"
                );
            }
            
        }
        
        // Test is maxSrgbChroma is as small as possible
        color.C = PerceptualColor::Helper::LchDefaults::maxSrgbChroma - 1;
        bool inGamutValueFound = false;
        for (
            qreal hue = 0;
            hue <= 360;
            hue += precisionDegreeMaxSrgbChroma
        ) {
            color.h = hue;
            for (
                qreal lightness = 0;
                lightness < 100;
                lightness += presicion
            ) {
                color.L = lightness;
                if (temp.inGamut(color)) {
                    inGamutValueFound = true;
                    break;
                }
            }
            if (inGamutValueFound) {
                break;
            }
        }
        QVERIFY2(
            inGamutValueFound,
            "Test is maxSrgbChroma is as small as possible"
        );

        // Test if versatile is small enough
        qreal precisionVersatileSrgbChroma =
            presicion
                / 360
                * 2
                * M_PI
                * PerceptualColor::Helper::LchDefaults::versatileSrgbChroma;
        color.C = PerceptualColor::Helper::LchDefaults::versatileSrgbChroma;
        color.L = 50;
        for (
            qreal hue = 0;
            hue <= 360;
            hue += precisionVersatileSrgbChroma
        ) {
            color.h = hue;
            QVERIFY2(
                temp.inGamut(color),
                "Test if versatile is small enough"
            );
        }

        // Test is versatile is as big as possible
        color.C =
            PerceptualColor::Helper::LchDefaults::versatileSrgbChroma + 1;
        color.L = 50;
        inGamutValueFound = true;
        for (
            qreal hue = 0;
            hue <= 360;
            hue += precisionVersatileSrgbChroma
        ) {
            color.h = hue;
            if (!temp.inGamut(color)) {
                inGamutValueFound = false;
                break;
            }
        }
        QVERIFY2(
            !inGamutValueFound,
            "Test is versatile is as big as possible"
        );

    }

    void testStandardWheelSteps() {
        QWheelEvent temp(
            QPointF(),
            QPointF(),
            QPoint(),
            QPoint(200, 120),
            0,
            Qt::Orientation::Vertical,
            Qt::MouseButton::MiddleButton,
            Qt::KeyboardModifier::NoModifier
        );
        QCOMPARE(PerceptualColor::Helper::standardWheelSteps(&temp), 1);
    }

};

}

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
