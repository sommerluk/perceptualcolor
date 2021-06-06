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

#include "chromalightnessdiagram_p.h"
#include "colorwheel_p.h"
#include "helper.h"
#include "lchvalues.h"

#include <math.h>

#include <QApplication>
#include <QDebug>
#include <QtMath>

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
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
        // changes on the color wheel trigger a change in the inner diagram
        d_pointer->m_colorWheel,
        &ColorWheel::hueChanged,
        this,
        [this](const qreal newHue) {
            LchDouble lch = d_pointer->m_chromaLightnessDiagram->currentColor();
            lch.h = newHue;
            // We have to be sure that the color is in-gamut also for the
            // new hue. If it is not, we adjust it:
            lch = d_pointer->m_rgbColorSpace->nearestInGamutColorByAdjustingChromaLightness(lch);
            d_pointer->m_chromaLightnessDiagram->setCurrentColor(lch);
        });
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
    setCurrentColor(
        // Though LchValues::srgbVersatileInitialColor() is expected to
        // be in-gamut, its more secure to guarantee this explicitly:
        d_pointer->m_rgbColorSpace->nearestInGamutColorByAdjustingChromaLightness(
            // Default sRGB initial color:
            LchValues::srgbVersatileInitialColor()));
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

/** @brief Calculate the optimal size for the inner diagram.
 *
 * @returns The maximum possible size of the diagram within the
 * inner part of the color wheel. With floating point precision.
 * Measured in <em>device-independant pixels</em>. */
QSizeF WheelColorPicker::WheelColorPickerPrivate::optimalChromaLightnessDiagramSize() const
{
    /** The outer dimensions of the widget are a rectangle within a
     * circumscribed circled, which is the inner border of the color wheel.
     *
     * The widget size is composed by the size of the diagram itself and
     * the size of the borders. The border size is fixed; only the diagram
     * size can vary.
     *
     * Known variables:
     * | variable     | comment                          | value                              |
     * | :----------- | :------------------------------- | :--------------------------------- |
     * | r            | relation b ÷ a                   | maximum lightness ÷ maximum chroma |
     * | h            | horizontial shift                | left + right diagram border        |
     * | v            | vertial shift                    | top + bottom diagram border        |
     * | d            | diameter of circumscribed circle | inner diameter of the color wheel  |
     * | b            | diagram height                   | a × r                              |
     * | widgetWidth  | widget width                     | a + h                              |
     * | widgetHeight | widget height                    | b + v                              |
     * | a            | diagram width                    | ?                                  |
     */
    const qreal r = 100.0 / m_rgbColorSpace->maximumChroma();
    const qreal h = m_chromaLightnessDiagram->d_pointer->leftBorderPhysical() + m_chromaLightnessDiagram->d_pointer->defaultBorderPhysical();
    const qreal v = 2 * m_chromaLightnessDiagram->d_pointer->defaultBorderPhysical();
    const qreal d = m_colorWheel->d_pointer->innerDiameter();

    /** We can calculate <em>a</em> because right-angled triangle
     * with <em>a</em> and with <em>b</em> as legs/catheti will have
     * has hypothenuse the diameter of the circumscribed circle:
     *
     * <em>[The following formula requires a working Internet connection
     * to be displayed.]</em>
     *
     * @f[
        \begin{align}
            widgetWidth²
            + widgetHeight²
            = & d²
        \\
            (a+h)²
            + (b+v)²
            = & d²
        \\
            (a+h)²
            + (ra+v)²
            = & d²
        \\
            a²
            + 2ah
            + h²
            + r²a²
            + 2rav
            + v²
            = & d²
        \\
            a²
            + r²a²
            + 2ah
            + 2rav
            + h²
            + v²
            = & d²
        \\
            (1+r²)a²
            + 2a(h+rv)
            + (h²+v²)
            = & d²
        \\
            a²
            + 2a\frac{h+rv}{1+r²}
            + \frac{h²+v²}{1+r²}
            = & \frac{d²}{1+r²}
        \\
            a²
            + 2a\frac{h+rv}{1+r²}
            + \left(\frac{h+rv}{1+r²}\right)^{2}
            - \left(\frac{h+rv}{1+r²}\right)^{2}
            + \frac{h²+v²}{1+r²}
            = &  \frac{d²}{1+r²}
        \\
            \left(a+\frac{h+rv}{1+r²}\right)^{2}
            - \left(\frac{h+rv}{1+r²}\right)^{2}
            + \frac{h²+v²}{1+r²}
            = & \frac{d²}{1+r²}
        \\
            \left(a+\frac{h+rv}{1+r²}\right)^{2}
            = & \frac{d²}{1+r²}
            + \left(\frac{h+rv}{1+r²}\right)^{2}
            - \frac{h²+v²}{1+r²}
        \\
            a
            + \frac{h+rv}{1+r²}
            = & \sqrt{
                \frac{d²}{1+r²}
                + \left(\frac{h+rv}{1+r²}\right)^{2}
                -\frac{h²+v²}{1+r²}
            }
        \\
            a
            = & \sqrt{
                \frac{d²}{1+r²}
                + \left(\frac{h+rv}{1+r²}\right)^{2}
                - \frac{h²+v²}{1+r²}
            }
            - \frac{h+rv}{1+r²}
        \end{align}
     * @f] */
    const qreal x = (1 + qPow(r, 2)); // x = 1 + r²
    const qreal a =
        // The square root:
        qSqrt(
            // First fraction:
            d * d / x
            // Second fraction:
            + qPow((h + r * v) / x, 2)
            // Thierd fraction:
            - (h * h + v * v) / x)
        // The part after the square root:
        - (h + r * v) / x;
    const qreal b = r * a;

    return QSizeF(a + h, // width
                  b + v  // height
    );
}

