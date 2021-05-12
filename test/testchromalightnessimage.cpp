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
#include "chromalightnessimage.h"

#include "PerceptualColor/rgbcolorspacefactory.h"
#include "helper.h"

#include <QtTest>

namespace PerceptualColor
{
class TestChromaLightnessImage : public QObject
{
    Q_OBJECT

public:
    TestChromaLightnessImage(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::create();

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
        ChromaLightnessImage test(m_rgbColorSpace);
    }

    void testGetImage()
    {
        QImage m_image;
        ChromaLightnessImage m_imageProvider(m_rgbColorSpace);
        m_imageProvider.setHue(0);

        // Testing extremely small images

        m_imageProvider.setImageSize(QSize(0, 0));
        m_image = m_imageProvider.getImage();
        QCOMPARE(m_image.size(), QSize(0, 0));

        m_imageProvider.setImageSize(QSize(1, 1));
        m_image = m_imageProvider.getImage();
        QCOMPARE(m_image.size(), QSize(1, 1));

        m_imageProvider.setImageSize(QSize(2, 2));
        m_image = m_imageProvider.getImage();
        QCOMPARE(m_image.size(), QSize(2, 2));

        m_imageProvider.setImageSize(QSize(-1, -1));
        m_image = m_imageProvider.getImage();
        QCOMPARE(m_image.size(), QSize(0, 0));

        // Start testing for a normal size image
        m_imageProvider.setImageSize(QSize(201, 101));
        m_image = m_imageProvider.getImage();
        QCOMPARE(m_image.height(), 101);
        QCOMPARE(m_image.width(), 201);

        // Test if position within the QImage is valid:
        QCOMPARE(m_image.pixelColor(0, 0).isValid(), true);

        // Test if position within the QImage is valid:
        QCOMPARE(m_image.pixelColor(0, 100).isValid(), true);

        // Test if position within the QImage is invalid:
        QTest::ignoreMessage(QtWarningMsg, "QImage::pixelColor: coordinate (0,101) out of range");
        QCOMPARE(m_image.pixelColor(0, 101).isValid(), false);
    }

    void testImageSize()
    {
        ChromaLightnessImage test(m_rgbColorSpace);
        // Test especially small values, that might make special
        // problems in the algorithm (devision by zero, offset by 1…)
        QCOMPARE(test.getImage().size(), QSize(0, 0));
        test.setImageSize(QSize(1, 1));
        QCOMPARE(test.getImage().size(), QSize(1, 1));
        test.setImageSize(QSize(2, 2));
        QCOMPARE(test.getImage().size(), QSize(2, 2));
        test.setImageSize(QSize(3, 3));
        QCOMPARE(test.getImage().size(), QSize(3, 3));
        test.setImageSize(QSize(4, 4));
        QCOMPARE(test.getImage().size(), QSize(4, 4));
        test.setImageSize(QSize(5, 5));
        QCOMPARE(test.getImage().size(), QSize(5, 5));
        // Test a normal size value
        test.setImageSize(QSize(500, 500));
        QCOMPARE(test.getImage().size(), QSize(500, 500));
    }

    void testCache()
    {
        ChromaLightnessImage test(m_rgbColorSpace);
        test.setImageSize(QSize(50, 25)); // Set a non-zero image size
        QVERIFY2(test.m_image.isNull(), "Verify that instantiation is done with empty cache.");
        test.setHue(5);
        QVERIFY2(test.m_image.isNull(), "Verify that setting hue does not trigger re-calculation.");
        test.getImage();
        QVERIFY2(!test.m_image.isNull(), "Verify that getImage() triggers re-calculation.");
        test.setHue(5);
        QVERIFY2(!test.m_image.isNull(),
                 "Verify that setBorder() does not erease the cache"
                 " if the value that was set is the same than before.");
    }

    void testSetHue_data()
    {
        QTest::addColumn<qreal>("hue");
        QTest::newRow("0") << 0.;
        QTest::newRow("20") << 20.;
        QTest::newRow("30") << 30.;
        QTest::newRow("40") << 40.;
        QTest::newRow("50") << 50.;
        QTest::newRow("60") << 60.;
        QTest::newRow("70") << 70.;
        QTest::newRow("80") << 80.;
        QTest::newRow("290") << 290.;
    }

    void testSetHueInvalid()
    {
        // Make sure that calling setHue with invalid values
        // does not crash.
        ChromaLightnessImage test(m_rgbColorSpace);
        test.setImageSize(QSize(20, 10)); // Set a non-zero image size
        test.setHue(0);
        Q_UNUSED(test.getImage());
        test.setHue(1);
        Q_UNUSED(test.getImage());
        test.setHue(2);
        Q_UNUSED(test.getImage());
        test.setHue(-10);
        Q_UNUSED(test.getImage());
        test.setHue(-1000);
        Q_UNUSED(test.getImage());
        test.setHue(100);
        Q_UNUSED(test.getImage());
        test.setHue(110);
        Q_UNUSED(test.getImage());
        test.setHue(250);
        Q_UNUSED(test.getImage());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaLightnessImage)

// The following “include” is necessary because we do not use a header file:
#include "testchromalightnessimage.moc"
