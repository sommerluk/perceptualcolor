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

#ifndef CHROMAHUEDIAGRAM_H
#define CHROMAHUEDIAGRAM_H

#include "PerceptualColor/abstractcirculardiagram.h"
#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor {

/** @brief A widget for selecting chroma and hue in LCh color space
 * 
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radial and the angle of the
 * LCh color space respectively the a axis and the b axis of the
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * Lab color model</a>) at a given lightness.
 * 
 * The widget allows to select a color within the specified gamut at the
 * given lightness. It reacts on mouse events and on keyboard events (see
 * @ref keyPressEvent() for details).
 * 
 * The circle form of the widget and the selection marker that always
 * indicates the distance from the center of the diagram, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radial
 * and angle.
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
 *   within the surrounding rectangle.
 * 
 * @todo The image is wrong: The hue indicator is not at the correct
 * position (at least at 150% screen scaling).
 * 
 * @todo Declare a property for @ref color()
 * 
 * @todo Example code: How to create the widget at a given
 * lightness.
 * 
 * @todo The image cache should be updated asynchronously (in its own thread
 * or even various own threads in parallel), but this should only be
 * triggered when the widget is visible, and not while the widget is
 * hidden. While waiting for the result, an empty image could be used.
 * It might be useful to provide first a low-resolution version, and only
 * later-on a high-resolution version.
 * 
 * @todo High-dpi support.
 * 
 * @todo Automatically scale the thickness of the wheel (and maybe even the
 * marker) with varying widget size?
 * 
 * @todo Support additional mouse buttons. For example, “forward” and
 * “backward” could be used to increase or decrease the radial, while
 * the angel is controlled by the mouse wheel.
 *         
 * @todo Provide RESET functions for all properties around the library? */
class ChromaHueDiagram : public AbstractCircularDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     * 
     * The widget allows the user to change the LCh chroma and the LCh hue
     * values. However, the LCh lightness value cannot be changed by the
     * user, but only by the programmer through this property.
     * 
     * @sa READ @ref color()
     * @sa WRITE @ref setColor()
     * @sa NOTIFY @ref colorChanged() */
    Q_PROPERTY(FullColorDescription color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(
        PerceptualColor::RgbColorSpace *colorSpace,
        QWidget *parent = nullptr
    );
    /** @brief Default destructor */
    virtual ~ChromaHueDiagram() override;
    /** @brief Getter for property @ref color
     *  @returns the property @ref color */
    FullColorDescription color() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setColor(const PerceptualColor::FullColorDescription &color);

Q_SIGNALS:
    /** @brief Notify signal for property @ref color().
     *  @param color the new color */
    void colorChanged(const PerceptualColor::FullColorDescription &color);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

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

}

#endif // CHROMAHUEDIAGRAM_H
