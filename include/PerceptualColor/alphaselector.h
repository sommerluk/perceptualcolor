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
// TODO The API should be similar to QSlider or at lest QAbstractSlider
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
    Q_ENUM(NumberFormat)
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
