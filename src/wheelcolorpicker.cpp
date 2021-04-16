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

#include "perceptualcolorlib_internal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h"

#include "colorwheel_p.h"

#include <math.h>

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

namespace PerceptualColor
{
/** @brief Constructor */
WheelColorPicker::WheelColorPicker(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new WheelColorPickerPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;
    d_pointer->m_ColorWheel = new ColorWheel(colorSpace, this);
    d_pointer->m_chromaLightnessDiagram = new ChromaLightnessDiagram(colorSpace, d_pointer->m_ColorWheel);
    d_pointer->m_ColorWheel->setFocusProxy(d_pointer->m_chromaLightnessDiagram);
    // Thought apparently Qt is putting newly added child widgets always
    // visually in front of previously added child widgets, this behaviour
    // is undocumented. Therefore, we call raise() to be sure that actually
    // this child widget is visually in front of the others:
    d_pointer->m_chromaLightnessDiagram->raise();
    d_pointer->resizeChildWidgets();
    d_pointer->m_chromaLightnessDiagram->setHue(d_pointer->m_chromaLightnessDiagram->currentColor().h);
    d_pointer->m_chromaLightnessDiagram->setFocusPolicy(Qt::FocusPolicy::ClickFocus
                                                        // TODO Why not just inherit TabFocus? But apparently it works! Why?
    );
    connect(d_pointer->m_ColorWheel, &ColorWheel::hueChanged, d_pointer->m_chromaLightnessDiagram, &ChromaLightnessDiagram::setHue);
    connect(d_pointer->m_chromaLightnessDiagram, &ChromaLightnessDiagram::currentColorChanged, this, [this](const PerceptualColor::LchDouble &newCurrentColor) { Q_EMIT currentColorChanged(newCurrentColor); });
    connect(
        // QWidget’s constructor requires a QApplication object. As this is
        // a class derived from QWidget, calling qApp is save.
        qApp,
        &QApplication::focusChanged,
        d_pointer.get(), // Without .get() apparently connect() does not work…
        &WheelColorPickerPrivate::handleFocusChanged);
}

/** @brief Default destructor */
WheelColorPicker::~WheelColorPicker() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
WheelColorPicker::WheelColorPickerPrivate::WheelColorPickerPrivate(WheelColorPicker *backLink)
    : q_pointer(backLink)
{
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event
 */
void WheelColorPicker::resizeEvent(QResizeEvent *event)
{
    AbstractDiagram::resizeEvent(event);
    d_pointer->resizeChildWidgets();
}

/** TODO Alternative: Install an event filter
 * on @ref WheelColorPickerPrivate::m_chromaLightnessDiagram. The event
 * filter could be either @ref WheelColorPicker or
 * @ref WheelColorPickerPrivate (the last case means that
 * @ref WheelColorPickerPrivate would still have to inherit from
 * <tt>QObject</tt>). What would be the best solution? */
void WheelColorPicker::WheelColorPickerPrivate::handleFocusChanged(QWidget *old, QWidget *now)
{
    if ((old == m_chromaLightnessDiagram) || (now == m_chromaLightnessDiagram)) {
        m_ColorWheel->update();
    }
}

// TODO A click in the inner of the color wheel (also at emtpy places)
// should trigger to get a focus.

// TODO This widget and its child should be a unit: Only one focus, and
// all keyboard and mouse events apply to both widgets. But not two focus
// indicators! The current solution still shows two focus
// indicators. Find a better solution!

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
QSize WheelColorPicker::WheelColorPickerPrivate::scaleRectangleToDiagonal(const QSize oldRectangle, const qreal newDiagonal)
{
    if (oldRectangle.isEmpty()) {
        return QSize();
    }
    qreal ratioWidthPerHeight = static_cast<qreal>(oldRectangle.width()) / oldRectangle.height();
    // static_cast<int> will round down. This is intentional.
    int newHeight = static_cast<int>(sqrt(pow(newDiagonal, 2) / (1 + pow(ratioWidthPerHeight, 2))));
    int newWidth = static_cast<int>(newHeight * ratioWidthPerHeight);
    return QSize(newWidth, newHeight);
}

/** @brief Update the size of the child widget.
 *
 * Updates the size of the @ref ChromaLightnessDiagram child widget:
 * m_chromaLightnessDiagram. It stays in the interior of the color wheel. */
void WheelColorPicker::WheelColorPickerPrivate::resizeChildWidgets()
{
    m_ColorWheel->resize(q_pointer->size());
    int diagonal = qMax(m_ColorWheel->d_pointer->contentDiameter() - 2 * (q_pointer->gradientThickness() + m_ColorWheel->d_pointer->border()), 0);
    // TODO Why is QSize(140, 100) a good choice? What gamuts exist? Up to
    // where goes chroma there?
    QSize newChromaLightnessDiagramSize = scaleRectangleToDiagonal(QSize(140, 100), diagonal);
    m_chromaLightnessDiagram->resize(newChromaLightnessDiagramSize);
    qreal radius = static_cast<qreal>(m_ColorWheel->d_pointer->contentDiameter()) / 2;
    m_chromaLightnessDiagram->move(
        // TODO Does qRound make sense here? Does it the right
        // thing (pixel-wise)?
        qRound(radius - newChromaLightnessDiagramSize.width() / 2.0),
        qRound(radius - newChromaLightnessDiagramSize.height() / 2.0));
}

LchDouble WheelColorPicker::currentColor() const
{
    return d_pointer->m_chromaLightnessDiagram->currentColor();
}

/** @brief Setter for currentColorRgb() property */
void WheelColorPicker::setCurrentColor(const LchDouble &newCurrentColor)
{
    d_pointer->m_chromaLightnessDiagram->setCurrentColor(newCurrentColor);
    d_pointer->m_ColorWheel->setHue(d_pointer->m_chromaLightnessDiagram->currentColor().h);
}

QSize WheelColorPicker::minimumSizeHint() const
{
    // TODO Actually implement this!
    return AbstractDiagram::minimumSizeHint();
}

QSize WheelColorPicker::sizeHint() const
{
    // TODO Actually implement this!
    return AbstractDiagram::sizeHint();
}

// TODO Choose HLC x 50 1. Then push the Page-up button. Why does the chroma
// value change from 1 to 0?

} // namespace PerceptualColor
