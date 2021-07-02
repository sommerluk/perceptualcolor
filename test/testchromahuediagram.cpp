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
#include "include/PerceptualColor/chromahuediagram.h"
// Second, the private implementation.
#include "chromahuediagram_p.h"

#include <QtTest>

#include "PerceptualColor/rgbcolorspacefactory.h"
#include "polarpointf.h"

static void snippet01()
{
    //! [instanciate]
    auto myColorSpace = PerceptualColor::RgbColorSpaceFactory::createSrgb();
    PerceptualColor::ChromaHueDiagram *myDiagram = new PerceptualColor::ChromaHueDiagram(myColorSpace);
    PerceptualColor::LchDouble myColor;
    myColor.h = 270;
    myColor.l = 50;
    myColor.c = 25;
    myDiagram->setCurrentColor(myColor);
    myDiagram->show();
    //! [instanciate]
    delete myDiagram;
}

namespace PerceptualColor
{
class TestChromaHueDiagram : public QObject
{
    Q_OBJECT

public:
    TestChromaHueDiagram(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

    bool isEqual(const LchDouble &first, const LchDouble &second)
    {
        return ((first.l == second.l) && (first.c == second.c) && (first.h == second.h));
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

    void testConstructorAndDestructor()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
    }

    void testShow()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show();
    }

    void testKeyPressEvent()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        LchDouble referenceColorLch;
        referenceColorLch.l = 50;
        referenceColorLch.c = 0;
        referenceColorLch.h = 180;
        myDiagram.setCurrentColor(referenceColorLch);
        if (myDiagram.currentColor().h != 180) {
            throw;
        }
        if (myDiagram.currentColor().c != 0) {
            throw;
        }
        LchDouble referenceColorChromaLch;
        referenceColorChromaLch.l = 50;
        referenceColorChromaLch.c = 10;
        referenceColorChromaLch.h = 180;
        myDiagram.setCurrentColor(referenceColorChromaLch);

        QKeyEvent myEvent {QEvent::KeyPress, 0, Qt::NoModifier}; // TODO Use QTest::keyClick() instead!

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c > 0, "Test Key_Up");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c > 0, "Test Key_PageUp");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c < 10, "Test Key_Down");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c < 10, "Test Key_PageDown");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c >= 0, "Test Key_Down never negative");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().c >= 0, "Test Key_PageDown never negative");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().h > 180, "Test Key_Left");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().h > 180, "Test Key_Home");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().h < 180, "Test Key_Right");

        myDiagram.setCurrentColor(referenceColorChromaLch);
        myEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(myDiagram.currentColor().h < 180, "Test Key_End");
    }

    void testMinimumSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(myDiagram.minimumSizeHint().width() > 0, "minimalSizeHint width is implemented.");
        QVERIFY2(myDiagram.minimumSizeHint().height() > 0, "minimalSizeHint height is implemented.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(myDiagram.sizeHint().width() > myDiagram.minimumSizeHint().width(), "sizeHint width is bigger than minimalSizeHint width.");
        QVERIFY2(myDiagram.sizeHint().height() > myDiagram.minimumSizeHint().height(), "sizeHint height is bigger than minimalSizeHint height.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testColorProperty()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QSignalSpy mySpy(&myDiagram, &PerceptualColor::ChromaHueDiagram::currentColorChanged);
        LchDouble referenceColorLch;
        referenceColorLch.l = 50;
        referenceColorLch.c = 10;
        referenceColorLch.h = 180;

        // Test if signal for new color is emitted.
        myDiagram.setCurrentColor(referenceColorLch);
        QCOMPARE(mySpy.count(), 1);
        QVERIFY2(isEqual(myDiagram.currentColor(), referenceColorLch), "Verify that the color is equal to the reference color.");

        // Test that no signal is emitted for old color.
        myDiagram.setCurrentColor(referenceColorLch);
        QCOMPARE(mySpy.count(), 1);
        QVERIFY2(isEqual(myDiagram.currentColor(), referenceColorLch), "Verify that the color is equal to the reference color.");
    }

    void testDiagramOffset()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show(); // Necessary to allow event processing
        myDiagram.resize(50, 50);
        qreal oldOffset = myDiagram.d_pointer->diagramOffset();
        myDiagram.resize(100, 100);
        QVERIFY2(myDiagram.d_pointer->diagramOffset() > oldOffset,
                 "Verify that the offset at widget size 150 is bigger "
                 "than at widget size 100.");
    }

    void testdiagramCenter()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.resize(100, 100);
        // Test conformance with diagramOffset()
        QCOMPARE(myDiagram.d_pointer->diagramCenter().x(), myDiagram.d_pointer->diagramOffset());
        QCOMPARE(myDiagram.d_pointer->diagramCenter().y(), myDiagram.d_pointer->diagramOffset());
    }

    void testConversions()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        LchDouble myGrayColor;
        myGrayColor.h = 0;
        myGrayColor.l = 50;
        myGrayColor.c = 0;
        myDiagram.setCurrentColor(myGrayColor);
        myDiagram.show(); // Necessary to make sure resize events are processed
        constexpr int widgetSize = 300;
        myDiagram.resize(widgetSize, widgetSize);
        QCOMPARE(myDiagram.size(), QSize(widgetSize, widgetSize));
        // Chose a position near to, but different from the center.
        constexpr int testPosition = widgetSize / 2 + 10;
        myDiagram.d_pointer->setColorFromWidgetPixelPosition(QPoint(testPosition, testPosition));
        QCOMPARE(myDiagram.d_pointer->m_currentColor.l, myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).L);
        QCOMPARE(PolarPointF(myDiagram.d_pointer->m_currentColor.c, myDiagram.d_pointer->m_currentColor.h).toCartesian().x(), myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).a);
        QCOMPARE(PolarPointF(myDiagram.d_pointer->m_currentColor.c, myDiagram.d_pointer->m_currentColor.h).toCartesian().y(), myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).b);
        QCOMPARE(myDiagram.d_pointer->widgetCoordinatesFromCurrentColor(), QPoint(testPosition, testPosition) + QPointF(0.5, 0.5));
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in inmediate
        // re-paint.
        ChromaHueDiagram myWidget {m_rgbColorSpace};
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

    void testOutOfGamutColors()
    {
        ChromaHueDiagram myWidget {m_rgbColorSpace};
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
        ChromaHueDiagram myWidget {m_rgbColorSpace};
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

    void testSnipped01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaHueDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromahuediagram.moc"
