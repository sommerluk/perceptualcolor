/*
 * Copyright 2020 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PERCEPTUALCOLOR_ALPHASELECTOR_H
#define PERCEPTUALCOLOR_ALPHASELECTOR_H

#include "PerceptualColor/gradientselector.h"
#include "PerceptualColor/rgbcolorspace.h"
#include "PerceptualColor/fullcolordescription.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QWidget>

namespace PerceptualColor {

/** @brief Combined widget with a gradient slider and a spin box; for selecting alpha channel
 *
 * Convenience widget to display and select an alpha channel value. 
 */
class AlphaSelector : public QWidget
{
    Q_OBJECT


    /** @brief Alpha value.
     * 
     * Range is 0..1
     * 
     * @sa setAlpha()
     */
    Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged USER true)

    /** @brief Color.
     * 
     * The alpha channel of this property is ignored. See
     * alpha() for the actually used alpha channed.
     * 
     * @sa setColor()
     */
    Q_PROPERTY(PerceptualColor::FullColorDescription color READ color WRITE setColor)

    /** @brief Representation form in the spin box.
     * 
     * @sa setRepresentation()
     */
    Q_PROPERTY(PerceptualColor::AlphaSelector::NumberFormat representation READ representation WRITE setRepresentation)
    
public:
    /** Constructor */
    AlphaSelector(RgbColorSpace *colorSpace, QWidget *parent = nullptr);
    qreal alpha() const;
    FullColorDescription color() const;

    /** @brief Number format for alpha value */
    enum class NumberFormat {
        percent,                /**< From 0% to 100% */
        one,                    /**< From 0.00 to 1.00 */
        twoHundredAndFiftyFive  /**< From 0 to 255 */
    };
    Q_ENUM(NumberFormat);
    AlphaSelector::NumberFormat representation() const;
    void registerAsBuddy(QLabel *label);

public Q_SLOTS:
    void setColor(const PerceptualColor::FullColorDescription &newColor);
    void setAlpha(const qreal newAlpha);
    void setRepresentation(const AlphaSelector::NumberFormat newRepresentation);

Q_SIGNALS:
    void alphaChanged(qreal alpha);

private:
    Q_DISABLE_COPY(AlphaSelector)
    GradientSelector *m_gradientSelector;
    QDoubleSpinBox *m_doubleSpinBox;
    RgbColorSpace *m_rgbColorSpace;
    FullColorDescription m_color;
    qreal m_alpha;
    NumberFormat m_representation;
private Q_SLOTS:
    void setAlphaFromRepresentationFormat(qreal newAlphaRepresentation);
};

}

#endif // PERCEPTUALCOLOR_ALPHASELECTOR_H
