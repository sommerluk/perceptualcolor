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

#include <QTest>
#include <QtTest/QtTest>
#include <QObject>
#include <PerceptualColor/helper.h>
#include "PerceptualColor/chromalightnessdiagram.h"

class TestHelper : public QObject
{
    Q_OBJECT

private:
    cmsHTRANSFORM littlecmsColorTransform;

private Q_SLOTS:
    void initTestCase() {
        // Create a LittleCMS transform for our LittleCMS context
        // declaration of variables
        cmsHPROFILE hInProfile, hOutProfile;
        // Create a v4 ICC profile object for the Lab color space. NULL means: White point D50.
        hInProfile = cmsCreateLab4Profile(NULL);
        // create an ICC profile object for the sRGB color space
        hOutProfile = cmsCreate_sRGBProfile();
        // Create the transform
        littlecmsColorTransform = cmsCreateTransform(
            hInProfile,                   // input profile object
            TYPE_Lab_DBL,                 // input buffer format
            hOutProfile,                  // output profile object
            TYPE_RGB_DBL,                 // output buffer format
            INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
            0                             // flags
        );
        // Close the profiles as they are no longer needed once the transform has been set up.
        // So we can save memory.
        cmsCloseProfile(hInProfile);
        cmsCloseProfile(hOutProfile);
    };
    void cleanupTestCase() {
        // Called after the last testfunction was executed
        // Free memory
        cmsDeleteTransform(littlecmsColorTransform);
    };

    void init() {
        // Called before each testfunction is executed
    };
    void cleanup() {
        // Called after every testfunction
    };

    /*
    void testChromaLightnessDiagramm() {
        QImage mImage;

        // Testing extremly small images
        mImage = PerceptualColor::ChromaLightnessDiagram::diagramImage(0, QSize(0, 0), littlecmsColorTransform);
        QCOMPARE(mImage.size(), QSize(0, 0));
        mImage = PerceptualColor::ChromaLightnessDiagram::diagramImage(0, QSize(1, 1), littlecmsColorTransform);
        QCOMPARE(mImage.size(), QSize(1, 1));
        mImage = PerceptualColor::ChromaLightnessDiagram::diagramImage(0, QSize(2, 2), littlecmsColorTransform);
        QCOMPARE(mImage.size(), QSize(2, 2));
        mImage = PerceptualColor::ChromaLightnessDiagram::diagramImage(0, QSize(-1, -1), littlecmsColorTransform);
        QCOMPARE(mImage.size(), QSize(0, 0));

        // Start testing for a normal size image
        mImage = PerceptualColor::ChromaLightnessDiagram::diagramImage(0, QSize(201, 101), littlecmsColorTransform);
        QCOMPARE(mImage.height(), 101);
        QCOMPARE(mImage.width(), 201);
        QCOMPARE(mImage.pixelColor(0, 0).isValid(), true); // position within the QImage is valid
        QCOMPARE(mImage.pixelColor(0, 100).isValid(), true); // position within the QImage is valid
        QTest::ignoreMessage(QtWarningMsg, "QImage::pixelColor: coordinate (0,101) out of range");
        QCOMPARE(mImage.pixelColor(0, 101).isValid(), false); // position within the QImage is invalid
    }
    */


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
    };
};

QTEST_MAIN(TestHelper);
#include "testhelper.moc" // necessary because we do not use a header file
