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
#include <PerceptualColor/helper.h>
#include <PerceptualColor/rgbcolorspace.h>

class TestHelper : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase() {
        // Called before the first testfunction is executed
    }

    void cleanupTestCase() {
        // Called after the last testfunction was executed
    };

    void init() {
        // Called before each testfunction is executed
    };
    void cleanup() {
        // Called after every testfunction
    };

    void testCmsRgb() {
        cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(NULL);
        cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
        cmsHTRANSFORM m_transformLabToRgbHandle = cmsCreateTransform(
            labProfileHandle,             // input profile handle
            TYPE_Lab_DBL,                 // input buffer format
            rgbProfileHandle,             // output profile handle
            TYPE_RGB_DBL,                 // output buffer format
            INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
            0                             // flags
        );
        PerceptualColor::Helper::cmsRGB rgb;
        cmsCIELab lab;
        lab.L = 50;
        lab.a = 0;
        lab.b = 0;
        // Test if the following line does not produce
        // a memory error on the heap.
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        // Test if the result is okay (so it has to be neutral gray: red,
        // green and blue should be roughly the same)
        QCOMPARE(qRound(rgb.red * 255), qRound(rgb.blue * 255));
        QCOMPARE(qRound(rgb.green * 255), qRound(rgb.blue * 255));
        // Test if Red, Green, Blue are at the correcpt position in memory
        lab.L = 53;
        lab.a = 80;
        lab.b = 67;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.red > 0.8, "Test if Red is at the correcpt position in memory");
        lab.L = 87;
        lab.a = -86;
        lab.b = 83;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.green > 0.8, "Test if Green is at the correcpt position in memory");
        lab.L = 32;
        lab.a = 79;
        lab.b = -107;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.blue > 0.8, "Test if Blue is at the correcpt position in memory");
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
    };

    void testGamutPrecision() {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        QVERIFY2(
            static_cast<qreal>(
                PerceptualColor::Helper::gamutPrecision
            ) < 1,
            "Gamut precison is not too high"
        );
    }

    void testLchDefaults() {
        // Is the value as documentated?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultChroma
            ),
            0
        );
        // Is the value as documentated?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultHue
            ),
            0
        );
        // Is the value as documentated?
        QCOMPARE(
            static_cast<qreal>(
                PerceptualColor::Helper::LchDefaults::defaultLightness
            ),
            50
        );
        
        PerceptualColor::RgbColorSpace temp;
        cmsCIELCh color;
        qreal presicion = 0.03;

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

    void testTransparencyBackground() {
        QImage temp = PerceptualColor::Helper::transparencyBackground();
        QVERIFY2(temp.size().width() > 0, "Width is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
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

QTEST_MAIN(TestHelper);
#include "testhelper.moc" // necessary because we do not use a header file
