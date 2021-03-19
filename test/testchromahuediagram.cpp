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

#include "perceptualcolorlib_internal.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "include/PerceptualColor/chromahuediagram.h"
#include "chromahuediagram_p.h"

#include <QtTest>

#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/polarpointf.h"

namespace PerceptualColor {

class TestChromaHueDiagram : public QObject
{
    Q_OBJECT

public:
    TestChromaHueDiagram(QObject *parent = nullptr) : QObject(parent) {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace {
        new RgbColorSpace()
    };

    bool isEqual(const LchDouble &first, const LchDouble &second) {
        return (
            (first.l == second.l)
                && (first.c == second.c)
                && (first.h == second.h)
        );
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

    void testConstructorAndDestructor() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
    }

    void testShow() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show();
    }

    void testKeyPressEvent() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        LchDouble referenceColorLch;
        referenceColorLch.l = 50;
        referenceColorLch.c = 0;
        referenceColorLch.h = 180;
        FullColorDescription referenceColor = FullColorDescription(
            m_rgbColorSpace,
            referenceColorLch,
            FullColorDescription::OutOfGamutBehaviour::sacrifyChroma
        );
        myDiagram.setColor(referenceColorLch);
        if (myDiagram.color().h != 180) {
            throw;
        }
        if (myDiagram.color().c != 0) {
            throw;
        }
        LchDouble referenceColorChromaLch;
        referenceColorChromaLch.l = 50;
        referenceColorChromaLch.c = 10;
        referenceColorChromaLch.h = 180;
        myDiagram.setColor(referenceColorChromaLch);

        QKeyEvent myEvent { QEvent::None, 0, Qt::NoModifier };

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Up,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c > 0,
            "Test Key_Up"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageUp,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c > 0,
            "Test Key_PageUp"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Down,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c < 10,
            "Test Key_Down"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageDown,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c < 10,
            "Test Key_PageDown"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Down,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c >= 0,
            "Test Key_Down never negative"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageDown,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().c >= 0,
            "Test Key_PageDown never negative"
        );


        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Left,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().h > 180,
            "Test Key_Left"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Home,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().h > 180,
            "Test Key_Home"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Right,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().h < 180,
            "Test Key_Right"
        );

        myDiagram.setColor(referenceColorChromaLch);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_End,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().h < 180,
            "Test Key_End"
        );

    }

    void testMinimalSizeHint() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(
            myDiagram.minimumSizeHint().width() > 0,
            "minimalSizeHint width is implemented."
        );
        QVERIFY2(
            myDiagram.minimumSizeHint().height() > 0,
            "minimalSizeHint height is implemented."
        );
    }

    void testSizeHint() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(
            myDiagram.sizeHint().width()
                > myDiagram.minimumSizeHint().width(),
            "minimalSizeHint width is implemented."
        );
        QVERIFY2(
            myDiagram.sizeHint().height()
                > myDiagram.minimumSizeHint().height(),
            "minimalSizeHint height is implemented."
        );
    }

    void testColorProperty() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QSignalSpy spyPerceptualDialog(
            &myDiagram,
            &PerceptualColor::ChromaHueDiagram::colorChanged
        );
        LchDouble referenceColorLch;
        referenceColorLch.l = 50;
        referenceColorLch.c = 10;
        referenceColorLch.h = 180;
        FullColorDescription referenceColor = FullColorDescription(
            m_rgbColorSpace,
            referenceColorLch,
            FullColorDescription::OutOfGamutBehaviour::sacrifyChroma
        );

        // Test if signal for new color is emitted.
        myDiagram.setColor(referenceColorLch);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QVERIFY2(
            isEqual(myDiagram.color(), referenceColorLch),
            "Verify that the diagram’s color is equal to the reference color."
        );

        // Test that no signal is emitted for old color.
        myDiagram.setColor(referenceColorLch);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QVERIFY2(
            isEqual(myDiagram.color(), referenceColorLch),
            "Verify that the diagram’s color is equal to the reference color."
        );
    }

    void testDiagramOffset() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show(); // Necessary to allow event processing
        myDiagram.resize(50, 50);
        qreal oldOffset = myDiagram.d_pointer->diagramOffset();
        myDiagram.resize(100, 100);
        QVERIFY2(
            myDiagram.d_pointer->diagramOffset() > oldOffset,
            "Verify that the offset at widget size 150 is bigger "
                "than at widget size 100."
        );
    }

    void testDiagramCenterInWidgetCoordinates() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.resize(100, 100);
        // Test conformance with diagramOffset()
        QCOMPARE(
            myDiagram.d_pointer->diagramCenterInWidgetCoordinates().x(),
            myDiagram.d_pointer->diagramOffset()
        );
        QCOMPARE(
            myDiagram.d_pointer->diagramCenterInWidgetCoordinates().y(),
            myDiagram.d_pointer->diagramOffset()
        );
    }

    void testConversions() {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        LchDouble myGrayColor;
        myGrayColor.h = 0;
        myGrayColor.l = 50;
        myGrayColor.c = 0;
        myDiagram.setColor(myGrayColor);
        myDiagram.show(); // Necessary to make sure resize events are processed
        constexpr int widgetSize = 300;
        myDiagram.resize(widgetSize, widgetSize);
        QCOMPARE(
            myDiagram.size(),
            QSize(widgetSize, widgetSize)
        );
        // Chose a position near to, but different from the center.
        constexpr int testPosition = widgetSize / 2 + 10;
        myDiagram.d_pointer->setColorFromWidgetPixelPosition(
            QPoint(testPosition, testPosition)
        );
        QCOMPARE(
            myDiagram.d_pointer->m_color.l,
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(
                QPoint(testPosition, testPosition)
            ).L
        );
        QCOMPARE(
            PolarPointF(
                myDiagram.d_pointer->m_color.c,
                myDiagram.d_pointer->m_color.h
            ).toCartesian().x(),
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(
                QPoint(testPosition, testPosition)
            ).a
        );
        QCOMPARE(
            PolarPointF(
                myDiagram.d_pointer->m_color.c,
                myDiagram.d_pointer->m_color.h
            ).toCartesian().y(),
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(
                QPoint(testPosition, testPosition)
            ).b
        );
        QCOMPARE(
            myDiagram.d_pointer->widgetCoordinatesFromCurrentColor(),
            QPoint(testPosition, testPosition) + QPointF(0.5, 0.5)
        );
    }

void testSnipped1() {
//! [ChromaHueDiagram Instanciate]
QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace {
    new PerceptualColor::RgbColorSpace()
};
PerceptualColor::ChromaHueDiagram *myDiagram =
    new PerceptualColor::ChromaHueDiagram(myColorSpace);
LchDouble myColor;
myColor.h = 270;
myColor.l = 50;
myColor.c = 25;
myDiagram->setColor(myColor);
myDiagram->show();
//! [ChromaHueDiagram Instanciate]
delete myDiagram;
}

};

}

QTEST_MAIN(PerceptualColor::TestChromaHueDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromahuediagram.moc"
