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

#ifndef CHROMALIGHTNESSDIAGRAM_P_H
#define CHROMALIGHTNESSDIAGRAM_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/chromalightnessdiagram.h"
#include "constpropagatingrawpointer.h"

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaLightnessDiagram::ChromaLightnessDiagramPrivate final
{
public:
    ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaLightnessDiagramPrivate() noexcept = default;

    /** @brief The border between the widget outer border and the
     * diagram itself.
     *
     * The diagram is not painted on the whole extend of the widget.
     * A border is left to allow that the selection handle can be painted
     * completely even when a pixel on the border of the diagram is
     * selected. The border is determined automatically, its value
     * depends on @ref handleRadius and @ref handleOutlineThickness.
     *
     * @sa updateBorder()
     */
    int m_border;
    /** @brief Internal storage of the chromaLightness() property */
//     QPointF m_chromaLightness;
    /** @brief Internal storage of the color() property */
    FullColorDescription m_color;
    /** @brief A cache for the diagram as QImage. @sa updateDiagramCache() */
    QImage m_diagramImage;
    /** True if the m_diagramImage cache is up-to-date. False otherwise.
     * @sa m_diagramImage
     * @sa updateDiagramCache */
    bool m_diagramCacheReady = false;
    /** @brief If a mouse event is active
     *
     * Holds if currently a mouse event is active or not.
     * @sa mousePressEvent()
     * @sa mouseMoveEvent()
     * @sa mouseReleaseEvent()
     */
    bool m_mouseEventActive;
    /** @brief Pointer to RgbColorSpace() object */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;

    QImage generateDiagramImage(
        const qreal imageHue,
        const QSize imageSize) const;
    QPoint currentImageCoordinates();
    QPointF fromImageCoordinatesToChromaLightness(
        const QPoint imageCoordinates
    );
    QPoint fromWidgetCoordinatesToImageCoordinates(
        const QPoint widgetCoordinates
    ) const;
    bool imageCoordinatesInGamut(const QPoint imageCoordinates);
    static QPoint nearestNeighborSearch(
        const QPoint originalPoint,
        const QImage &image
    );
    void updateDiagramCache();
    void setImageCoordinates(const QPoint newImageCoordinates);
    void updateBorder();

private:
    Q_DISABLE_COPY(ChromaLightnessDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaLightnessDiagram> q_pointer;

};

}

#endif // CHROMALIGHTNESSDIAGRAM_P_H
