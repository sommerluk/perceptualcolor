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
#include "chromalightnessdiagram.h"
// Second, the private implementation.
#include "chromalightnessdiagram_p.h"

#include "PerceptualColor/rgbcolorspacefactory.h"

#include <QtTest>

namespace PerceptualColor
{
class TestChromaLightnessDiagram : public QObject
{
    Q_OBJECT

public:
    TestChromaLightnessDiagram(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

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
        ChromaLightnessDiagram test(m_rgbColorSpace);
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in inmediate
        // re-paint.
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize());
        myWidget.repaint();
        myWidget.resize(QSize(-1, -1));
        myWidget.repaint();
        myWidget.resize(QSize(-1, 0));
        myWidget.repaint();
        myWidget.resize(QSize(0, -1));
        myWidget.repaint();
        myWidget.resize(QSize(0, 1));
        myWidget.repaint();
        myWidget.resize(QSize(1, 0));
        myWidget.repaint();
        myWidget.resize(QSize(1, 1));
        myWidget.repaint();
        myWidget.resize(QSize(2, 2));
        myWidget.repaint();
        myWidget.resize(QSize(3, 3));
        myWidget.repaint();
        myWidget.resize(QSize(4, 4));
        myWidget.repaint();
        myWidget.resize(QSize(5, 5));
        myWidget.repaint();
        myWidget.resize(QSize(6, 6));
        myWidget.repaint();
        myWidget.resize(QSize(7, 7));
        myWidget.repaint();
        myWidget.resize(QSize(8, 8));
        myWidget.repaint();
        myWidget.resize(QSize(9, 9));
        myWidget.repaint();
        myWidget.resize(QSize(10, 10));
        myWidget.repaint();
        myWidget.resize(QSize(11, 11));
        myWidget.repaint();
        myWidget.resize(QSize(12, 12));
        myWidget.repaint();
        myWidget.resize(QSize(13, 13));
        myWidget.repaint();
        myWidget.resize(QSize(14, 14));
        myWidget.repaint();
    }

