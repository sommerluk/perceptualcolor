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

#include "qtconfiguration.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/abstractcirculardiagram.h"
// Second, the private implementation.
#include "abstractcirculardiagram_p.h"

#include <QFocusEvent>

// TODO Remove me:
#include <QDebug>

namespace PerceptualColor {

/** @brief The constructor.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the QWidget base class constructor. */
AbstractCircularDiagram::AbstractCircularDiagram(QWidget *parent) :
    AbstractDiagram(parent)/*,
    d_pointer(new AbstractCircularDiagramPrivate())*/
{
    // Set size policy
    QSizePolicy temp = QSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    // Quote from Qt’s documentation for setWidthForHeight():
    //     “It is not possible to have a layout with both height-for-width
    //      and width-for-height constraints at the same time.“
    // As we cannot have both, we choose height-for-width because this is
    // the one that also exists directly in QWidget, so we can be
    // consistent.
    temp.setHeightForWidth(true);
    setSizePolicy(temp);

    // Set focus policy
    setFocusPolicy(Qt::FocusPolicy::TabFocus);
}

/** @brief Destructor */
AbstractCircularDiagram::~AbstractCircularDiagram() noexcept
{
}

/** @brief Indicates that the widget's preferred height depends on its width.
 * 
 * Reimplemented from base class.
 * 
 * @note Qt’s layout management makes only very limited use of this
 * information.
 * @sa @ref heightForWidth */
bool AbstractCircularDiagram::hasHeightForWidth() const
{
    return true;
}

/** @brief Returns the preferred height for this widget, given the
 * width <tt>w</tt>.
 * 
 * The widget prefers a circular form, so same height as width.
 * 
 * Reimplemented from base class.
 * 
 * @note Qt’s layout management makes only very limited use of this
 * information.
 * @param w widget width
 * @returns the preferred height for this widget, given the width <tt>w</tt>
 * @sa @ref hasHeightForWidth */
int AbstractCircularDiagram::heightForWidth(int w) const
{
    return w;
}

/** @brief The diameter of this circular widget, measured in physical pixels.
 * @returns The diameter of this circular widget, measured in physical pixels.
 * This is the maximum possible diameter for a circular-shaped widget, given
 * the current @ref physicalPixelSize. */
int AbstractCircularDiagram::physicalPixelWidgetDiameter() const
{
    int temp = qMin(
        physicalPixelSize().width(),
        physicalPixelSize().height()
    );
    if (temp < 0) {
        temp = 0;
    }
    return temp;
}

}
