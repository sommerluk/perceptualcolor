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

#ifndef RGBCOLORSPACEFACTORY_H
#define RGBCOLORSPACEFACTORY_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include <QSharedPointer>

namespace PerceptualColor
{
class RgbColorSpace;

/** @brief Factory for color space objects.
 *
 * These color space objects are needed in the constructors of various classes
 * of this library.
 *
 * Creating color space objects can be slow. But once created, they can be
 * used simultaniously on various widgets. Thanks to the QSharedPointer, you
 * can easily create a color space object, pass it to the widget constructors
 * you like, and then forget about it – it will be deleted automatically when
 * the last widget that used it has been deleted. And passing the shared
 * pointer to widget constructors is fast! Usage example:
 *
 * @snippet test/testrgbcolorspacefactory.cpp Create */
class PERCEPTUALCOLOR_IMPORTEXPORT RgbColorSpaceFactory
{
public:
    // No Q_INVOKABLE here because the class does not inherit QObject:
    static QSharedPointer<PerceptualColor::RgbColorSpace> createSrgb();

private:
    /** @internal
     *
     * @brief Private default constructor.
     *
     * This class should have no instances, therefore the constructor
     * is private. */
    RgbColorSpaceFactory() = default;

    /** @internal
     *
     * @brief Only for unit tests. */
    friend class TestRgbColorSpaceFactory;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACEFACTORY_H
