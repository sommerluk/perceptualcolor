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

// Own header
#include "PerceptualColor/circulardiagram.h"

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent The widget’s parent widget. This paramenter will be passed
 * to the QWidget base class constructor. */
CircularDiagram::CircularDiagram(QWidget *parent)
: AbstractDiagram(parent)
{
}

/** @brief Indicates that the widget's preferred height depends on its width.
 * 
 * Reimplemented from base class.
 * 
 * @note Qt’s layout management makes only very limited use of this
 * information.
 * @sa @ref heightForWidth */
bool CircularDiagram::hasHeightForWidth() const
{
    return true;
}

/** @brief Returns the preferred height for this widget, given the
 * width <tt>w</tt>.
 * 
 * The widget preferres a circular form, so same height as width.
 * 
 * Reimplemented from base class.
 * 
 * @note Qt’s layout management makes only very limited use of this
 * information.
 * @param w widget width
 * @returns the preferred height for this widget, given the width <tt>w</tt>
 * @sa @ref hasHeightForWidth */
int CircularDiagram::heightForWidth(int w) const
{
    return w;
}

}
