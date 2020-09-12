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

#ifndef ABSTRACTDIAGRAM_H
#define ABSTRACTDIAGRAM_H

#include <QFrame>

namespace PerceptualColor {
    
/** @brief Base class for LCh diagrams.
 * 
 * Provides some elements that are common for all LCh diagrams in this
 * library. */
class AbstractDiagram : public QFrame
{
    Q_OBJECT

public:

    AbstractDiagram(QWidget *parent = nullptr);

protected:

    // constexpr

    /** @brief The radius for circular markers.
     * 
     * Measured in Device Independent Pixels: Pixels used by application (user
     * space), subject to scaling by the operating system or Qt. */
    static constexpr int markerRadius = 4;
    /** @brief The line thickness for markers.
     * 
     * Measured in Device Independent Pixels: Pixels used by application (user
     * space), subject to scaling by the operating system or Qt. */
    static constexpr int markerThickness = 2;
    /** @brief Amount of single step for chroma.
     * 
     * Measured in LCh chroma units.
     * 
     * The smaller of two natural steps that this widget provides and
     * typically corresponds to the user pressing a key or using the mouse
     * wheel: The value will be incremented/decremented by the amount of this
     * value.
     *
     * @sa @ref pageStepChroma */
    static constexpr qreal singleStepChroma = 1;
    /** @brief Amount of single step for hue.
     * 
     * Measured in degree.
     * 
     * The smaller of two natural steps that this widget provides and
     * typically corresponds to the user pressing a key or using the mouse
     * wheel: The value will be incremented/decremented by the amount of this
     * value.
     * 
     * @sa @ref pageStepHue
     * 
     * @todo What would be a good value for this? Its effect depends on
     * chroma: On higher chroma, the same step in hue means a bigger visual
     * color difference. We could even calculate that, but it does not seem to
     * be very intuitive if the reaction on mouse wheel events are different
     * depending on chroma - that would not be easy to understand for the
     * user. And it might be better that the user this way also notices
     * intuitively that hue changes are not linear across chroma. Anyway:
     * What would be a sensible default step? */
    static constexpr qreal singleStepHue = static_cast<qreal>(360) / 100;
    /** @brief Amount of page step for chroma.
     * 
     * Measured in LCh chroma units.
     * 
     * The larger of two natural steps that this widget provides and
     * typically corresponds to the user pressing a key or using the mouse
     * wheel: The value will be incremented/decremented by the amount of this
     * value.
     * 
     * The value is 10 times @ref singleStepChroma. This behavior
     * corresponds to QAbstractSlider, who’s page step is also 10  bigger than
     * its single step. */
    static constexpr qreal pageStepChroma = 10 * singleStepChroma;
    /** @brief Amount of page step for hue.
     * 
     * Measured in degree.
     * 
     * The larger of two natural steps that this widget provides and
     * typically corresponds to the user pressing a key or using the mouse
     * wheel: The value will be incremented/decremented by the amount of this
     * value.
     * 
     * The value is 10 times @ref singleStepHue. This behavior
     * corresponds to QAbstractSlider, who’s page step is also 10  bigger than
     * its single step. */
    static constexpr qreal pageStepHue = 10 * singleStepHue;

    // methods
    QColor focusIndicatorColor() const;
    QSize physicalPixelSize() const;
    
    // static methods
    static QImage transparencyBackground(const qreal devicePixelRatioF);

private:
    Q_DISABLE_COPY(AbstractDiagram)
    friend class TestAbstractDiagram;

};

}

#endif // ABSTRACTDIAGRAM_H