/** @brief Update the geometry of the child widgets.
 *
 * This widget does <em>not</em> use layout management for its child widgets.
 * Therefore, this function should be called on all resize events of this
 * widget.
 *
 * @post The geometry (size and the position) of the child widgets are
 * adapted according to the current size of <em>this</em> widget itself. */
void WheelColorPicker::WheelColorPickerPrivate::resizeChildWidgets()
{
    // Set new geometry of color wheel. Only the size changes, while the
    // position (which is 0, 0) remains always unchanged.
    m_colorWheel->resize(q_pointer->size());

    // Calculate new size for chroma-lightness-diagram
    const QSizeF widgetSize = optimalChromaLightnessDiagramSize();

    // Calculate new top-left corner position for chroma-lightness-diagram
    // (relative to parent widget)
    const qreal radius = m_colorWheel->maximumWidgetSquareSize() / 2.0;
    const QPointF widgetTopLeftPos(
        // x position
        radius - widgetSize.width() / 2.0,
        // y position:
        radius - widgetSize.height() / 2.0);

    // Correct the new geometry of chroma-lightness-diagram to fit into
    // an integer raster.
    QRectF diagramGeometry(widgetTopLeftPos, widgetSize);
    // We have to round to full integers, so that our integar-based rectangle
    // does not exceed the dimensions of the floating-point rectangle.
    // Round to bigger coordinates for top-left corner:
    diagramGeometry.setLeft(ceil(diagramGeometry.left()));
    diagramGeometry.setTop(ceil(diagramGeometry.top()));
    // Round to smaller coordinates for bottom-right corner:
    diagramGeometry.setRight(floor(diagramGeometry.right()));
    diagramGeometry.setBottom(floor(diagramGeometry.bottom()));
    // TODO The rounding has probably changed the ratio (b ÷ a) of the
    // diagram itself with the chroma-hue widget. Therefore, maybe a little
    // bit of gammut is not visible at the right of the diagram. There
    // might be two possibilities to solve this: Either ChromaLightnessDiagram
    // gets support for scaling to user-defined maximum chroma (unlikely)
    // or we implement it here, just by reducing a little bit the height
    // of the widget until the full gamut gets in (easier).

    // Apply new geometry
    m_chromaLightnessDiagram->setGeometry(diagramGeometry.toRect());
}

// No documentation here (documentation of properties
// and its getters are in the header)
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

    // Avoid that setting the new hue will move the color into gamut.
    // (As documented, this function accepts happyly out-of-gamut colors.)
    QSignalBlocker myBlocker(d_pointer->m_colorWheel);
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
    const QSizeF minimumDiagramSize =
        // Get the mimimum size of the chroma-lightness widget.
        d_pointer->m_chromaLightnessDiagram->minimumSizeHint()
        // We have to fit this in a widget pixel raster. But the perfect
        // position might be between two integer coordinates. We might
        // have to shift up to 1 pixel at each of the four margins.
        + QSize(2, 2);
    const int diameterForMinimumDiagramSize =
        // The minimum inner diameter of the color wheel has
        // to be equal (or a little bit bigger) than the
        // diagonal through the chroma-lightness widget.
        qCeil(
            // c = √(a² + b²)
            qSqrt(qPow(minimumDiagramSize.width(), 2) + qPow(minimumDiagramSize.height(), 2)))
        // Add size for the color wheel gradient
        + d_pointer->m_colorWheel->gradientThickness()
        // Add size for the border around the color wheel gradient
        + d_pointer->m_colorWheel->d_pointer->border();
    // Necessary size for this widget so that the diagram fits:
    const QSize sizeForMinimumDiagramSize(diameterForMinimumDiagramSize, // x
                                          diameterForMinimumDiagramSize  // y
    );

    return sizeForMinimumDiagramSize
        // Expand to the minimumSizeHint() of the color wheel itself
        .expandedTo(d_pointer->m_colorWheel->minimumSizeHint())
        // Expand to the global minimum size for GUI elements
        .expandedTo(QApplication::globalStrut());
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
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

} // namespace PerceptualColor
