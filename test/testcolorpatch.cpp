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

#include "perceptualcolorlib_internal.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/colorpatch.h"
#include "colorpatch_p.h"

#include <QtTest>

static void snippet01() {
//! [ColorPatch Create widget]
PerceptualColor::ColorPatch *m_patch = new PerceptualColor::ColorPatch;
m_patch->setColor(Qt::blue);
//! [ColorPatch Create widget]
//! [ColorPatch Bigger minimum size]
m_patch->setMinimumSize(QSize(50, 50));
//! [ColorPatch Bigger minimum size]
QCOMPARE(m_patch->color(), QColor(Qt::blue));
delete m_patch;
}


namespace PerceptualColor {

class TestColorPatch : public QObject
{
    Q_OBJECT

public:
    TestColorPatch(QObject *parent = nullptr) : QObject(parent) {
    }

private:

    static void voidMessageHandler(
        QtMsgType,
        const QMessageLogContext &,
        const QString &
    ) {
        // dummy message handler that does not print messages
    }

    void helperProvideQColors() {
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);

        QTest::addColumn<QColor>("color");
        QTest::newRow("RGB 1 2 3") << QColor(1, 2, 3);
        QTest::newRow("RGBA 1 2 3 4") << QColor(1, 2, 3, 4);
        QTest::newRow("RGB 1 2 300") << QColor(1, 2, 300);
        QTest::newRow("RGBA 1 2 300 4") << QColor(1, 2, 300, 4);

        QTest::newRow("RGB 0.1 0.2 0.3") << QColor::fromRgbF(0.1, 0.2, 0.3);
        QTest::newRow("RGBA 0.1 0.2 0.3 0.4")
            << QColor::fromRgbF(0.1, 0.2, 0.3, 0.4);
        QTest::newRow("RGB 0.1 6.2 0.300")
            << QColor::fromRgbF(0.1, 6.2, 0.300);
        QTest::newRow("RGBA 0.1 6.2 0.300 0.4")
            << QColor::fromRgbF(0.1, 6.2, 0.300, 0.4);

        QTest::newRow("CMYK 1 2 3 4") << QColor::fromCmyk(1, 2, 3, 4);
        QTest::newRow("CMYK 1 2 3 4 5") << QColor::fromCmyk(1, 2, 3, 4, 5);
        QTest::newRow("CMYK 1 2 300 4") << QColor::fromCmyk(1, 2, 300, 4);
        QTest::newRow("CMYK 1 2 300 4 5")
            << QColor::fromCmyk(1, 2, 300, 4, 5);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4")
            << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4 0.6495217645")
            << QColor::fromCmykF(0.1, 0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4")
            << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 0.6495217645")
            << QColor::fromCmykF(0.1, 6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("HSL 2 3 4") << QColor::fromHsl(2, 3, 4);
        QTest::newRow("HSL 2 3 4 5") << QColor::fromHsl(2, 3, 4, 5);
        QTest::newRow("HSL 2 300 4") << QColor::fromHsl(2, 300, 4);
        QTest::newRow("HSL 2 300 4 5") << QColor::fromHsl(2, 300, 4, 5);
        QTest::newRow("HSL 0.2 0.300 0.4")
            << QColor::fromHslF(0.2, 0.300, 0.4);
        QTest::newRow("HSL 0.2 0.300 0.4 0.6495217645")
            << QColor::fromHslF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSL 6.2 0.300 0.4")
            << QColor::fromHslF(6.2, 0.300, 0.4);
        QTest::newRow("HSL 6.2 0.300 0.4 0.6495217645")
            << QColor::fromHslF(6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("HSV 2 3 4") << QColor::fromHsv(2, 3, 4);
        QTest::newRow("HSV 2 3 4 5") << QColor::fromHsv(2, 3, 4, 5);
        QTest::newRow("HSV 2 300 4") << QColor::fromHsv(2, 300, 4);
        QTest::newRow("HSV 2 300 4 5") << QColor::fromHsv(2, 300, 4, 5);
        QTest::newRow("HSV 0.2 0.300 0.4")
        << QColor::fromHsvF(0.2, 0.300, 0.4);
        QTest::newRow("HSV 0.2 0.300 0.4 0.6495217645")
            << QColor::fromHsvF(0.2, 0.300, 0.4, 0.6495217645);
        QTest::newRow("HSV 6.2 0.300 0.4")
            << QColor::fromHsvF(6.2, 0.300, 0.4);
        QTest::newRow("HSV 6.2 0.300 0.4 0.6495217645")
            << QColor::fromHsvF(6.2, 0.300, 0.4, 0.6495217645);

        QTest::newRow("invalid") << QColor();

        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    QColor m_color;

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

    void testInitialazation() {
        PerceptualColor::ColorPatch thePatch;
        // Test initial value (an invalid color following the documentation)
        QCOMPARE(thePatch.color(), QColor());
    }

    void testM_color() {
        PerceptualColor::ColorPatch thePatch;
        thePatch.setColor(Qt::red);
        QCOMPARE(thePatch.color(), Qt::red);
        QCOMPARE(thePatch.d_pointer->m_color, Qt::red);
    }

    void testApplyColors() {
        PerceptualColor::ColorPatch thePatch;
        // Test initial value (an invalid color following the documentation)
        QCOMPARE(thePatch.color(), QColor());
    }

    void testColorProperty_data() {
        helperProvideQColors();
    }

    void testColorProperty() {
        QFETCH(QColor, color);
        PerceptualColor::ColorPatch thePatch;
        thePatch.setColor(color);
        QCOMPARE(thePatch.color(), color);
    }

    void helperReceiveSignals(QColor color) {
        m_color = color;
    }

    void testColorChanged() {
        // Initialization
        PerceptualColor::ColorPatch thePatch;
        connect(
            &thePatch,
            &PerceptualColor::ColorPatch::colorChanged,
            this,
            &TestColorPatch::helperReceiveSignals
        );

        m_color = Qt::red;
        thePatch.setColor(QColor()); // invalid like initial value
        // We expect that no signal was emitted
        QCOMPARE(m_color, Qt::red);

        m_color = Qt::red;
        thePatch.setColor(Qt::blue); // new value
        // We expect that a signal was emitted
        QCOMPARE(m_color, Qt::blue);

        m_color = Qt::red;
        thePatch.setColor(Qt::blue); // is yet blue, set again to blue…
        // We expect that no signal was emitted
        QCOMPARE(m_color, Qt::red);

        m_color = Qt::red;
        thePatch.setColor(QColor()); // new: invalid color
        // We expect that a signal was emitted
        QCOMPARE(m_color, QColor());
    }

    void testSnippet01() {
        snippet01();
    }

};

}

QTEST_MAIN(PerceptualColor::TestColorPatch)
#include "testcolorpatch.moc" // necessary because we do not use a header file
