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
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaLightnessDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromalightnessdiagram.moc"
