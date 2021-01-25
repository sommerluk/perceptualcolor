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

#include "perceptualcolorlib_qtconfiguration.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h"

#include <math.h>

#include <QCoreApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

namespace PerceptualColor {

/** @brief Constructor */
WheelColorPicker::WheelColorPicker(
    const QSharedPointer<RgbColorSpace> &colorSpace,
    QWidget *parent
) :
    SimpleColorWheel(colorSpace, parent),
    d_pointer(new WheelColorPickerPrivate(this))
{
    
    d_pointer->m_chromaLightnessDiagram = new ChromaLightnessDiagram(
        colorSpace,
        this
    );
    d_pointer->resizeChildWidget();
    d_pointer->m_chromaLightnessDiagram->setHue(hue());
    d_pointer->m_chromaLightnessDiagram->show();
    d_pointer->m_chromaLightnessDiagram->setFocusPolicy(
        Qt::FocusPolicy::ClickFocus
    ); // TODO Why not just inherit TabFocus? But apparently it works! Why?
    d_pointer->m_chromaLightnessDiagram->setFocusProxy(this);
    connect(
        this,
        &WheelColorPicker::hueChanged,
        d_pointer->m_chromaLightnessDiagram,
        &ChromaLightnessDiagram::setHue
    );
    connect(
        d_pointer->m_chromaLightnessDiagram,
        &ChromaLightnessDiagram::colorChanged,
        this,
        &WheelColorPicker::currentColorChanged
    );
}

/** @brief Default destructor */
WheelColorPicker::~WheelColorPicker() noexcept
{
}

/** @brief Constructor
 * 
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
WheelColorPicker::WheelColorPickerPrivate::WheelColorPickerPrivate(
    WheelColorPicker *backLink
) : q_pointer(backLink)
{
}

// TODO This widget and its child should be a unit: Only one focus, and
// all keyboard and mouse events apply to both widgets. But not two focus
// indicators! The current solution has a focusInEvent and focusOutEvent
// reimplementation and a focusChanged() signal which does not connect
// to update() and requires scheduleUpdate() and still shows two focus
// indicators. Quite a hack. Find a better solution!

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 * 
 * @param event The corresponding resize event
 */
void WheelColorPicker::resizeEvent(QResizeEvent* event)
{
    SimpleColorWheel::resizeEvent(event);
    d_pointer->resizeChildWidget();
}

/** Convenience slot that calls update() on the base implementation.
 * 
 * @todo This slot exists as a workaround because connecting directly
 * didn’t work before using pimpl idiom. Now that we use pimpl idiom,
 * maybe it works without this class! */
void WheelColorPicker::WheelColorPickerPrivate::scheduleUpdate()
{
    q_pointer->update();
}

/** @brief Scale a rectangle to a given diagonal line length
 * 
 * @param oldRectangle the size of a rectangle
 * @param newDiagonal the desired new diagonal line length (distance from the
 * bottom left to the top right corner.
 * @returns the size of a scaled rectangle, that has the given diagonal line
 * length and preserves the original ratio between width and height - or an
 * invalid size if oldRectangle had a surface of 0. The result is rounded
 * the next smaller integer!
 */
QSize WheelColorPicker::WheelColorPickerPrivate::scaleRectangleToDiagonal(
    const QSize oldRectangle,
    const qreal newDiagonal
)
{
    if (oldRectangle.isEmpty()) {
        return QSize();
    }
    qreal ratioWidthPerHeight =
        static_cast<qreal>(oldRectangle.width()) / oldRectangle.height();
    // static_cast<int> will round down. This is intentional.
    int newHeight = static_cast<int>(
        sqrt(
            pow(newDiagonal, 2) / (1 + pow(ratioWidthPerHeight, 2))
        )
    );
    int newWidth = static_cast<int>(newHeight * ratioWidthPerHeight);
    return QSize(newWidth, newHeight);
}

void WheelColorPicker::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Home:
        case Qt::Key_End:
            /* These keys are guaranteed to be handled by
             * ChromaLightnessDiagram(). This guarantee is important.
             * If we would pass a key that is _not_ handled by
             * ChromaLightnessDiagram(), this event would return here
             * because ChromaLightnessDiagram() leaves treatment of
             * non-handled keys up to the parent widget, which
             * is _this_ widget. This would make an infinite recursion. */
            QCoreApplication::sendEvent(
                d_pointer->m_chromaLightnessDiagram,
                event
            );
            break;
        default:
            SimpleColorWheel::keyPressEvent(event);
            break;
    }
}

/** @brief Update the size of the child widget.
 *
 * Updates the size of the @ref ChromaLightnessDiagram child widget:
 * m_chromaLightnessDiagram. It stays in the interior of the color wheel. */
void WheelColorPicker::WheelColorPickerPrivate::resizeChildWidget()
{
    int diagonal = qMax(
        q_pointer->contentDiameter()
            - 2
            * (m_wheelThickness + q_pointer->border),
        0
    );
    // TODO Why is QSize(140, 100) a good choice? What gamuts exist? Up to
    // where goes chroma there?
    QSize newChromaLightnessDiagramSize = scaleRectangleToDiagonal(
        QSize(140, 100),
        diagonal
    );
    m_chromaLightnessDiagram->resize(newChromaLightnessDiagramSize);
    qreal radius = static_cast<qreal>(q_pointer->contentDiameter()) / 2;
    m_chromaLightnessDiagram->move(
        // TODO Does qRound make sense here? Does it the right
        // thing (pixel-wise)?
        qRound(radius - newChromaLightnessDiagramSize.width() / 2.0),
        qRound(radius - newChromaLightnessDiagramSize.height() / 2.0)
    );
}

FullColorDescription WheelColorPicker::currentColor()
{
    return d_pointer->m_chromaLightnessDiagram->color();
}

/** @brief Setter for currentColorRgb() property */
void WheelColorPicker::setCurrentColor(
    const FullColorDescription &newCurrentColor
)
{
    d_pointer->m_chromaLightnessDiagram->setColor(newCurrentColor);
    setHue(newCurrentColor.toLch().h);
}

// TODO Choose HLC x 50 1. Then push the Page-up button. Why does the chroma
// value change from 1 to 0?

}
