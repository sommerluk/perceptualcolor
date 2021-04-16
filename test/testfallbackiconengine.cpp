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
#include "fallbackiconengine.h"

#include <QIcon>
#include <QPainter>
#include <QtTest>

Q_DECLARE_METATYPE(QIcon::Mode)
Q_DECLARE_METATYPE(QIcon::State)

namespace PerceptualColor
{
class TestFallbackIconEngine : public QObject
{
    Q_OBJECT

public:
    TestFallbackIconEngine(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

    void helperProvideTestData()
    {
        QTest::addColumn<QRect>("rect");
        QTest::addColumn<QIcon::Mode>("mode");
        QTest::addColumn<QIcon::State>("state");

        QList<QPoint> rectPointList;
        rectPointList.append(QPoint(0, 0));
        rectPointList.append(QPoint(0, 10));
        rectPointList.append(QPoint(10, 0));
        rectPointList.append(QPoint(10, 10));
        rectPointList.append(QPoint(-10, 0));
        rectPointList.append(QPoint(0, -10));
        rectPointList.append(QPoint(-10, -10));

        QList<QSize> rectSizeList;
        rectSizeList.append(QSize(0, 0));
        rectSizeList.append(QSize(1, 1));
        rectSizeList.append(QSize(10, 10));
        rectSizeList.append(QSize(10, 20));
        rectSizeList.append(QSize(-1, -1));
        rectSizeList.append(QSize(0, 10));
        rectSizeList.append(QSize(10, 0));

        QList<QIcon::Mode> modeList;
        modeList.append(QIcon::Mode::Active);
        modeList.append(QIcon::Mode::Disabled);
        modeList.append(QIcon::Mode::Normal);
        modeList.append(QIcon::Mode::Selected);

        QList<QIcon::State> stateList;
        stateList.append(QIcon::State::Off);
        stateList.append(QIcon::State::On);

        for (int i = 0; i < rectPointList.size(); ++i) {
            for (int j = 0; j < rectSizeList.size(); ++j) {
                for (int k = 0; k < modeList.size(); ++k) {
                    for (int l = 0; l < stateList.size(); ++l) {
                        QTest::newRow(QStringLiteral("QRect(QPoint(%1, %2), QSize(%3, %4)) %5 %6")
                                          .arg(rectPointList.at(i).x())
                                          .arg(rectPointList.at(i).y())
                                          .arg(rectSizeList.at(j).width())
                                          .arg(rectSizeList.at(j).height())
                                          .arg(modeList.at(k))
                                          .arg(stateList.at(l))
                                          .toUtf8()
                                          .data())
                            << QRect(rectPointList.at(i), rectSizeList.at(j)) << modeList.at(k) << stateList.at(l);
                    }
                }
            }
        }
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

    void testConstructorDestructorOnStack()
    {
        // The class should instanciate on the stack without crash.
        FallbackIconEngine test;
    }

    void testConstructorDestructorOnHeap()
    {
        // The class should instanciate on the heap without crash.
        QScopedPointer<FallbackIconEngine> test {new FallbackIconEngine};
    }

    void testClone()
    {
        QScopedPointer<FallbackIconEngine> test1 {new FallbackIconEngine};
        QWidget testWidget;
        test1->setReferenceWidget(&testWidget);
        // The clone function should not crash.
        QScopedPointer<QIconEngine> test2 {test1->clone()};
        FallbackIconEngine *test2a = dynamic_cast<FallbackIconEngine *>(test2.data());
        QVERIFY2(test2a != nullptr,
                 "The clone should correctly cast dynamically "
                 "to FallbackIconEngine*.");
        // Test if the reference widget was copied correctly.
        QCOMPARE(test2a->m_referenceWidget, &testWidget);
    }

    void testPaint_data()
    {
        helperProvideTestData();
    }

    void testPaint()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QFETCH(QIcon::State, state);
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        FallbackIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        myEngine.paint(&myPainter, rect, mode, state);

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testPaintRefreshFallbackIcon_data()
    {
        helperProvideTestData();
    }

    void testPaintRefreshFallbackIcon()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        FallbackIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        myEngine.paintRefreshFallbackIcon(&myPainter, rect, mode);

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testPixmap_data()
    {
        helperProvideTestData();
    }

    void testPixmap()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QFETCH(QIcon::State, state);
        FallbackIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        myEngine.pixmap(rect.size(), mode, state);

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testSetReferenceWidget()
    {
        FallbackIconEngine myEngine;
        QScopedPointer<QWidget> myWidget {new QWidget};
        // Setting reference widget shall not crash.
        myEngine.setReferenceWidget(myWidget.data());
        QCOMPARE(myEngine.m_referenceWidget, myWidget.data());
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        // Should not crash
        myEngine.paint(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active, QIcon::State::On);
        // Should not crash
        myEngine.paintRefreshFallbackIcon(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active);
        // Should not crash
        myEngine.pixmap(QSize(11, 11), QIcon::Mode::Active, QIcon::State::On);
        // Now delete the widget, then test again for crashs
        myWidget.reset();
        // Should not crash
        myEngine.paint(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active, QIcon::State::On);
        // Should not crash
        myEngine.paintRefreshFallbackIcon(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active);
        // Should not crash
        myEngine.pixmap(QSize(11, 11), QIcon::Mode::Active, QIcon::State::On);
    }
};

}

QTEST_MAIN(PerceptualColor::TestFallbackIconEngine)

// The following “include” is necessary because we do not use a header file:
#include "testfallbackiconengine.moc"
