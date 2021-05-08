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

#ifndef CHROMALIGHTNESSDIAGRAM_H
#define CHROMALIGHTNESSDIAGRAM_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

#include "PerceptualColor/constpropagatinguniquepointer.h"

#include "PerceptualColor/abstractdiagram.h"
#include "PerceptualColor/lchdouble.h"

namespace PerceptualColor
{
class RgbColorSpace;

/** @internal
 *
 * @brief A widget that displays a chroma-lightness diagram.
 *
 * This widget displays a chroma-lightness diagram for a given hue.
 *
 * @image html ChromaLightnessDiagram.png "ChromaLightnessDiagram" width=250
 *
 * The widget shows the chroma-lightness diagram at the whole widget extend.
 * - Vertically the lightness from 0 (bottom) to 100 (top).
 * - Horizontally the chroma from 0 (left) to a higher value (right). The same
 *   scale is used like for the vertical axis: So if the widget size is a
 *   square, both chroma and lightness range from 0 to 100. If the widget
 *   width is twice the height, the lightness ranges from 0 to 100
 *   and the chroma ranges from 0 to 200.
 *
 * @note This widget <em>always</em> accepts focus by a mouse
 * click within the displayed gamut. This happens regardless of the
 * <tt>QWidget::focusPolicy</tt> property. If you set the
 * <tt>QWidget::focusPolicy</tt> property to a value that accepts
 * focus by mouse click, the focus will not only be accepted for
 * clicks within the actual display gamut, but also for clicks
 * within the surrounding rectangle. TODO Does this make sense?
 *
 * @note This class is not part of the public API because its interface
 * is not polished enough. Notably it does not automatically scale the
 * diagram to fit a given gamut (means: to fit up to a given maximum
 * chroma). Even if we would fix this: We would need a public API
 * that is widthForHeight-dependent to allow the library user to
 * comfortably make use of this! */
class ChromaLightnessDiagram : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * The widget allows the user to change the LCh chroma and the
     * LCh lightness values. However, the LCh hue value cannot be
     * changed by the user, but only by the programmer through this property.
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged() */
    Q_PROPERTY(PerceptualColor::LchDouble currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

public:
    Q_INVOKABLE explicit ChromaLightnessDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~ChromaLightnessDiagram() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    PerceptualColor::LchDouble currentColor() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *  @param newCurrentColor the new current color */
    void currentColorChanged(const PerceptualColor::LchDouble &newCurrentColor);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaLightnessDiagram)

    class ChromaLightnessDiagramPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ChromaLightnessDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaLightnessDiagramPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestChromaLightnessDiagram;

    /** @internal
     * @brief Internal friend declaration.
     *
     * This class is used as child class in @ref WheelColorPicker.
     * There is a tight collaboration. */
    friend class WheelColorPicker;
};

} // namespace PerceptualColor

#endif // CHROMALIGHTNESSDIAGRAM_H
