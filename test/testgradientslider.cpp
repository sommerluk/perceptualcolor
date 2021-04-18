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

#include "perceptualinternal.h"

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/gradientslider.h"
// Second, the private implementation.
#include "gradientslider_p.h"

#include <QSignalSpy>
#include <QtTest>

namespace PerceptualColor
{
class TestGradientSlider : public QObject
{
    Q_OBJECT

public:
    TestGradientSlider(QObject *parent = nullptr)
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

    void testConstructorAndDestructor()
    {
        // Constructor and destructor should not crash.
        GradientSlider test(m_rgbColorSpace);
        QCOMPARE(test.d_pointer->m_rgbColorSpace, m_rgbColorSpace);
        QCOMPARE(test.d_pointer->m_orientation, Qt::Vertical);
    }

    void testConstructorAndDestructorHorizontal()
    {
        // Constructor and destructor should not crash.
        GradientSlider test(m_rgbColorSpace, Qt::Horizontal);
        QCOMPARE(test.d_pointer->m_rgbColorSpace, m_rgbColorSpace);
        QCOMPARE(test.d_pointer->m_orientation, Qt::Horizontal);
    }

    void testConstructorAndDestructorVertical()
    {
        // Constructor and destructor should not crash.
        GradientSlider test(m_rgbColorSpace, Qt::Vertical);
        QCOMPARE(test.d_pointer->m_rgbColorSpace, m_rgbColorSpace);
        QCOMPARE(test.d_pointer->m_orientation, Qt::Vertical);
    }

