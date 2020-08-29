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

#ifndef CIRCULARDIAGRAM_H
#define CIRCULARDIAGRAM_H

#include "PerceptualColor/abstractdiagram.h"

namespace PerceptualColor {
    
/** @brief Base class for circular diagrams.
 * 
 * Provides some elements that are common for all circular diagrams in this
 * library. 
 * 
 * As the natural interaction space of these diagrams is a circle, it’s best
 * to have equal <tt>width()</tt> and <tt>hight()</tt> for the widget. This
 * class provides the corresponding implementations to notify the layout
 * manager about that. See @ref hasHeightForWidth and @ref heightForWidth for
 * details. */
class CircularDiagram : public AbstractDiagram
{
    Q_OBJECT

public:
    CircularDiagram(QWidget *parent = nullptr);
    virtual bool hasHeightForWidth() const override;
    virtual int heightForWidth(int w) const override;

private:
    Q_DISABLE_COPY(CircularDiagram)

};

}

#endif // CIRCULARDIAGRAM_H
