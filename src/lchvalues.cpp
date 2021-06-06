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

// Own header
#include "lchvalues.h"

namespace PerceptualColor
{
/** @brief Neutral gray color as Lab value.
 *
 * Neutral gray is a good choice for the background, as it is equally
 * distant from black and white, and also quite distant from any
 * saturated color. */
LchDouble LchValues::neutralGray()
{
    return LchDouble(neutralLightness, neutralChroma, neutralHue);
}

/** @brief Versatile initial color for widgets in
 * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
 * sRGB gamut
 *
 * This provides a colorfull (non-gray) initial color.
 *
 * The lightness and the chroma value are somewhere in the middle of
 * the valid range. In sRGB this is always a valid value pair, even if
 * the hue changes. Both, lightness and chroma, are linear ranges,
 * so it seems appropriate to have values somewhere in the middle
 * as initial color: The widgets will display the handles somewhere
 * in the middle, which is easier to spot for new users than having
 * them at the very beginning or the very end.
 *
 * The hue range is not linear, but circular – and so are the widgets
 * displaying him: The handle has the same visibility, whereever he is
 * and whatever the hue value is. Therefore, we use 0° as default value
 * which seems to be the natural choise.
 *
 * @note An alternative to 0° might be 180°. If the lightness is 50%,
 * among 0°, 90°, 180° and 270°, it’s 180° that has the lowest possible
 * maximum chroma. So when choose 180°, with a given chroma, the resulting
 * color is more vivid and clean than those at 0°, 90° and 270°. */
LchDouble LchValues::srgbVersatileInitialColor()
{
    return LchDouble(neutralLightness, srgbVersatileChroma, neutralHue);
}

} // namespace PerceptualColor
