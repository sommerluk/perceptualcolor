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

#ifndef CHROMAHUEDIAGRAM_P_H
#define CHROMAHUEDIAGRAM_P_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "PerceptualColor/chromahuediagram.h"

#include "chromahueimage.h"
#include "colorwheelimage.h"
#include "constpropagatingrawpointer.h"
#include "lchvalues.h"

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom
 *
 * @anchor ChromaHueMeasurement <b>Measurement details:</b> The general
 * @ref MeasurementDetails "notes about measurement" apply also to this widget.
 * Furthermore, in this widget, when painting a pixel of the gamut, the color
 * of the pixel will be the color of the coordinate point at the center of the
 * pixel. So the pixel at position <tt>QPoint(x, y)</tt> gets the color that
 * corresponds to the coordinate point <tt>QPoint(x+0.5, y+0.5)</tt>.
 * Also, mouse events work with pixel position; so when reacting on mouse
 * events than it’s the center of the given mouse event pixel position that is
 * considered when processing the mouse event. */
class ChromaHueDiagram::ChromaHueDiagramPrivate final
{
public:
    ChromaHueDiagramPrivate(ChromaHueDiagram *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaHueDiagramPrivate() noexcept = default;

    // Member variables
    /** @brief The image of the chroma-hue diagram itself. */
    ChromaHueImage m_chromaHueImage;
    /** @brief Internal storage of the @ref currentColor() property */
    LchDouble m_currentColor;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a @ref mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref mouseMoveEvent() will move the diagram’s
     *   color handle.
     * - Once a @ref mouseReleaseEvent() occurs, the value is set to
     *   <tt>false</tt>. Further mouse movements will not move the handle
     *   anymore. */
    bool m_isMouseEventActive = false;
    /** @brief The maximum chroma value.
     *
     * This is the chroma value that is on the border of the gray circle.
     *
     * @todo This should not be hard-coded to sRGB. */
    qreal m_maxChroma = LchValues::srgbMaximumChroma;
    /** @brief Pointer to @ref RgbColorSpace object used to describe the
     * color space. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief The image of the color wheel. */
    ColorWheelImage m_wheelImage;

    // Member functions
    int diagramBorder() const;
    QPointF diagramCenterInWidgetCoordinates() const;
    qreal diagramOffset() const;
    cmsCIELab fromWidgetPixelPositionToLab(const QPoint position) const;
    bool isWidgetPixelPositionWithinMouseSensibleCircle(const QPoint widgetCoordinates) const;
    void setColorFromWidgetPixelPosition(const QPoint position);
    QPointF widgetCoordinatesFromCurrentColor() const;

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaHueDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // CHROMAHUEDIAGRAM_P_H
