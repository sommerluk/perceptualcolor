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

#include "PerceptualColor/alphaselector.h"

#include <QDebug>
#include <QSignalBlocker>
#include <QHBoxLayout>

namespace PerceptualColor {

AlphaSelector::AlphaSelector(RgbColorSpace *colorSpace, QWidget *parent) : QWidget(parent)
{
    m_rgbColorSpace = colorSpace;
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    m_gradientSelector = new GradientSelector(m_rgbColorSpace);
    m_gradientSelector->setOrientation(Qt::Orientation::Horizontal);
    m_doubleSpinBox = new QDoubleSpinBox();
    m_doubleSpinBox->setAlignment(Qt::AlignmentFlag::AlignRight);
    layout->addWidget(m_gradientSelector);
    layout->addWidget(m_doubleSpinBox);
    setLayout(layout);
    connect(
        m_gradientSelector,
        &GradientSelector::fractionChanged,
        this,
        &AlphaSelector::setAlpha
    );
    connect(
        m_doubleSpinBox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        &AlphaSelector::setAlphaFromRepresentationFormat
    );
    cmsCIELCh lch;
    lch.L = Helper::LchDefaults::defaultLightness;
    lch.C = Helper::LchDefaults::defaultChroma;
    lch.h = Helper::LchDefaults::defaultHue;
    FullColorDescription temp = FullColorDescription(
        m_rgbColorSpace,
        lch,
        FullColorDescription::outOfGamutBehaviour::preserve
    );
    /* setRepresentation does only do something if the new value is different from the old one.
     * As we rely on it for initiailzation, we set it first to some other value, and second
     * to the really desired default value. */
    setRepresentation(NumberFormat::one); // first set to an unwanted value
    setRepresentation(NumberFormat::percent); // second set to the really wanted value
    /** Same procedure for alpha() property */
    setAlpha(0); // first set to an unwanted value
    setAlpha(1); // second set to the really wanted value
    /* Not necessary to do the same for color() because the default constructor of this object
     * constructs an invalid color anyway, and because our new value is valid, it will be set
     * correctly. */
    setColor(temp);
}

qreal AlphaSelector::alpha() const
{
    return m_alpha;
}

FullColorDescription AlphaSelector::color() const
{
    return m_color;
}

AlphaSelector::NumberFormat AlphaSelector::representation() const
{
    return m_representation;
}


void AlphaSelector::setRepresentation(AlphaSelector::NumberFormat newRepresentation)
{
    if (m_representation == newRepresentation) {
        return;
    }
    m_representation = newRepresentation;
    switch (m_representation) {
        case AlphaSelector::NumberFormat::one:
            m_doubleSpinBox->setMinimum(0);
            m_doubleSpinBox->setMaximum(1);
            m_doubleSpinBox->setSuffix(QLatin1String());
            m_doubleSpinBox->setDecimals(2);
            m_doubleSpinBox->setValue(m_alpha);
            break;
        case AlphaSelector::NumberFormat::percent:
            m_doubleSpinBox->setMinimum(0);
            m_doubleSpinBox->setMaximum(100);
            m_doubleSpinBox->setSuffix(QStringLiteral(u"%"));
            m_doubleSpinBox->setDecimals(0);
            m_doubleSpinBox->setValue(m_alpha * 100);
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            m_doubleSpinBox->setMinimum(0);
            m_doubleSpinBox->setMaximum(255);
            m_doubleSpinBox->setSuffix(QLatin1String());
            m_doubleSpinBox->setDecimals(0);
            m_doubleSpinBox->setValue(m_alpha * 255);
            break;
        default:
            throw;
    }
}

void AlphaSelector::setAlphaFromRepresentationFormat(qreal newAlphaRepresentation)
{
    switch (m_representation) {
        case AlphaSelector::NumberFormat::one:
            setAlpha(newAlphaRepresentation);
            break;
        case AlphaSelector::NumberFormat::percent:
            setAlpha(newAlphaRepresentation / static_cast<qreal>(100));
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            setAlpha(newAlphaRepresentation / static_cast<qreal>(255));
            break;
        default:
            throw;
    }
}

void AlphaSelector::setColor(const FullColorDescription &newColor)
{
    if (m_color == newColor) {
        return;
    }
    m_color = newColor;
    FullColorDescription first = FullColorDescription(
        m_rgbColorSpace,
        newColor.toLch(),
        FullColorDescription::outOfGamutBehaviour::preserve,
        0
    );
    FullColorDescription second = FullColorDescription(
        m_rgbColorSpace,
        newColor.toLch(),
        FullColorDescription::outOfGamutBehaviour::preserve,
        1
    );
    m_gradientSelector->setColors(first, second);
}

/** @brief Register this widget as buddy.
 * 
 * This function registers this widget as the focus buddy of a QLabel. It's
 * better using this function than calling directly QLabel::setBuddy(). This
 * function does not set this entire widget as focus buddy, but only the spinbox
 * part, which seems reasonable as focus buddies react on keyboard events and
 * the spinbox is better adapted to keyboard input than the slider.
 */
void AlphaSelector::registerAsBuddy(QLabel *label)
{
    label->setBuddy(m_doubleSpinBox);
}

void AlphaSelector::setAlpha(qreal newAlpha)
{
    if (m_alpha == newAlpha) {
        return;
    }
    m_alpha = newAlpha;
    Q_EMIT alphaChanged(m_alpha);
    const QSignalBlocker blockerSpinBox(m_doubleSpinBox);
    switch (m_representation) {
        case AlphaSelector::NumberFormat::one:
            m_doubleSpinBox->setValue(m_alpha);
            break;
        case AlphaSelector::NumberFormat::percent:
            m_doubleSpinBox->setValue(m_alpha * 100);
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            m_doubleSpinBox->setValue(m_alpha * 255);
            break;
        default:
            throw;
    }
    const QSignalBlocker blockerGradient(m_gradientSelector);
    m_gradientSelector->setFraction(m_alpha);
}

}
