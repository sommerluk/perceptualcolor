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
#include "helper.h"

#include <QtTest>

#include <QPainter>
#include <QTextDocument>

#include <rgbcolorspace.h>

static bool snippet01()
{
    const QString myRichText = QStringLiteral(u"abc");
    const QString myPlainText = QStringLiteral(u"abc");
    QScopedPointer<QWidget> myWidget1(new QWidget());
    QScopedPointer<QWidget> myWidget2(new QWidget());
    //! [richTextMarkerExample]
    myWidget1->setWhatsThis( // Make sure rich text is treated as such:
        PerceptualColor::richTextMarker() + myRichText);

    myWidget2->setWhatsThis( // Make sure plain text is treated as such:
        PerceptualColor::richTextMarker() + myPlainText.toHtmlEscaped());
    //! [richTextMarkerExample]
    return Qt::mightBeRichText(myWidget1->whatsThis()) //
        && Qt::mightBeRichText(myWidget2->whatsThis());
}

namespace PerceptualColor
{
class TestHelper : public QObject
{
    Q_OBJECT

public:
    TestHelper(QObject *parent = nullptr)
        : QObject(parent)
    {
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

    void testInRangeInt()
    {
        QCOMPARE(PerceptualColor::isInRange<int>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, 0, -1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, 0, -1), false);

        QCOMPARE(PerceptualColor::isInRange<double>(-3.1, 0.2, -1.3), false);
    }

    void testGamutPrecision()
    {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        QVERIFY2(PerceptualColor::gamutPrecision < 1, "Verify that gamut precision value is not too high");
    }

    void testTransparencyBackground()
    {
        QImage temp = transparencyBackground(1);
        QVERIFY2(temp.size().width() > 0, "Width of image is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height of image is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
    }

    void testStandardWheelSteps()
    {
        QWheelEvent temp(QPointF(), QPointF(), QPoint(), QPoint(200, 120), 0, Qt::Orientation::Vertical, Qt::MouseButton::MiddleButton, Qt::KeyboardModifier::NoModifier);
        QCOMPARE(PerceptualColor::standardWheelStepCount(&temp), 1);
    }

    void testLchConvertion()
    {
        // Check if round-trips work fine.
        // One sense
        cmsCIELCh startValue;
        startValue.L = 50.1;
        startValue.C = 20.1;
        startValue.h = 80.1;
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).L, startValue.L);
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).C, startValue.C);
        QCOMPARE(toCmsCieLch(toLchDouble(startValue)).h, startValue.h);
        // The other sense
        LchDouble startValue2;
        startValue2.l = 50.1;
        startValue2.c = 20.1;
        startValue2.h = 80.1;
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).l, startValue2.l);
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).c, startValue2.c);
        QCOMPARE(toLchDouble(toCmsCieLch(startValue2)).h, startValue2.h);
    }

    void testSteps()
    {
        QVERIFY2(pageStepChroma > singleStepChroma, "Chroma page step is bigger than single step.");
        QVERIFY2(singleStepChroma > 0, "Chroma single step is positive.");
        QVERIFY2(pageStepHue > singleStepHue, "Hue page step is bigger than single step.");
        QVERIFY2(singleStepHue > 0, "Hue single step is positive.");
    }

    void testRounding()
    {
        QCOMPARE(roundToDigits(12.3456, 6), 12.345600);
        QCOMPARE(roundToDigits(12.3456, 5), 12.34560);
        QCOMPARE(roundToDigits(12.3456, 4), 12.3456);
        QCOMPARE(roundToDigits(12.3456, 3), 12.346);
        QCOMPARE(roundToDigits(12.3456, 2), 12.35);
        QCOMPARE(roundToDigits(12.3456, 1), 12.3);
        QCOMPARE(roundToDigits(12.3456, 0), 12.);
        QCOMPARE(roundToDigits(12.3456, -1), 10.);
        QCOMPARE(roundToDigits(12.3456, -2), 0.);
        QCOMPARE(roundToDigits(92.3456, -2), 100.);
    }

    void testRichTextMarker()
    {
        const QString myMarker = richTextMarker();
        QVERIFY(myMarker.count() > 0);
        QVERIFY(Qt::mightBeRichText(myMarker));

        const QString myText = QStringLiteral(u"abc");
        QVERIFY(!Qt::mightBeRichText(myText)); // Assertion
        QVERIFY(Qt::mightBeRichText(myMarker + myText));
    }

    void testRichTextMarkerSnippet()
    {
        QVERIFY(snippet01());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
