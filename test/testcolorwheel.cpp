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
#include "PerceptualColor/colorwheel.h"
// Second, the private implementation.
#include "colorwheel_p.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QtTest>

#include "helper.h"

namespace PerceptualColor
{
class TestColorWheel : public QObject
{
    Q_OBJECT

public:
    TestColorWheel(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpace::createSrgb();

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
        ColorWheel temp(m_rgbColorSpace);
    }

    void testMouseFocusBehaviour()
    {
        QWidget myWindow;
        QHBoxLayout *myLayout = new QHBoxLayout;
        QLineEdit *myLineEdit = new QLineEdit;
        myLayout->addWidget(myLineEdit);
        ColorWheel *myColorWheel = new ColorWheel(m_rgbColorSpace);
        myLayout->addWidget(myColorWheel);
        myWindow.setLayout(myLayout);

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests:
        myWindow.show();
        QApplication::setActiveWindow(&myWindow);

        myLineEdit->setFocus();
        QCOMPARE(myLineEdit->hasFocus(), true);
        QCOMPARE(myColorWheel->hasFocus(), false);

        // A click that is not in the wheel should not give focus:
        QTest::mouseClick(
            // Widget to click
            myColorWheel,
            // Mouse button to simulate
            Qt::MouseButton::LeftButton,
            // Modifiers
            Qt::KeyboardModifier::NoModifier,
            // Position for the mouse click
            QPoint(10, 10));
        QCOMPARE(myColorWheel->hasFocus(), false);
        // A click in the middle should give focus:
        QTest::mouseClick(
            // Widget to click
            myColorWheel,
            // Mouse button to simulate
            Qt::MouseButton::LeftButton,
            // Modifiers
            Qt::KeyboardModifier::NoModifier
            // Not specifying a point means: click in the center of the widget.
        );
        QCOMPARE(myColorWheel->hasFocus(), true);
    }

    void testHueProperty()
    {
        ColorWheel myWheel(m_rgbColorSpace);
        QSignalSpy mySpy(&myWheel, &ColorWheel::hueChanged);
        qreal referenceHue = 12.345;

        // Test if signal is emitted.
        myWheel.setHue(referenceHue);
        QCOMPARE(mySpy.count(), 1);
        QCOMPARE(myWheel.hue(), referenceHue);

        // Test that no signal is emitted for old hue.
        myWheel.setHue(referenceHue);
        QCOMPARE(mySpy.count(), 1);
        QCOMPARE(myWheel.hue(), referenceHue);
    }

    void testMinimumSizeHint()
    {
        ColorWheel myColorWheel(m_rgbColorSpace);
        QVERIFY2(myColorWheel.minimumSizeHint().width() > 0, "minimalSizeHint width is implemented.");
        QVERIFY2(myColorWheel.minimumSizeHint().height() > 0, "minimalSizeHint height is implemented.");
        // Check that the hint is a square:
        QCOMPARE(myColorWheel.minimumSizeHint().width(), myColorWheel.minimumSizeHint().height());
    }

    void testSizeHint()
    {
        ColorWheel myColorWheel(m_rgbColorSpace);
        QVERIFY2(myColorWheel.sizeHint().width() > myColorWheel.minimumSizeHint().width(), "sizeHint width is bigger than minimalSizeHint width.");
        QVERIFY2(myColorWheel.sizeHint().height() > myColorWheel.minimumSizeHint().height(), "sizeHint height is bigger than minimalSizeHint height.");
        // Check that the hint is a square:
        QCOMPARE(myColorWheel.minimumSizeHint().width(), myColorWheel.minimumSizeHint().height());
    }

    void testBorder()
    {
        ColorWheel myColorWheel(m_rgbColorSpace);
        QVERIFY2(myColorWheel.d_pointer->border() > 0, "border() is a valid value > 0.");
    }

    void testInnerDiameter()
    {
        ColorWheel myColorWheel(m_rgbColorSpace);
        QVERIFY2(myColorWheel.d_pointer->innerDiameter() > 0, "innerDiameter() is a valid value > 0.");
        QVERIFY2(myColorWheel.d_pointer->innerDiameter() < myColorWheel.size().width(), "innerDiameter() is smaller than the widget’s width.");
        QVERIFY2(myColorWheel.d_pointer->innerDiameter() < myColorWheel.size().height(), "innerDiameter() is smaller than the widget’s height.");
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in inmediate
        // re-paint.
        ColorWheel myWidget {m_rgbColorSpace};
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

    void testOutOfRange()
    {
        ColorWheel myWidget {m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test setting out-of-range hues

        const qreal hue = 500;
        myWidget.setHue(hue);
        // Out-of-range hues should initially be preserved.
        QCOMPARE(myWidget.hue(), 500);
        // After user interaction, they should be normalized.
        QTest::keyClick(&myWidget, Qt::Key_Plus);
        QVERIFY(isInRange<qreal>(0, myWidget.hue(), 360));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorWheel)

// The following “include” is necessary because we do not use a header file:
#include "testcolorwheel.moc"
