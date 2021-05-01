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
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace {new RgbColorSpace()};

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
        myLineEdit->setFocus();
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
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorWheel)

// The following “include” is necessary because we do not use a header file:
#include "testcolorwheel.moc"
