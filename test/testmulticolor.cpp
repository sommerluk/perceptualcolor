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
#include "multicolor.h"

#include <QtTest>

#include "PerceptualColor/rgbcolorspacefactory.h"

namespace PerceptualColor
{
class TestMultiColor : public QObject
{
    Q_OBJECT

public:
    TestMultiColor(QObject *parent = nullptr)
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

    void testConstructorDestructorStatic()
    {
        MultiColor myMulticolor;
    }

    void testConstructorDestructorDynamic()
    {
        QScopedPointer<MultiColor> myPointer {new MultiColor()};
    }

    void testCopyConstructor()
    {
        MultiColor myMulticolor1;
        MultiColor myMulticolor2(myMulticolor1);
        QCOMPARE(myMulticolor2.toHlc(), myMulticolor1.toHlc());
        QCOMPARE(myMulticolor2.toLch().l, myMulticolor1.toLch().l);
        QCOMPARE(myMulticolor2.toLch().c, myMulticolor1.toLch().c);
        QCOMPARE(myMulticolor2.toLch().h, myMulticolor1.toLch().h);
        QCOMPARE(myMulticolor2.toRgbQColor(), myMulticolor1.toRgbQColor());
    }

    void testCopyAssignment()
    {
        MultiColor myMulticolor1;
        MultiColor myMulticolor2;
        myMulticolor2 = myMulticolor1;
        QCOMPARE(myMulticolor2.toHlc(), myMulticolor1.toHlc());
        QCOMPARE(myMulticolor2.toLch().l, myMulticolor1.toLch().l);
        QCOMPARE(myMulticolor2.toLch().c, myMulticolor1.toLch().c);
        QCOMPARE(myMulticolor2.toLch().h, myMulticolor1.toLch().h);
        QCOMPARE(myMulticolor2.toRgbQColor(), myMulticolor1.toRgbQColor());
    }

    void testMoveConstructor()
    {
        MultiColor myReference = MultiColor::fromRgbQColor( //
            RgbColorSpaceFactory::createSrgb(),
            Qt::yellow);
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2(std::move(myMulticolor1));
        QCOMPARE(myMulticolor2.toHlc(), myReference.toHlc());
        QCOMPARE(myMulticolor2.toLch().l, myReference.toLch().l);
        QCOMPARE(myMulticolor2.toLch().c, myReference.toLch().c);
        QCOMPARE(myMulticolor2.toLch().h, myReference.toLch().h);
        QCOMPARE(myMulticolor2.toRgbQColor(), myReference.toRgbQColor());
    }

    void testMoveAssignment()
    {
        MultiColor myReference = MultiColor::fromRgbQColor( //
            RgbColorSpaceFactory::createSrgb(),
            Qt::yellow);
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2;
        myMulticolor2 = std::move(myMulticolor1);
        QCOMPARE(myMulticolor2.toHlc(), myReference.toHlc());
        QCOMPARE(myMulticolor2.toLch().l, myReference.toLch().l);
        QCOMPARE(myMulticolor2.toLch().c, myReference.toLch().c);
        QCOMPARE(myMulticolor2.toLch().h, myReference.toLch().h);
        QCOMPARE(myMulticolor2.toRgbQColor(), myReference.toRgbQColor());
    }

    void testLch()
    {
        LchDouble myLch;
        myLch.l = 51;
        myLch.c = 21;
        myLch.h = 1;
        MultiColor myMulticolor1 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(),
            myLch);
        QCOMPARE(myMulticolor1.toLch().l, 51);
        QCOMPARE(myMulticolor1.toLch().c, 21);
        QCOMPARE(myMulticolor1.toLch().h, 1);
        QCOMPARE(myMulticolor1.toHlc(), (QList<double> {1, 51, 21}));
    }

    void testRgb()
    {
        MultiColor myMulticolor1 =     //
            MultiColor::fromRgbQColor( //
                RgbColorSpaceFactory::createSrgb(),
                Qt::yellow);
        QCOMPARE(myMulticolor1.toRgbQColor(), Qt::yellow);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiColor)

// The following “include” is necessary because we do not use a header file:
#include "testmulticolor.moc"
