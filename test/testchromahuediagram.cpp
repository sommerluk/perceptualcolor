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
#include "include/PerceptualColor/chromahuediagram.h"

#include <QtTest>

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
        cmsCIELCh referenceColorLch;
        referenceColorLch.L = 50;
        referenceColorLch.C = 0;
        referenceColorLch.h = 180;
        FullColorDescription referenceColor = FullColorDescription(
            m_rgbColorSpace,
            referenceColorLch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        );
        myDiagram.setColor(referenceColor);
        if (myDiagram.color().toLch().h != 180) {
            throw;
        }
        if (myDiagram.color().toLch().C != 0) {
            throw;
        }
        cmsCIELCh referenceColorChromaLch;
        referenceColorChromaLch.L = 50;
        referenceColorChromaLch.C = 10;
        referenceColorChromaLch.h = 180;
        FullColorDescription referenceColorChroma = FullColorDescription(
            m_rgbColorSpace,
            referenceColorChromaLch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        );
        myDiagram.setColor(referenceColorChroma);
        if (myDiagram.color().toLch().h != 180) {
            throw;
        }
        if (myDiagram.color().toLch().C != 10) {
            throw;
        }
        
        QKeyEvent myEvent { QEvent::None, 0, Qt::NoModifier };

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Up,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C > 0,
            "Test Key_Up"
        );

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageUp,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C > 0,
            "Test Key_PageUp"
        );
      
        myDiagram.setColor(referenceColorChroma);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Down,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C < 10,
            "Test Key_Down"
        );

        myDiagram.setColor(referenceColorChroma);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageDown,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C < 10,
            "Test Key_PageDown"
        );

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Down,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C == 0,
            "Test Key_Down never negative"
        );

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_PageDown,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().C == 0,
            "Test Key_PageDown never negative"
        );


        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Left,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().h > 180,
            "Test Key_Left"
        );

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Home,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().h > 180,
            "Test Key_Home"
        );
      
        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_Right,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().h < 180,
            "Test Key_Right"
        );

        myDiagram.setColor(referenceColor);
        myEvent = QKeyEvent(
            QEvent::KeyPress,
            Qt::Key_End,
            Qt::NoModifier
        );
        myDiagram.keyPressEvent(&myEvent);
        QVERIFY2(
            myDiagram.color().toLch().h < 180,
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
        QVERIFY2(
            myDiagram.color().isValid(),
            "Initialization with a valid value."
        );
        QSignalSpy spyPerceptualDialog(
            &myDiagram,
            &PerceptualColor::ChromaHueDiagram::colorChanged
        );
        cmsCIELCh referenceColorLch;
        referenceColorLch.L = 50;
        referenceColorLch.C = 10;
        referenceColorLch.h = 180;
        FullColorDescription referenceColor = FullColorDescription(
            m_rgbColorSpace,
            referenceColorLch,
            FullColorDescription::outOfGamutBehaviour::sacrifyChroma
        );
        
        // Test if signal for new color is emitted.
        myDiagram.setColor(referenceColor);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(
            myDiagram.color().toRgbQColor(),
            referenceColor.toRgbQColor()
        );
        
        // Test that no signal is emitted for old color.
        myDiagram.setColor(referenceColor);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(
            myDiagram.color().toRgbQColor(),
            referenceColor.toRgbQColor()
        );
        
        // Test using an invalid FullColorDescription
        myDiagram.setColor(FullColorDescription());
        QCOMPARE(
            myDiagram.color().toRgbQColor(),
            referenceColor.toRgbQColor()
        );
        QVERIFY2(
            spyPerceptualDialog.count() == 1,
            "No signal was emitted when the invalid color assignment "
                "had been ignored."
        );
    }

};

}

QTEST_MAIN(PerceptualColor::TestChromaHueDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromahuediagram.moc" 