    void testSetCurrentColorFromWidgetPixelPosition1()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because if the widget is too small, there
        // is no place for a diagram, and some value conversions are
        // diagram-based..
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        const QPoint positive(10, 20);
        const QPoint negative(-10, -20);
        myWidget.resize(QSize(1, 1));
        // Executing the following lines should not crash!
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(positive);
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(negative);
    }

    void testSetCurrentColorFromWidgetPixelPosition2()
    {
        // Test this function for out-of-gamut positions
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        constexpr int size = 100;
        myWidget.resize(size, size);
        LchDouble color;

        // Test for top-left corner
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // Same x and y spacing from top-left corner
            QPoint(size * (-1), size * (-1)));
        color = myWidget.currentColor();
        QCOMPARE(color.l, 100);
        QCOMPARE(color.c, 0);

        // Test for bottom-left corner
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // Same x and y spacing from bottom-left corner
            QPoint(size * (-1), size * 2));
        color = myWidget.currentColor();
        QCOMPARE(color.l, 0);
        QCOMPARE(color.c, 0);

        // Test for middle-right position
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // x position far from diagram boundaries, y position in the middle
            QPoint(size * 10, size * 50 / 100));
        color = myWidget.currentColor();
        // Lightness should be somewhere in the middle.
        QVERIFY(color.l > 10);
        QVERIFY(color.l < 90);
        // At least 25 should be possible on all hues.
        QVERIFY(color.c > 25);
    }

    void testDefaultBorderPhysical()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= 0);
    }

    void testLeftBorderPhysical()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= 0);
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= myWidget.d_pointer->defaultBorderPhysical());
    }

    void testCalculateImageSizePhysical()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes.
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.resize(QSize());
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(-1, -1));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(-1, 0));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(0, -1));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(0, 1));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(1, 0));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
        myWidget.resize(QSize(1, 1));
        myWidget.d_pointer->calculateImageSizePhysical(); // Should not crash
    }

    void testFromWidgetPixelPositionToColor()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes.
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        const QPoint positive(10, 20);
        const QPoint negative(-10, -20);
        myWidget.resize(QSize());
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(-1, -1));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(-1, 0));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(0, -1));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(0, 1));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(1, 0));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(1, 1));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(2, 2));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(3, 3));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(4, 4));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(5, 5));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(6, 6));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(7, 7));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(8, 8));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(9, 9));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(10, 10));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(11, 11));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(12, 12));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(13, 13));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
        myWidget.resize(QSize(14, 14));
        myWidget.d_pointer->fromWidgetPixelPositionToColor(positive);
        myWidget.d_pointer->fromWidgetPixelPositionToColor(negative);
    }

    void testMouseSupport1()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(2, 2);
        // Mouse movements should not crash when the size of the widget is
        // too small to show a diagram:
        QTest::mousePress(&myWidget, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(0, 0));
        QTest::mouseMove(&myWidget, QPoint(1, 1));
        QTest::mouseRelease(&myWidget, Qt::MouseButton::LeftButton);
    }

    void testMouseSupport2()
    {
        // Test reactions to mouse events when moving out-of-gamut
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        constexpr int size = 100;
        myWidget.resize(size, size);
        LchDouble color;

        // Test for top-left corner
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * (-1), size * (-1)));
        // Test if the widget value is really the nearest in-gamut color
        color = myWidget.currentColor();
        QCOMPARE(color.l, 100);
        QCOMPARE(color.c, 0);

        // Test for bottom-left corner
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * (-1), size * 2));
        // Test if the widget value is really the nearest in-gamut color
        color = myWidget.currentColor();
        QCOMPARE(color.l, 0);
        QCOMPARE(color.c, 0);

        // Test for middle-right position
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * 10, size * 50 / 100));
        // Test if the widget value is really the nearest in-gamut color
        color = myWidget.currentColor();
        // Lightness should be somewhere in the middle.
        QVERIFY(color.l > 10);
        QVERIFY(color.l < 90);
        // At least 25 should be possible on all hues.
        QVERIFY(color.c > 25);
    }

    void testPaintEventNormalSize()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        // Test normal size
        myWidget.resize(100, 100);
        myWidget.repaint();
    }

    void testPaintEventTooSmallSize()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        // Test small size (too small to show a diagram)
        myWidget.resize(2, 2);
        myWidget.repaint();
    }

    void testPaintEventEmptySize()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        // Test empty size
        myWidget.resize(0, 0);
        myWidget.repaint();
    }

    void testKeyPressEvent()
    {
        ChromaLightnessDiagram myDiagram(m_rgbColorSpace);
        LchDouble referenceColorLch;
        referenceColorLch.l = 50;
        referenceColorLch.c = 20;
        referenceColorLch.h = 180;
        myDiagram.setCurrentColor(referenceColorLch);

        // Assert pre-conditions:

        QCOMPARE(myDiagram.currentColor().l, 50);
        QCOMPARE(myDiagram.currentColor().c, 20);
        QCOMPARE(myDiagram.currentColor().h, 180);

        // Actual test:

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Left);
        QVERIFY(myDiagram.currentColor().l == referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c < referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Right);
        QVERIFY(myDiagram.currentColor().l == referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c > referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Up);
        QVERIFY(myDiagram.currentColor().l > referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Down);
        QVERIFY(myDiagram.currentColor().l < referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Home);
        QVERIFY(myDiagram.currentColor().l == referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c > referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_End);
        QVERIFY(myDiagram.currentColor().l == referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c < referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageUp);
        QVERIFY(myDiagram.currentColor().l > referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageDown);
        QVERIFY(myDiagram.currentColor().l < referenceColorLch.l);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        referenceColorLch.c = 0;

        // Chroma should never become negative

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Left);
        QCOMPARE(myDiagram.currentColor().l, referenceColorLch.l);
        QCOMPARE(myDiagram.currentColor().c, referenceColorLch.c);
        QCOMPARE(myDiagram.currentColor().h, referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_End);
        QCOMPARE(myDiagram.currentColor().l, referenceColorLch.l);
        QCOMPARE(myDiagram.currentColor().c, referenceColorLch.c);
        QCOMPARE(myDiagram.currentColor().h, referenceColorLch.h);

        referenceColorLch.l = 0;

        // Lightness should never be smallen than 0.

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Down);
        QVERIFY(myDiagram.currentColor().l >= 0);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageDown);
        QVERIFY(myDiagram.currentColor().l >= 0);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        referenceColorLch.l = 100;

        // Lightness should never be bigger than 100.

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Up);
        QVERIFY(myDiagram.currentColor().l <= 100);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);

        myDiagram.setCurrentColor(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageUp);
        QVERIFY(myDiagram.currentColor().l <= 100);
        QVERIFY(myDiagram.currentColor().c == referenceColorLch.c);
        QVERIFY(myDiagram.currentColor().h == referenceColorLch.h);
    }

    void testIsWidgetPixelPositionInGamut()
    {
        ChromaLightnessDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show();
        myDiagram.resize(QSize(2, 2));
        // On very small widget sizes, no diagram is visible. Therefore,
        // no pixel should be in-gamut.
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 2)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 2)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 2)));
    }

    void testCurrentColorProperty()
    {
        ChromaLightnessDiagram test {m_rgbColorSpace};
        LchDouble color;
        color.l = 50;
        color.c = 20;
        color.h = 10;
        test.setCurrentColor(color);
        QVERIFY(test.currentColor().hasSameCoordinates(color));
        QSignalSpy spy(&test, &ChromaLightnessDiagram::currentColorChanged);
        QCOMPARE(spy.count(), 0);

        // Change hue only:
        color.h += 1;
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 1);
        QVERIFY(test.currentColor().hasSameCoordinates(color));

        // Change chroma only:
        color.c += 1;
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 2);
        QVERIFY(test.currentColor().hasSameCoordinates(color));

        // Change lightness only:
        color.l += 1;
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 3);
        QVERIFY(test.currentColor().hasSameCoordinates(color));

        // Not changing the color should not trigger the signal
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 3);
        QVERIFY(test.currentColor().hasSameCoordinates(color));
    }

    void testResizeEvent()
    {
        ChromaLightnessDiagram test {m_rgbColorSpace};
        test.show();
        // Resize events should not crash!
        test.resize(QSize(100, 100)); // normal size
        test.resize(QSize(2, 2));     // very small size
        test.resize(QSize(0, 0));     // empty size
        test.resize(QSize(-1, -1));   // invalid size
    }

    void testSizeHintAndMinimumSizeHint()
    {
        ChromaLightnessDiagram test {m_rgbColorSpace};
        test.show();
        QVERIFY(test.minimumSizeHint().width() >= 0);
        QVERIFY(test.minimumSizeHint().height() >= 0);
        QVERIFY(test.sizeHint().width() >= test.minimumSizeHint().width());
        QVERIFY(test.sizeHint().height() >= test.minimumSizeHint().height());
    }

    void testOutOfGamutColors()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting out-of-gamut colors works

        const LchDouble myFirstColor(100, 150, 0);
        myWidget.setCurrentColor(myFirstColor);
        QVERIFY(myFirstColor.hasSameCoordinates(myWidget.currentColor()));
        QVERIFY(myFirstColor.hasSameCoordinates(myWidget.d_pointer->m_currentColor));

        const LchDouble mySecondColor(0, 150, 0);
        myWidget.setCurrentColor(mySecondColor);
        QVERIFY(mySecondColor.hasSameCoordinates(myWidget.currentColor()));
        QVERIFY(mySecondColor.hasSameCoordinates(myWidget.d_pointer->m_currentColor));
    }

    void testOutOfRange()
    {
        ChromaLightnessDiagram myWidget {m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting colors, that are not only out-of-gamut colors
        // but also out of a reasonable range, works.

        const LchDouble myFirstColor(300, 550, -10);
        myWidget.setCurrentColor(myFirstColor);
        QVERIFY(myFirstColor.hasSameCoordinates(myWidget.currentColor()));
        QVERIFY(myFirstColor.hasSameCoordinates(myWidget.d_pointer->m_currentColor));

        const LchDouble mySecondColor(-100, -150, 890);
        myWidget.setCurrentColor(mySecondColor);
        QVERIFY(mySecondColor.hasSameCoordinates(myWidget.currentColor()));
        QVERIFY(mySecondColor.hasSameCoordinates(myWidget.d_pointer->m_currentColor));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaLightnessDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromalightnessdiagram.moc"
