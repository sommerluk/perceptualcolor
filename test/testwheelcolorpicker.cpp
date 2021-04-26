﻿// SPDX-License-Identifier: MIT
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
#include "PerceptualColor/wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h"

#include <QtTest>

#include "rgbcolorspace.h"

namespace PerceptualColor
{
class TestWheelColorPicker : public QObject
{
    Q_OBJECT

public:
    TestWheelColorPicker(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<RgbColorSpace> m_colorSpace {new RgbColorSpace};

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
        // Test for crashs in constructor or destructor
        WheelColorPicker test {m_colorSpace};
    }

    void testCurrentColorProperty()
    {
        WheelColorPicker test {m_colorSpace};
        LchDouble color;
        color.l = 50;
        color.c = 20;
        color.h = 10;
        test.setCurrentColor(color);
        QSignalSpy spy(&test, &WheelColorPicker::currentColorChanged);
        QCOMPARE(spy.count(), 0);

        // Change hue only:
        color.h += 1;
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColor().h, color.h);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.h);

        // Change chroma only:
        color.c += 1;
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColor().c, color.c);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.h);

        // Not changing the color should not trigger the signal
        test.setCurrentColor(color);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColor().c, color.c);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.h);
    }

    void testSizeHints()
    {
        WheelColorPicker test {m_colorSpace};
        QVERIFY(test.minimumSizeHint().width() <= test.sizeHint().width());
        QVERIFY(test.minimumSizeHint().height() <= test.sizeHint().height());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestWheelColorPicker)

// The following “include” is necessary because we do not use a header file:
#include "testwheelcolorpicker.moc"
