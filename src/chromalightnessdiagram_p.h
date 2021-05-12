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

#ifndef CHROMALIGHTNESSDIAGRAM_P_H
#define CHROMALIGHTNESSDIAGRAM_P_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "chromalightnessdiagram.h"

#include "chromalightnessimage.h"
#include "constpropagatingrawpointer.h"

#include <QImage>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaLightnessDiagram::ChromaLightnessDiagramPrivate final
{
public:
    ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaLightnessDiagramPrivate() noexcept = default;

    // Member variables
    /** @brief The image of the chroma-lightness diagram itself. */
    ChromaLightnessImage m_chromaLightnessImage;
    /** @brief Internal storage of the @ref currentColor property */
    LchDouble m_currentColor;
    /** @brief The border between the widget outer top, right and bottom
     * border and the diagram itself.
     *
     * The diagram is not painted on the whole extend of the widget.
     * A border is left to allow that the selection handle can be painted
     * completely even when a pixel on the border of the diagram is
     * selected.
     *
     * This is the value for the top, right and bottom border. For the left
     * border, see @ref m_leftBorder instead.
     *
     * Measured in <em>device-independant pixels</em>. */
    const qreal m_defaultBorder = q_pointer->handleRadius() + q_pointer->handleOutlineThickness() / 2.0;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a @ref mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref mouseMoveEvent() will move the diagram’s
     *   color handle.
     * - Once a @ref mouseReleaseEvent() occurs, the value is set to
     *   <tt>false</tt>. Further mouse movements will not move the handle
     *   anymore.
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the whole widget. However, <em>this</em> widget is meant as a
     * circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false;
    /** @brief The left border between the widget outer left border and the
     * diagram itself.
     *
     * The diagram is not painted on the whole extend of the widget.
     * A border is left to allow that the selection handle can be painted
     * completely even when a pixel on the border of the diagram is
     * selected.
     *
     * This is the value for the left border. For the other three borders,
     * see @ref m_defaultBorder instead.
     *
     * Measured in <em>device-independant pixels</em>. */
    const qreal m_leftBorder = qMax<qreal>(
        // First candidate: normal border used in this widget, plus
        // thickness of the focus indicator.
        m_defaultBorder + q_pointer->handleOutlineThickness(),
        // Second candidate: Generally recommended value for focus indicator
        q_pointer->spaceForFocusIndicator());
    /** @brief Pointer to RgbColorSpace() object */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;

    // Member functions
    QPoint currentImageCoordinates();
    QPointF fromImageCoordinatesToChromaLightness(const QPoint imageCoordinates);
    QPoint fromWidgetCoordinatesToImageCoordinates(const QPoint widgetCoordinates) const;
    bool imageCoordinatesInGamut(const QPoint imageCoordinates);
    static QPoint nearestNeighborSearch(const QPoint originalPoint, const QImage &image);
    void setCurrentColorFromImageCoordinates(const QPoint newImageCoordinates);

private:
    Q_DISABLE_COPY(ChromaLightnessDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaLightnessDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // CHROMALIGHTNESSDIAGRAM_P_H