    void testFirstColor()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        LchaDouble color;
        color.l = 50;
        color.c = 50;
        color.h = 50;
        color.a = 1;
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::firstColorChanged);
        testSlider.setFirstColor(color);
        QVERIFY(testSlider.firstColor().hasSameCoordinates(color));
        QCOMPARE(spy.count(), 1);
    }

    void testSecondColor()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        LchaDouble color;
        color.l = 50;
        color.c = 50;
        color.h = 50;
        color.a = 1;
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::secondColorChanged);
        testSlider.setSecondColor(color);
        QVERIFY(testSlider.secondColor().hasSameCoordinates(color));
        QCOMPARE(spy.count(), 1);
    }

    void testSetColors()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        LchaDouble color;
        color.l = 50;
        color.c = 50;
        color.h = 50;
        color.a = 1;
        QSignalSpy spyFirst(&testSlider, &PerceptualColor::GradientSlider::firstColorChanged);
        QSignalSpy spySecond(&testSlider, &PerceptualColor::GradientSlider::secondColorChanged);
        testSlider.setColors(color, color);
        QVERIFY(testSlider.firstColor().hasSameCoordinates(color));
        QCOMPARE(spyFirst.count(), 1);
        QVERIFY(testSlider.secondColor().hasSameCoordinates(color));
        QCOMPARE(spySecond.count(), 1);
    }

    void testMinimalSizeHint()
    {
        GradientSlider testWidget(m_rgbColorSpace);
        QVERIFY2(testWidget.minimumSizeHint().width() > 0, "minimalSizeHint width is implemented.");
        QVERIFY2(testWidget.minimumSizeHint().height() > 0, "minimalSizeHint height is implemented.");
    }

    void testSizeHint()
    {
        GradientSlider testWidget(m_rgbColorSpace);
        QVERIFY2(testWidget.sizeHint().width() >= testWidget.minimumSizeHint().width(), "sizeHint width is bigger than or equal to minimalSizeHint width.");
        QVERIFY2(testWidget.sizeHint().height() >= testWidget.minimumSizeHint().height(),
                 "sizeHint height is bigger than or equal to minimalSizeHint "
                 "height.");
    }

    void testSingleStep()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::singleStepChanged);
        testSlider.setSingleStep(0.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(testSlider.singleStep(), 0.5);
        QCOMPARE(testSlider.d_pointer->m_singleStep, 0.5);
        testSlider.setSingleStep(0.2);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(testSlider.singleStep(), 0.2);
        QCOMPARE(testSlider.d_pointer->m_singleStep, 0.2);
        testSlider.setSingleStep(-10);
        QCOMPARE(spy.count(), 3);
        QCOMPARE(testSlider.singleStep(), 0);
        QCOMPARE(testSlider.d_pointer->m_singleStep, 0);
        testSlider.setSingleStep(10);
        QCOMPARE(spy.count(), 4);
        QCOMPARE(testSlider.singleStep(), 1);
        QCOMPARE(testSlider.d_pointer->m_singleStep, 1);
    }

    void testPageStep()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::pageStepChanged);
        testSlider.setPageStep(0.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(testSlider.pageStep(), 0.5);
        QCOMPARE(testSlider.d_pointer->m_pageStep, 0.5);
        testSlider.setPageStep(0.2);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(testSlider.pageStep(), 0.2);
        QCOMPARE(testSlider.d_pointer->m_pageStep, 0.2);
        testSlider.setPageStep(-10);
        QCOMPARE(spy.count(), 3);
        QCOMPARE(testSlider.pageStep(), 0);
        QCOMPARE(testSlider.d_pointer->m_pageStep, 0);
        testSlider.setPageStep(10);
        QCOMPARE(spy.count(), 4);
        QCOMPARE(testSlider.pageStep(), 1);
        QCOMPARE(testSlider.d_pointer->m_pageStep, 1);
    }

    void testValue()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        testSlider.setValue(0.3);
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::valueChanged);
        testSlider.setValue(0.5);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(testSlider.value(), 0.5);
        QCOMPARE(testSlider.d_pointer->m_value, 0.5);
        testSlider.setValue(0.2);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(testSlider.value(), 0.2);
        QCOMPARE(testSlider.d_pointer->m_value, 0.2);
        testSlider.setValue(-10);
        QCOMPARE(spy.count(), 3);
        QCOMPARE(testSlider.value(), 0);
        QCOMPARE(testSlider.d_pointer->m_value, 0);
        testSlider.setValue(10);
        QCOMPARE(spy.count(), 4);
        QCOMPARE(testSlider.value(), 1);
        QCOMPARE(testSlider.d_pointer->m_value, 1);
    }

    void testKeyPressEventLTR()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        testSlider.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        testSlider.setSingleStep(0.1);
        testSlider.setPageStep(0.2);
        testSlider.setValue(0.3);
        QTest::keyClick(&testSlider, Qt::Key_Plus);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Minus);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Up);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Down);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Right);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Left);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_PageUp);
        QCOMPARE(testSlider.value(), 0.5);
        QTest::keyClick(&testSlider, Qt::Key_PageDown);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Home);
        QCOMPARE(testSlider.value(), 0.0);
        QTest::keyClick(&testSlider, Qt::Key_End);
        QCOMPARE(testSlider.value(), 1.0);
    }

    void testKeyPressEventRTL()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        testSlider.setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        testSlider.setSingleStep(0.1);
        testSlider.setPageStep(0.2);
        testSlider.setValue(0.3);
        QTest::keyClick(&testSlider, Qt::Key_Plus);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Minus);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Up);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Down);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Left);
        QCOMPARE(testSlider.value(), 0.4);
        QTest::keyClick(&testSlider, Qt::Key_Right);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_PageUp);
        QCOMPARE(testSlider.value(), 0.5);
        QTest::keyClick(&testSlider, Qt::Key_PageDown);
        QCOMPARE(testSlider.value(), 0.3);
        QTest::keyClick(&testSlider, Qt::Key_Home);
        QCOMPARE(testSlider.value(), 0.0);
        QTest::keyClick(&testSlider, Qt::Key_End);
        QCOMPARE(testSlider.value(), 1.0);
    }

    void testOrientationDefaultConstructor()
    {
        GradientSlider testSlider(m_rgbColorSpace);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Vertical);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    }

    void testOrientationVerticalConstructor()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Orientation::Vertical);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Vertical);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    }

    void testOrientationHorizontalConstructor()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
    }

    void testOrientation()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Vertical);
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::orientationChanged);
        testSlider.setOrientation(Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(spy.count(), 1);
        testSlider.setOrientation(Qt::Orientation::Vertical);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Vertical);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
        QCOMPARE(spy.count(), 2);
    }

    void testSetOrientationWithoutSignalAndForceNewSizePolicy()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Vertical);
        QSignalSpy spy(&testSlider, &PerceptualColor::GradientSlider::orientationChanged);
        testSlider.d_pointer->setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(spy.count(), 0);
        testSlider.d_pointer->setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.orientation(), Qt::Orientation::Horizontal);
        QCOMPARE(testSlider.sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
        QCOMPARE(testSlider.sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
        QCOMPARE(spy.count(), 0);
    }

    void testPhysicalPixelLength()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QVERIFY2(testSlider.d_pointer->physicalPixelLength() >= 0, "physicalPixelLength() should be ≥ 0.");
    }

    void testPhysicalPixelThickness()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        QVERIFY2(testSlider.d_pointer->physicalPixelThickness() >= 0, "physicalPixelLength() should be ≥ 0.");
    }

    void testFromWidgetPositionToValue()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        qreal value;
        value = testSlider.d_pointer->fromWidgetPixelPositionToValue(QPoint(0, 0));
        QVERIFY2((value >= 0) && (value <= 1), "fromWidgetPixelPositionToValue() should be 0 ≤ value ≤ 1.");
        value = testSlider.d_pointer->fromWidgetPixelPositionToValue(QPoint(-100, -100));
        QVERIFY2((value >= 0) && (value <= 1), "fromWidgetPixelPositionToValue() should be 0 ≤ value ≤ 1.");
        value = testSlider.d_pointer->fromWidgetPixelPositionToValue(QPoint(10000, 10000));
        QVERIFY2((value >= 0) && (value <= 1), "fromWidgetPixelPositionToValue() should be 0 ≤ value ≤ 1.");
    }

    void testPaintEvent()
    {
        GradientSlider testSlider(m_rgbColorSpace, Qt::Vertical);
        testSlider.show();
        // Paint event should not crash.
        // repaint() will call paintEvent()…
        testSlider.repaint();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestGradientSlider)

// The following “include” is necessary because we do not use a header file:
#include "testgradientslider.moc"
