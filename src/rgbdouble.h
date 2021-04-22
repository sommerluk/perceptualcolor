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

#ifndef RGBDOUBLE_H
#define RGBDOUBLE_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include <QDebug>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief A RGB color.
 *
 * Storage of floating point RGB values with <tt>double</tt> precision.
 *
 * The valid range for each component is <tt>[0, 1]</tt>, but you are free
 * to set also invalid values.
 *
 * The data is not default-initializad; it is undefined when the object
 * is created!
 *
 * @note This format of storage is practical for working with
 * <a href="http://www.littlecms.com/">LittleCMS</a>. LittleCMS
 * uses internally the type <tt>cmsFloat64Number</tt> which is defined
 * as synonym of <tt>double</tt>. However, we use <tt>double</tt>
 * to declare <em>this</em> type because it has to be comprehensible
 * without knowing about LittleCMS internals. But we make sure by static
 * asserts in file @ref rgbdouble.cpp that indeed both types are identical.
 * We use static asserts (and not unit tests) to do so, because data types
 * may vary on different compilers and systems, and with static asserts,
 * we make sure to have a feedback on every system this library is build
 * on. Finally, this RGB format can be treated as buffer within LittleCMS,
 * which is quite useful for converting quickly a single color.
 *
 * Example:
 * @snippet test/testrgbdouble.cpp Use RgbDouble
 *
 * This class is declared as type to Qt's type system:
 * <tt>Q_DECLARE_METATYPE(PerceptualColor::RgbDouble)</tt>.
 * Depending on your use case (for example if you want to use it
 * reliably in Qt's signals and slots), you might consider calling
 * <tt>qRegisterMetaType()</tt> for this type, once you have a QApplication
 * object.
 *
 * @todo This data type is only used within @ref RgbColorSpace. It is not
 * part of the public API. Therefore, it seems more appropriate to either
 * delete it entirely (substituting its usage by an array of three elements
 * with LittleCMS data type) or keep it, but use consequently LittleCMS
 * for the unterlying data and make unit tests with LittleCMS to be
 * sure it behaves as expected, also when used as a buffer. */
struct RgbDouble {
    /** @brief The red value. */
    double red;
    /** @brief The green value. */
    double green;
    /** @brief The blue value. */
    double blue;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::RgbDouble &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::RgbDouble)

#endif // RGBDOUBLE_H
