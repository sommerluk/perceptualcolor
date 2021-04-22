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
#include "version.h"

#include <QtTest>

static void snippet01()
{
//! [Version Macro usage]
#if (PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= QT_VERSION_CHECK(1, 2, 3))
// Some code…
#else
// Some different code…
#endif
    //! [Version Macro usage]
}

namespace PerceptualColor
{
class TestVersion : public QObject
{
    Q_OBJECT

public:
    TestVersion(QObject *parent = nullptr)
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

    void testPerceptualColorRunTimeVersion()
    {
        QVERIFY2(perceptualColorRunTimeVersion() >= QVersionNumber(0, 0, 1), "Verify that the version number is at least 0.0.1.");
        QVERIFY2(perceptualColorRunTimeVersion() < QVersionNumber(99, 0, 1), "Verify that the version number is not too big.");
    }

    void testSnippet01()
    {
        snippet01();
    }
};

} // flags

QTEST_MAIN(PerceptualColor::TestVersion)

// The following “include” is necessary because we do not use a header file:
#include "testversion.moc"
