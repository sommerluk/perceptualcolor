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
#include "rgbcolorspace.h"
// Second, the private implementation.
#include "rgbcolorspace_p.h"

#include <QtTest>

#include "PerceptualColor/rgbcolorspacefactory.h"

namespace PerceptualColor
{
class TestRgbColorSpace : public QObject
{
    Q_OBJECT

public:
    TestRgbColorSpace(QObject *parent = nullptr)
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

    void testNearestInGamutColorByAdjustingChromaLightness()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        LchDouble color;
        LchDouble nearestInGamutColor;

        // In-gamut colors should not be changed.
        color.l = 50;
        color.c = 20;
        color.h = 10;
        nearestInGamutColor = myColorSpace->nearestInGamutColorByAdjustingChromaLightness(color);
        QVERIFY(nearestInGamutColor.hasSameCoordinates(color));

        // A negative chroma value should not be normalized (this would
        // mean to change the hue), but just put to 0.
        color.l = 50;
        color.c = -20;
        color.h = 10;
        nearestInGamutColor = myColorSpace->nearestInGamutColorByAdjustingChromaLightness(color);
        QCOMPARE(nearestInGamutColor.l, 50);
        QCOMPARE(nearestInGamutColor.c, 0);
        QCOMPARE(nearestInGamutColor.h, 10);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRgbColorSpace)

// The following “include” is necessary because we do not use a header file:
#include "testrgbcolorspace.moc"
