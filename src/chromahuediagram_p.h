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
 *  implementation</em> idiom
 * 
 * @anchor MeasurementDetails <b>Measurement details</b>
 * 
 * This class deals simultaniously with various
 * coordinate systems and  unit of measurement.
 * 
 * <b>Units of measurement</b>
 * 
 * - <em>Device-indepentend pixel/coordinates</em> are the  unit of
 *   measurement for widgets, windows, screens, mouse events and so on in Qt.
 * - <em>Physical pixel/coordinates</em> are the unit that measures actual
 *   physical screen pixel.
 * 
 * The conversion factor between these two units of measurement is
 * <tt>QPaintDevice::devicePixelRatioF()</tt>. It is usually <tt>1</tt> on
 * classic low resolution screens, and bigger than <tt>1</tt> on high
 * resolution screens. See https://doc.qt.io/qt-6/highdpi.html for more
 * details on Qt’s High DPI support.
 * 
 * <b>Coordinate points versus pixel positions</b>
 * 
 * - <em>Coordinate points</em> are points in the mathematical sense, that
 *   means they have zero surface. They should always be represented by
 *   floating point data types; this is necessary to allow conversions
 *   without having too much rounding errors.
 * - <em>Pixel positions</em> describe the position of a particular pixel.
 *   Pixel are surfaces, not points. A pixel is a square of the width and
 *   length <tt>1</tt>. The pixel at position <tt>QPoint(x, y)</tt> is the
 *   square with the top-left edge at coordinate point <tt>QPoint(x, y)</tt>
 *   and the botton-right edge at coordinate point <tt>QPoint(x+1, y+1)</tt>.
 * 
 * Some functions (like mouse events work with pixel positions), other
 * functions (like antialiased floatting-point drawing operations) work
 * with coordinate points. It’s important to always distinguish correctly
 * these two different concepts. See https://doc.qt.io/qt-6/coordsys.html
 * for more details on Qt’s coordinate systems.
 * 
 * In this widget, when painting a pixel of the gamut, the color of the
 * pixel will be the color of the coordinate point at the center of the pixel.
 * So the pixel at position <tt>QPoint(x, y)</tt> gets the color that
 * corresponds to the coordinate point <tt>QPoint(x+0.5, y+0.5)</tt>.
 * Also, mouse events work with pixel position; so when reacting on mouse
 * events than the center of the given mouse event pixel position is
 * used to decide how to react on the mouse event. */
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
    /** @brief The image of the chroma-hue diagram itself. */
    ChromaHueImage m_chromaHueImage;
    /** @brief Internal storage of the @ref color() property */
    FullColorDescription m_color;
    /** @brief The border around the round diagram.
     * 
     * Measured in widget coordinates. 
     * 
     * The border is the space where the surrounding color wheel and, where
     * applicable, the focus indicator is painted.
     * @sa @ref handleOutlineThickness */
    static constexpr qreal m_diagramBorder = 8 * handleOutlineThickness;
    /** @brief The point that is the center of the diagram coordinate system.
     * 
     * This value is measured in widget coordinates. */
    qreal m_diagramOffset = 0;
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
     * This is the value that is on the border of the gray circle.
     * 
     * @todo This should not be hard-coded to sRGB. */
    qreal m_maxChroma = LchValues::srgbMaximumChroma;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief The image of the color wheel. */
    ColorWheelImage m_wheelImage;
    /** @brief Diameter of the widget.
     * 
     * Mesured in widget coordinates.
     * 
     * This is different from <tt>size()</tt>. It is the maximum possible
     * diameter that is available within the current <tt>size()</tt>. */
    int m_widgetDiameter = 0;

    // Member functions
    cmsCIELab fromWidgetPixelPositionToLab(const QPoint position);
    bool isWidgetPixelPositionWithinMouseSensibleCircle(
        const QPoint widgetCoordinates
    );
    void setColorFromWidgetPixelPosition(const QPoint position);
    QPointF widgetCoordinatesFromCurrentColor();

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)
    
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaHueDiagram> q_pointer;
};

}

#endif // CHROMAHUEDIAGRAM_P_H
