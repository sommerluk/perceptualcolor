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

// First included header is the header of the class we are testing;
// this forces the header to be self-contained.
#include "chromahueimage.h"

#include <QtTest>

#include "helper.h"
#include "lchvalues.h"

class TestChromaHueSnippetClass : public QWidget
{
    Q_OBJECT
public:
    // A constructor that is clazy-conform
    TestChromaHueSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }
    void testSnippet01()
    {
        //! [ChromaHueImage HiDPI usage]
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace {new PerceptualColor::RgbColorSpace()};
        PerceptualColor::ChromaHueImage test(myColorSpace);
        // The function setImageSize() expects an int
        // value. static_cast<int> will round down, which
        // is the desired behaviour here. (Rounding up
        // would mean one more physical pixel, and on some Qt
        // styles this would fail.)
        test.setImageSize(static_cast<int>(100 * devicePixelRatioF()));
        test.setBorder(5 * devicePixelRatioF());
        test.setDevicePixelRatioF(devicePixelRatioF());
        QImage myImage = test.getImage();
        //! [ChromaHueImage HiDPI usage]
        Q_UNUSED(myImage)
    }
};

namespace PerceptualColor
{
class TestChromaHueImage : public QObject
{
    Q_OBJECT

public:
    TestChromaHueImage(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<RgbColorSpace> colorSpace {new RgbColorSpace};

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

    void testConstructor()
    {
        ChromaHueImage test(colorSpace);
    }

    void testGetImage()
    {
        ChromaHueImage test(colorSpace);
        Q_UNUSED(test.getImage());
    }

    void testImageSize()
    {
        ChromaHueImage test(colorSpace);
        // Test especially small values, that might make special
        // problems in the algorithm (devision by zero, offset by 1…)
        QCOMPARE(test.getImage().size(), QSize(0, 0));
        test.setImageSize(1);
        QCOMPARE(test.getImage().size(), QSize(1, 1));
        test.setImageSize(2);
        QCOMPARE(test.getImage().size(), QSize(2, 2));
        test.setImageSize(3);
        QCOMPARE(test.getImage().size(), QSize(3, 3));
        test.setImageSize(4);
        QCOMPARE(test.getImage().size(), QSize(4, 4));
        test.setImageSize(5);
        QCOMPARE(test.getImage().size(), QSize(5, 5));
        // Test a normal size value
        test.setImageSize(500);
        QCOMPARE(test.getImage().size(), QSize(500, 500));
    }

    void testDevicePixelRatioF()
    {
        ChromaHueImage test(colorSpace);
        test.setImageSize(100);
        // Image size is as described.
        QCOMPARE(test.getImage().size(), QSize(100, 100));
        // Default devicePixelRatioF is 1
        QCOMPARE(test.getImage().devicePixelRatio(), 1);
        // Testing with a (non-integer) scale factor
        test.setDevicePixelRatioF(1.5);
        // Image size remains unchanged.
        QCOMPARE(test.getImage().size(), QSize(100, 100));
        // Default devicePixelRatioF is 1.5
        QCOMPARE(test.getImage().devicePixelRatio(), 1.5);
    }

    void testBorderOdd()
    {
        ChromaHueImage test(colorSpace);
        test.setImageSize(99);
        // Default border is zero: no transparent border.
        QVERIFY2(test.getImage().pixelColor(49, 0).alpha() > 0, "Verify that pixel top center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(49, 98).alpha() > 0, "Verify that pixel bottom center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(0, 49).alpha() > 0, "Verify that pixel left is not transparent.");
        QVERIFY2(test.getImage().pixelColor(98, 49).alpha() > 0, "Verify that pixel right is not transparent.");
        test.setBorder(1);
        // Now, the pixels should become transparent.
        QCOMPARE(test.getImage().pixelColor(49, 0).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(49, 98).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(0, 49).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(98, 49).alpha(), 0);
    }

    void testBorderEven()
    {
        ChromaHueImage test(colorSpace);
        test.setImageSize(100);
        // Default border is zero: no transparent border.
        QVERIFY2(test.getImage().pixelColor(49, 0).alpha() > 0, "Verify that pixel top center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(50, 0).alpha() > 0, "Verify that pixel top center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(49, 99).alpha() > 0, "Verify that pixel bottom center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(50, 99).alpha() > 0, "Verify that pixel bottom center is not transparent.");
        QVERIFY2(test.getImage().pixelColor(0, 49).alpha() > 0, "Verify that pixel left is not transparent.");
        QVERIFY2(test.getImage().pixelColor(0, 50).alpha() > 0, "Verify that pixel left is not transparent.");
        QVERIFY2(test.getImage().pixelColor(99, 49).alpha() > 0, "Verify that pixel right is not transparent.");
        QVERIFY2(test.getImage().pixelColor(99, 50).alpha() > 0, "Verify that pixel right is not transparent.");
        test.setBorder(1);
        // Now, the pixels should become transparent.
        QCOMPARE(test.getImage().pixelColor(49, 0).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(50, 0).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(49, 99).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(50, 99).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(0, 49).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(0, 50).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(99, 49).alpha(), 0);
        QCOMPARE(test.getImage().pixelColor(99, 50).alpha(), 0);
    }

    void testCache()
    {
        ChromaHueImage test(colorSpace);
        test.setImageSize(50); // Set a non-zero image size
        QVERIFY2(test.m_image.isNull(), "Verify that instantiation is done with empty cache.");
        test.setBorder(5);
        QVERIFY2(test.m_image.isNull(), "Verify that setting border does not trigger re-calculation.");
        test.getImage();
        QVERIFY2(!test.m_image.isNull(), "Verify that getImage() triggers re-calculation.");
        test.setBorder(5);
        QVERIFY2(!test.m_image.isNull(),
                 "Verify that setBorder() does not erease the cache"
                 " if the value that was set is the same than before.");
    }

    void testCornerCases()
    {
        ChromaHueImage test(colorSpace);
        test.setImageSize(50); // Set a non-zero image size
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setBorder(-10);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setBorder(10);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setBorder(25);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setBorder(100);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setBorder(5);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setLightness(-10);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setLightness(0);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setLightness(50);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setLightness(100);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setLightness(150);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setChromaRange(-10);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setChromaRange(0);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setChromaRange(50);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
        test.setChromaRange(300);
        QVERIFY2(!test.getImage().isNull(),
                 "Verify that there is no crash and the returned image is not "
                 "null.");
    }

    void testVeryBigBorder()
    {
        ChromaHueImage test(colorSpace);
        const int myImageSize = 51;
        test.setImageSize(myImageSize); // Set a non-zero image size
        // Set a border that is bigger than half of the image size
        test.setBorder(myImageSize / 2 + 1);
        // The border is so big that the hole image should be transparent.
        for (int x = 0; x < myImageSize; ++x) {
            for (int y = 0; y < myImageSize; ++y) {
                QCOMPARE(test.getImage().pixelColor(x, y).alpha(), 0);
            }
        }
    }

    void testSetLightness_data()
    {
        QTest::addColumn<qreal>("lightness");
        QTest::newRow("10") << 10.;
        QTest::newRow("20") << 20.;
        QTest::newRow("30") << 30.;
        QTest::newRow("40") << 40.;
        QTest::newRow("50") << 50.;
        QTest::newRow("60") << 60.;
        QTest::newRow("70") << 70.;
        QTest::newRow("80") << 80.;
        QTest::newRow("90") << 90.;
    }

    void testSetLightness()
    {
        QFETCH(qreal, lightness);
        ChromaHueImage test(colorSpace);
        constexpr int imageSize = 20;
        test.setImageSize(imageSize); // Set a non-zero image size
        test.setLightness(lightness);
        // Test the lightness. We are using QColor’s simple (non-color-managed)
        // lightness property. Therefore, we allow a tolerance up to 10%.
        QVERIFY2(PerceptualColor::isInRange(lightness * 0.9, test.getImage().pixelColor(imageSize / 2, imageSize / 2).lightnessF() * 100, lightness * 1.1),
                 "Verify that the correct lightness is applied. "
                 "(10% tolerance is allowed.)");
    }

    void testSetLightnessInvalid()
    {
        // Make sure that calling setLightness with invalid values
        // does not crash.
        ChromaHueImage test(colorSpace);
        test.setImageSize(20); // Set a non-zero image size
        test.setLightness(0);
        Q_UNUSED(test.getImage());
        test.setLightness(1);
        Q_UNUSED(test.getImage());
        test.setLightness(2);
        Q_UNUSED(test.getImage());
        test.setLightness(-10);
        Q_UNUSED(test.getImage());
        test.setLightness(-1000);
        Q_UNUSED(test.getImage());
        test.setLightness(100);
        Q_UNUSED(test.getImage());
        test.setLightness(110);
        Q_UNUSED(test.getImage());
        test.setLightness(250);
        Q_UNUSED(test.getImage());
    }

    void testSetChromaRange()
    {
        // Make sure that calling setChromaRange with strange values
        // does not crash.
        ChromaHueImage test(colorSpace);
        test.setImageSize(20); // Set a non-zero image size
        test.setChromaRange(-10);
        Q_UNUSED(test.getImage());
        test.setChromaRange(-1);
        Q_UNUSED(test.getImage());
        test.setChromaRange(0);
        Q_UNUSED(test.getImage());
        test.setChromaRange(1);
        Q_UNUSED(test.getImage());
        test.setChromaRange(10);
        Q_UNUSED(test.getImage());
        test.setChromaRange(100);
        Q_UNUSED(test.getImage());
        test.setChromaRange(1000);
        Q_UNUSED(test.getImage());
        test.setChromaRange(10000);
        Q_UNUSED(test.getImage());
        test.setChromaRange(100000);
        Q_UNUSED(test.getImage());
    }

    void testSizeBorderCombinations()
    {
        // Make sure this code does not crash.
        ChromaHueImage test(colorSpace);
        test.setImageSize(20); // Set a non-zero image size
        test.setBorder(10);    // Set exactly the half of image size as border
        Q_UNUSED(test.getImage());
    }

    void testDevicePixelRatioFForExtremeCases()
    {
        ChromaHueImage test(colorSpace);
        // Testing with a (non-integer) scale factor
        test.setDevicePixelRatioF(1.5);
        // Test with fully transparent image (here, the border is too big
        // for the given image size)
        test.setImageSize(20);
        test.setBorder(30);
        QCOMPARE(test.getImage().devicePixelRatio(), 1.5);
    }

    void testIfGamutIsCenteredCorrectlyOnOddSize()
    {
        ChromaHueImage test(colorSpace);
        test.setBorder(0);
        test.setLightness(50);
        test.setChromaRange(LchValues::srgbMaximumChroma);
        constexpr int oddSize = 101;
        test.setImageSize(oddSize); // an odd numer
        constexpr int positionAtCenter = (oddSize - 1) / 2;
        const qreal chromaAtCenter = colorSpace->toLch(test.getImage().pixelColor(positionAtCenter, positionAtCenter)).c;
        for (int x = positionAtCenter - 2; x <= positionAtCenter + 2; ++x) {
            for (int y = positionAtCenter - 2; y <= positionAtCenter + 2; ++y) {
                if ((x == positionAtCenter) && (y == positionAtCenter)) {
                    continue;
                }
                const qreal chromaAround = colorSpace->toLch(test.getImage().pixelColor(x, y)).c;
                QVERIFY2(chromaAtCenter < chromaAround,
                         "The chroma of the pixel at the center of the image "
                         "is lower than the chroma of any of the pixels "
                         "around.");
            }
        }
    }

    void testIfGamutIsCenteredCorrectlyOnEvenSize()
    {
        ChromaHueImage test(colorSpace);
        test.setBorder(0);
        test.setLightness(50);
        test.setChromaRange(LchValues::srgbMaximumChroma);
        constexpr int evenSize = 100;
        test.setImageSize(evenSize); // an odd numer
        constexpr int positionAtCenter2 = evenSize / 2;
        constexpr int positionAtCenter1 = positionAtCenter2 - 1;
        const qreal chromaAtCenterA = colorSpace->toLch(test.getImage().pixelColor(positionAtCenter1, positionAtCenter1)).c;
        const qreal chromaAtCenterB = colorSpace->toLch(test.getImage().pixelColor(positionAtCenter1, positionAtCenter2)).c;
        const qreal chromaAtCenterC = colorSpace->toLch(test.getImage().pixelColor(positionAtCenter2, positionAtCenter1)).c;
        const qreal chromaAtCenterD = colorSpace->toLch(test.getImage().pixelColor(positionAtCenter2, positionAtCenter2)).c;
        const qreal maximumChromaAtCenter = qMax(qMax(chromaAtCenterA, chromaAtCenterB), qMax(chromaAtCenterC, chromaAtCenterD));
        for (int x = positionAtCenter1 - 2; x <= positionAtCenter2 + 2; ++x) {
            for (int y = positionAtCenter1 - 2; y <= positionAtCenter2 + 2; ++y) {
                if (isInRange(positionAtCenter1, x, positionAtCenter2) && isInRange(positionAtCenter1, y, positionAtCenter2)) {
                    continue;
                }
                const qreal chromaAround = colorSpace->toLch(test.getImage().pixelColor(x, y)).c;
                QVERIFY2(maximumChromaAtCenter < chromaAround,
                         "The chroma of the pixels at the center of the image "
                         "is lower than the chroma of any of the pixels "
                         "around.");
            }
        }
    }

    void testSnippet01()
    {
        TestChromaHueSnippetClass mySnippets;
        mySnippets.testSnippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaHueImage)

// The following “include” is necessary because we do not use a header file:
#include "testchromahueimage.moc"
