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
#include "lchvalues.h"

#include <QtTest>

#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor
{
class TestLchValues : public QObject
{
    Q_OBJECT

public:
    TestLchValues(QObject *parent = nullptr)
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

    void testLchValues()
    {
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::LchValues::neutralChroma),
                 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::LchValues::neutralHue), 0);
        // Is the value as documented?
        QCOMPARE(
            static_cast<qreal>(PerceptualColor::LchValues::neutralLightness),
            50);

        PerceptualColor::RgbColorSpace temp;
        LchDouble color;
        qreal presicion = 0.1;

        // Test if maxSrgbChroma is big enough
        qreal precisionDegreeMaxSrgbChroma =
            presicion / 360 * 2 * M_PI * LchValues::srgbMaximumChroma;
        color.c = PerceptualColor::LchValues::srgbMaximumChroma;
        for (qreal hue = 0; hue <= 360; hue += precisionDegreeMaxSrgbChroma) {
            color.h = hue;
            for (qreal lightness = 0; lightness < 100; lightness += presicion) {
                color.l = lightness;
                QVERIFY2(!temp.inGamut(color),
                         "Test if maxSrgbChroma is big enough");
            }
        }

        // Test if maxSrgbChroma is as small as possible
        color.c = PerceptualColor::LchValues::srgbMaximumChroma - 1;
        bool inGamutValueFound = false;
        for (qreal hue = 0; hue <= 360; hue += precisionDegreeMaxSrgbChroma) {
            color.h = hue;
            for (qreal lightness = 0; lightness < 100; lightness += presicion) {
                color.l = lightness;
                if (temp.inGamut(color)) {
                    inGamutValueFound = true;
                    break;
                }
            }
            if (inGamutValueFound) {
                break;
            }
        }
        QVERIFY2(inGamutValueFound,
                 "Test if maxSrgbChroma.h is as small as possible");

        // Test if versatile is small enough
        qreal precisionVersatileSrgbChroma = presicion / 360 * 2 * M_PI *
            PerceptualColor::LchValues::srgbVersatileChroma;
        color.c = PerceptualColor::LchValues::srgbVersatileChroma;
        color.l = 50;
        for (qreal hue = 0; hue <= 360; hue += precisionVersatileSrgbChroma) {
            color.h = hue;
            QVERIFY2(temp.inGamut(color), "Test if versatile is small enough");
        }

        // Test if versatile is as big as possible
        color.c = PerceptualColor::LchValues::srgbVersatileChroma + 1;
        color.l = 50;
        inGamutValueFound = true;
        for (qreal hue = 0; hue <= 360; hue += precisionVersatileSrgbChroma) {
            color.h = hue;
            if (!temp.inGamut(color)) {
                inGamutValueFound = false;
                break;
            }
        }
        QVERIFY2(!inGamutValueFound, "Test if versatile is as big as possible");
    }

    void testNeutralGray()
    {
        // Test that the unified initialization is done in the correct order.
        QCOMPARE(LchValues::neutralGray.l,
                 50 // Should be half the way between light and dark
        );
        QCOMPARE(LchValues::neutralGray.c,
                 0 // Should have no chroma
        );
        QCOMPARE(LchValues::neutralGray.h,
                 0 // Hue does not matter, but by convention should be 0
        );
    }
};

}

QTEST_MAIN(PerceptualColor::TestLchValues)
// The following “include” is necessary because we do not use a header file:
#include "testlchvalues.moc"
