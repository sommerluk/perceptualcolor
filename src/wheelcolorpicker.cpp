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
#include "PerceptualColor/wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h"

// TODO Can’t we avoid to include colorwheel_p.h?
#include "colorwheel_p.h"

#include "lchvalues.h"

#include <math.h>

#include <QApplication>
#include <QDebug>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color spaces within this widget should operate.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
WheelColorPicker::WheelColorPicker(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new WheelColorPickerPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;
    d_pointer->m_colorWheel = new ColorWheel(colorSpace, this);
    d_pointer->m_chromaLightnessDiagram = new ChromaLightnessDiagram(
        // Same color space for this widget:
        colorSpace,
        // This widget is smaller than the color wheel. It will be a child
        // of the color wheel, so that missed mouse or key events will be
        // forwarded to the parent widget (color wheel).
        d_pointer->m_colorWheel);
    d_pointer->m_colorWheel->setFocusProxy(d_pointer->m_chromaLightnessDiagram);
    d_pointer->resizeChildWidgets();

    connect(
        // changes on the color wheel trigger a change in the AbstractDiagram
        d_pointer->m_colorWheel,
        &ColorWheel::hueChanged,
        d_pointer->m_chromaLightnessDiagram,
        &ChromaLightnessDiagram::setHue);
    connect(d_pointer->m_chromaLightnessDiagram,
            &ChromaLightnessDiagram::currentColorChanged,
            this,
            // As value is stored anyway within ChromaLightnessDiagram member,
            // it’s enough to just emit the corresponding signal of this class:
            &WheelColorPicker::currentColorChanged);
    connect(
        // QWidget’s constructor requires a QApplication object. As this
        // is a class derived from QWidget, calling qApp is save here.
        qApp,
        &QApplication::focusChanged,
        d_pointer.get(), // Without .get() apparently connect() won’t work…
        &WheelColorPickerPrivate::handleFocusChanged);

    // Initial color
    setCurrentColor(LchValues::srgbVersatileInitialColor);
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

/** Repaint @ref m_colorWheel when focus changes
 * on @ref m_chromaLightnessDiagram
 *
 * @ref m_chromaLightnessDiagram is the focus proxy of @ref m_colorWheel.
 * Both show a focus indicator when keyboard focus is active. But
 * apparently @ref m_colorWheel does not always repaint when focus
 * changes. Therefore, this slot can be connected to the <tt>qApp</tt>’s
 * <tt>focusChanged()</tt> signal to make sure that the repaint works.
 *
 * @note It might be an alternative to write an event filter
 * for @ref m_chromaLightnessDiagram to do the same work. The event
 * filter could be either @ref WheelColorPicker or
 * @ref WheelColorPickerPrivate (the last case means that
 * @ref WheelColorPickerPrivate would still have to inherit from
 * <tt>QObject</tt>). But that would probably be more compicate… */
void WheelColorPicker::WheelColorPickerPrivate::handleFocusChanged(QWidget *old, QWidget *now)
{
    if ((old == m_chromaLightnessDiagram) || (now == m_chromaLightnessDiagram)) {
        m_colorWheel->update();
    }
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event */
void WheelColorPicker::resizeEvent(QResizeEvent *event)
{
    AbstractDiagram::resizeEvent(event);
    d_pointer->resizeChildWidgets();
}

// TODO xxx revision starts here

/** @brief Scale a rectangle to a given diagonal line length
 *
 * @param oldRectangle the size of a rectangle
 * @param newDiagonal the desired new diagonal line length (distance from the
 * bottom left to the top right corner.
 * @returns the size of a scaled rectangle, that has the given diagonal line
 * length and preserves the original ratio between width and height - or an
 * invalid size if oldRectangle had a surface of 0. The result is rounded
 * the next smaller integer! */
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

/** @brief Update the size and the position of the child widgets. */
void WheelColorPicker::WheelColorPickerPrivate::resizeChildWidgets()
{
    m_colorWheel->resize(q_pointer->size());
    int diagonal = qMax(m_colorWheel->d_pointer->contentDiameter() - 2 * (q_pointer->gradientThickness() + m_colorWheel->d_pointer->border()), 0);
    // TODO Why is QSize(140, 100) a good choice? What gamuts exist? Up to
    // where goes chroma there?
    QSize newChromaLightnessDiagramSize = scaleRectangleToDiagonal(QSize(140, 100), diagonal);
    m_chromaLightnessDiagram->resize(newChromaLightnessDiagramSize);
    qreal radius = static_cast<qreal>(m_colorWheel->d_pointer->contentDiameter()) / 2;
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

/** @brief Setter for the @ref currentColor() property.
 *
 * @param newCurrentColor the new color */
void WheelColorPicker::setCurrentColor(const LchDouble &newCurrentColor)
{
    // The following line will also emit the signal of this class:
    d_pointer->m_chromaLightnessDiagram->setCurrentColor(newCurrentColor);
    d_pointer->m_colorWheel->setHue(d_pointer->m_chromaLightnessDiagram->currentColor().h);
}

/** @brief Recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref sizeHint() */
QSize WheelColorPicker::minimumSizeHint() const
{
    // TODO Actually implement this!
    return AbstractDiagram::minimumSizeHint();
}

/** @brief Recommmended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize WheelColorPicker::sizeHint() const
{
    // TODO Actually implement this!
    return AbstractDiagram::sizeHint();
}

} // namespace PerceptualColor
