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

#ifndef CHROMAHUEDIAGRAM_H
#define CHROMAHUEDIAGRAM_H

#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/perceptualcolorlib_global.h"

#include "PerceptualColor/abstractdiagram.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor
{
/** @brief A widget for selecting chroma and hue in LCh color space
 *
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radial and the angle of the
 * LCh color space respectively the a axis and the b axis of the
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * Lab color model</a>) at a given lightness.
 *
 * The widget allows the user to select a color (chroma and hue) within the
 * specified gamut at a given lightness. It reacts on mouse events and on
 * keyboard events (see @ref keyPressEvent() for details).
 *
 * The form of the selection handle (that always indicates the distance from
 * the center of the diagram) and the circular form of the widget, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radial
 * and angle.
 *
 * Usage example: @snippet test/testchromahuediagram.cpp instanciate
 *
 * @note This widget <em>always</em> accepts focus by a mouse click within
 * the circle. This happens regardless of the <tt>QWidget::focusPolicy</tt>
 * property:
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that does not accept focus by mouse click, the focus
 *   will nevertheless be accepted for clicks within the actual circle.
 *   (This is the default behavior.)
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that accepts focus by mouse click, the focus will not only be
 *   accepted for clicks within the actual circle, but also for clicks
 *   anyware within the (rectangular) widget.
 *
 * @todo Example code: How to create the widget at a given
 * lightness.
 *
 * @todo Allow to touch the widget on the color wheel (and have a reaction).
 *
 * @todo Use a cross cursor for better usability: The cross cursor indicates
 * to the user that an area can be clicked in. Do it only within the gamut
 * (where the color handle can actually go) or in the hole gray circle,
 * which is the mouse sensitive area (but out of the gamut the color
 * handle cannot follow)?
 *
 * @todo Support additional mouse buttons. For example, “forward” and
 * “backward” could be used to increase or decrease the radial.
 *
 * @todo What if black or white are out of gamut on L=0.1 or L=99.9? Where
 * are the handles placed? Visible or invisible? How to react? Should
 * there be always a pixel in the middle that is visible (black or white)
 * even when out of gamut? */
class ChromaHueDiagram : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * The widget allows the user to change the LCh chroma and the LCh hue
     * values. However, the LCh lightness value cannot be changed by the
     * user, but only by the programmer through this property.
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged() */
    Q_PROPERTY(LchDouble currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~ChromaHueDiagram() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    LchDouble currentColor() const;
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
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaHueDiagram)

    class ChromaHueDiagramPrivate;
    /** @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ChromaHueDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaHueDiagramPrivate> d_pointer;

    /** @brief Only for unit tests. */
    friend class TestChromaHueDiagram;
};

} // namespace PerceptualColor

#endif // CHROMAHUEDIAGRAM_H
