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

#include <QPointer>

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaHueDiagram::ChromaHueDiagramPrivate final
{
public:
    /** @brief Constructor
     * 
     * @param backLink Pointer to the object to which <em>this</em> object
     * is attached to. */
    ChromaHueDiagramPrivate(ChromaHueDiagram *backLink);
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
    /** @brief Internal storage of the @ref color() property */
    FullColorDescription m_color;
    /** Holds whether or not @ref m_diagramImage() is up-to-date.
     *  @sa @ref updateDiagramCache() */
    bool m_diagramCacheReady = false;
    /** @brief A cache for the diagram as QImage. Might be outdated.
     *  @sa @ref updateDiagramCache()
     *  @sa @ref m_diagramCacheReady */
    QImage m_diagramImage;
    /** @brief Position of the center of the diagram coordinate system
     * 
     * This value is measured in widget coordinates. */
    qreal m_diagramOffset = 0;
    /** @brief Diameter of the widget.
     * 
     * This is different from <tt>size()</tt>. It is the maximum possible
     * diameter that is available within the current <tt>size()</tt>. */
    int m_widgetDiameter = 0;
    /** @todo This should not be hard-coded sRGB. */
    qreal m_maxChroma = Helper::LchDefaults::maxSrgbChroma;
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
    QPointer<RgbColorSpace> m_rgbColorSpace;
    /** Holds whether or not @ref m_wheelImage() is up-to-date.
     *  @sa @ref updateWheelCache() */
    bool m_isWheelCacheReady = false;
    /** @brief A cache for the wheel as QImage. Might be outdated.
     *  @sa @ref updateWheelCache()
     *  @sa @ref m_isWheelCacheReady */
    QImage m_wheelImage;

    // Member functions
    bool areImageCoordinatesWithinDiagramSurface(
        const QPoint imageCoordinates
    );
    QPoint imageCoordinatesFromColor();
    QPointF fromImageCoordinatesToAB(const QPoint imageCoordinates);
    static QImage generateDiagramImage(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const qreal border,
        const QColor backgroundColor
    );
    static QImage generateDiagramImage2(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    static QImage generateDiagramImage3(
        const RgbColorSpace *colorSpace,
        const int imageSize,
        const qreal maxChroma,
        const qreal lightness,
        const int border
    );
    void setColorFromImageCoordinates(const QPoint imageCoordinates);
    void updateDiagramCache();
    void updateWheelCache();

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)
    
    // TODO WARNING This may not be a unique pointer, but must be a normal
    // pointer, because the pointer is not owning… But… it should
    // nevertheless be const-propagating!!
    // And: Find a const-propagating-pointer-solution which allows calling
    // private slots, which is missing at AlphaSelector. But it’s better to
    // test private slots wich another class then AlphaSelector, because
    // AlphaSelector has overloaded signals, which makes everything more
    // complicate. Also: Couldn’t we just make the connect() call within
    // the private class instead of the public class to get around this
    // problem?
    ChromaHueDiagram *q_pointer;
};

}

#endif // CHROMAHUEDIAGRAM_P_H
