// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2020 Lukas Sommer somerluk@gmail.com
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

#ifndef LCHDOUBLE_H
#define LCHDOUBLE_H

#include <lcms2.h>

namespace PerceptualColor {

/** @brief A LCH color.
 * 
 * Storage of floating point LCH values with <tt>double</tt> precision.
 * 
 * The data is not default-initializad; it is undefined when the object
 * is created.
 * 
 * Example:
 * @snippet test/testlchdouble.cpp Use LchDouble
 * 
 * Data members:
 * 
 * - L: Lightness, mesured in percent. The valid range is <tt>[0, 100]</tt>.
 * - C: Chroma. <tt>0</tt> means no chroma (grayscale). The maximum
 *   value depends on the gamut, for sRGB for example it’s
 *   @ref LchValues::srgbMaximumChroma. Other gamuts can
 *   be bigger, but the final limit is the gamut of human
 *   perception, out of which a Chroma value does not make
 *   sense: @ref LchValues::humanMaximumChroma
 * - h: Hue, measured in degree. The valid range is <tt>[0, 360[</tt>.
 * 
 * @sa @ref LchValues explains more details about the valid
 * range. */
using LchDouble = cmsCIELCh;

} // namespace PerceptualColor

#endif // LCHDOUBLE_H
