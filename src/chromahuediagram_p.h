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

#ifndef CHROMAHUEDIAGRAM_P_H
#define CHROMAHUEDIAGRAM_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/chromahuediagram.h"
#include "constpropagatingrawpointer.h"

#include "colorwheelimage.h"
#include "chromahueimage.h"
#include "lchvalues.h"

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaHueDiagram::ChromaHueDiagramPrivate final
{
public:
    ChromaHueDiagramPrivate(
        ChromaHueDiagram *backLink,
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace
    );
    /** @brief Default destructor
     * 
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaHueDiagramPrivate() noexcept = default;

    // Member variables
    /** @brief The border around the round diagram.
     * 
     * Measured in Device Independent Pixels: Pixels used by application
     * (user space), subject to scaling by the operating system or Qt. 
     * 
     * The border is the space where the surrounding color wheel and, where
     * applicable, the focus indicator is painted.
     * @sa @ref markerThickness */
    static constexpr qreal diagramBorder = 8 * markerThickness;
    /** @brief The image of the chroma-hue diagram itself. */
    ChromaHueImage m_chromaHueImage;
    /** @brief Internal storage of the @ref color() property */
    FullColorDescription m_color;
    /** @brief Position of the center of the diagram coordinate system
     * 
     * This value is measured in widget coordinates. */
    qreal m_diagramOffset = 0;
    /** @brief Holds if currently a mouse event is active or not.
     * 
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a @ref mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref mouseMoveEvent() will move the diagram’s color
     *   marker.
     * - Once a @ref mouseReleaseEvent() occurs, the value is set to
     *   <tt>false</tt>. Further mouse movements will not move the marker
     *   anymore. */
    bool m_isMouseEventActive = false;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @todo This should not be hard-coded sRGB. */
    qreal m_maxChroma = LchValues::srgbMaximumChroma;
    /** @brief The image of the color wheel. */
    ColorWheelImage m_wheelImage;
    /** @brief Diameter of the widget.
     * 
     * This is different from <tt>size()</tt>. It is the maximum possible
     * diameter that is available within the current <tt>size()</tt>. */
    int m_widgetDiameter = 0;

    // Member functions
    bool areImageCoordinatesWithinDiagramSurface(
        const QPoint imageCoordinates
    );
    QPointF fromImageCoordinatesToAB(const QPoint imageCoordinates);
    QPoint imageCoordinatesFromColor();
    void setColorFromImageCoordinates(const QPoint imageCoordinates);

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)
    
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaHueDiagram> q_pointer;
};

}

#endif // CHROMAHUEDIAGRAM_P_H
