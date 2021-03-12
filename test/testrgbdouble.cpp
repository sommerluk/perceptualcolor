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

#include "perceptualcolorlib_qtconfiguration.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/rgbdouble.h"

#include <QtTest>

#include <lcms2.h>

namespace PerceptualColor {

class TestRgbDouble : public QObject
{

    Q_OBJECT

public:
    TestRgbDouble(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    static void voidMessageHandler(
        QtMsgType,
        const QMessageLogContext &,
        const QString &
    ) {
        // dummy message handler that does not print messages
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

    void testConstructorDestructor() {
        // This should not crash.
        RgbDouble test;
        test.red = 0.5;
        Q_UNUSED(test);
    }

    void testCopyConstructor() {
        // This should not crash.
        RgbDouble test;
        test.red = 0.5;
        test.green = 0.6;
        test.blue = 0.7;
        RgbDouble copy(test);
        QCOMPARE(
            copy.red,
            0.5
        );
        QCOMPARE(
            copy.green,
            0.6
        );
        QCOMPARE(
            copy.blue,
            0.7
        );
    }

    void testQDebugSupport()
    {
        PerceptualColor::RgbDouble test;
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);
        qDebug() << test;
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testRgbDouble() {
        cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(nullptr);
        cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
        cmsHTRANSFORM m_transformLabToRgbHandle = cmsCreateTransform(
            labProfileHandle,             // input profile handle
            TYPE_Lab_DBL,                 // input buffer format
            rgbProfileHandle,             // output profile handle
            TYPE_RGB_DBL,                 // output buffer format
            INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
            0                             // flags
        );
        cmsCloseProfile(labProfileHandle);
        cmsCloseProfile(rgbProfileHandle);
        PerceptualColor::RgbDouble rgb;
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
        // Test if Red, Green, Blue are at the correct position in memory
        lab.L = 53;
        lab.a = 80;
        lab.b = 67;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(
            rgb.red > 0.8,
            "Test if Red is at the correct position in memory"
        );
        lab.L = 87;
        lab.a = -86;
        lab.b = 83;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(
            rgb.green > 0.8,
            "Test if Green is at the correct position in memory"
        );
        lab.L = 32;
        lab.a = 79;
        lab.b = -107;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(
            rgb.blue > 0.8,
            "Test if Blue is at the correct position in memory"
        );

        // Clean up
        cmsDeleteTransform(m_transformLabToRgbHandle);
    }

void testSnippet01() {
//! [Use RgbDouble]
cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(nullptr);
cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
cmsHTRANSFORM m_transformRgbToLabHandle = cmsCreateTransform(
    rgbProfileHandle,             // input profile handle
    TYPE_RGB_DBL,                 // input buffer format
    labProfileHandle,             // output profile handle
    TYPE_Lab_DBL,                 // output buffer format
    INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
    0                             // flags
);
cmsCloseProfile(labProfileHandle);
cmsCloseProfile(rgbProfileHandle);
PerceptualColor::RgbDouble rgb;
rgb.red = 1;
rgb.green = 0.5;
rgb.blue = 0;
cmsCIELab lab;
// Convert exactly 1 value:
cmsDoTransform(
    m_transformRgbToLabHandle,  // transform handle
    &rgb,                       // input buffer
    &lab,                       // output buffer
    1                           // number of RGB values to convert
);
cmsDeleteTransform(m_transformRgbToLabHandle);
//! [Use RgbDouble]
constexpr int tolerance = 5;
constexpr int expectedL = 68;
constexpr int expectedA = 46;
constexpr int expectedB = 75;
QVERIFY2(
    (expectedL - tolerance) < lab.L,
    "Verify that hue is within tolerance."
);
QVERIFY2(
    lab.L < (expectedL + tolerance),
    "Verify that hue is within tolerance."
);
QVERIFY2(
    (expectedA - tolerance) < lab.a,
    "Verify that lightness is within tolerance."
);
QVERIFY2(
    lab.a < (expectedA + tolerance),
    "Verify that lightness is within tolerance."
);
QVERIFY2(
    (expectedB - tolerance) < lab.b,
    "Verify that chroma is within tolerance."
);
QVERIFY2(
    lab.b < (expectedB + tolerance),
    "Verify that chroma is within tolerance."
);
}

};

}

QTEST_MAIN(PerceptualColor::TestRgbDouble)

// The following “include” is necessary because we do not use a header file:
#include "testrgbdouble.moc"
