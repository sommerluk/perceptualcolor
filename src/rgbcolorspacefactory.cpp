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

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/rgbcolorspacefactory.h"

#include "rgbcolorspace.h"

namespace PerceptualColor
{
/** @brief Create an sRGB color space object.
 *
 * This is a build-in profile that does not require any external ICC file.
 *
 * @returns A shared pointer to a newly created sRGB color space object. */
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpaceFactory::createSrgb()
{
    return RgbColorSpace::createSrgb();
}

/** @brief Create a color space object for a given ICC file.
 *
 * This function may fail to create the color space object when it cannot
 * open the given file, or when the file cannot be interpreted by LittleCMS.
 *
 * @param fileName The file name. TODO Must have a form that is compliant with
 * <tt>QFile</tt>.
 *
 * @returns A shared pointer to a newly created color space object on success.
 * A shared pointer to <tt>nullptr</tt> otherwise. */
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpaceFactory::createFromFile(const QString &fileName)
{
    return RgbColorSpace::createFromFile(fileName);
}

} // namespace PerceptualColor
