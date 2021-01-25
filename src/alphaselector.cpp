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

#include "perceptualcolorlib_qtconfiguration.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/alphaselector.h"
// Second, the private implementation.
#include "alphaselector_p.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QSignalBlocker>

#include "lchvalues.h"

namespace PerceptualColor {

AlphaSelector::AlphaSelector(
    const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
    QWidget *parent
) :
    QWidget(parent),
    d_pointer(new AlphaSelectorPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    d_pointer->m_gradientSelector = new GradientSelector(
        d_pointer->m_rgbColorSpace
    );
    d_pointer->m_gradientSelector->setOrientation(Qt::Orientation::Horizontal);
    d_pointer->m_doubleSpinBox = new QDoubleSpinBox();
    d_pointer->m_doubleSpinBox->setAlignment(Qt::AlignmentFlag::AlignRight);
    layout->addWidget(d_pointer->m_gradientSelector);
    layout->addWidget(d_pointer->m_doubleSpinBox);
    setLayout(layout);
    connect(
        d_pointer->m_gradientSelector,
        &GradientSelector::fractionChanged,
        this,
        &AlphaSelector::setAlpha
    );
    connect(
        d_pointer->m_doubleSpinBox,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        [this](qreal newAlphaRepresentation) {
            d_pointer->setAlphaFromRepresentationFormat(
                newAlphaRepresentation
            );
        }
    );

    cmsCIELCh lch;
    lch.L = LchValues::defaultLightness;
    lch.C = LchValues::defaultChroma;
    lch.h = LchValues::defaultHue;
    FullColorDescription temp = FullColorDescription(
        d_pointer->m_rgbColorSpace,
        lch,
        FullColorDescription::outOfGamutBehaviour::preserve
    );
    /* setRepresentation does only do something if the new value is
     * different from the old one. As we rely on it for initialization,
     * we set it first to some other value, and second to the really
     * desired default value. */
    // First, set to an unwanted value:
    setRepresentation(NumberFormat::one);
    // Second, set to the really wanted value:
    setRepresentation(NumberFormat::percent);
    /** Same procedure for alpha() property */
    setAlpha(0); // first set to an unwanted value
    setAlpha(1); // second set to the really wanted value
    /* Not necessary to do the same for color() because the default
     * constructor of this object constructs an invalid color anyway,
     * and because our new value is valid, it will be set correctly. */
    setColor(temp);
}

AlphaSelector::~AlphaSelector() noexcept
{
}

/** @brief Constructor
 * 
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
AlphaSelector::AlphaSelectorPrivate::AlphaSelectorPrivate(
    AlphaSelector *backLink
) : q_pointer(backLink)
{
}

qreal AlphaSelector::alpha() const
{
    return d_pointer->m_alpha;
}

FullColorDescription AlphaSelector::color() const
{
    return d_pointer->m_color;
}

AlphaSelector::NumberFormat AlphaSelector::representation() const
{
    return d_pointer->m_representation;
}


void AlphaSelector::setRepresentation(
    AlphaSelector::NumberFormat newRepresentation
)
{
    if (d_pointer->m_representation == newRepresentation) {
        return;
    }
    d_pointer->m_representation = newRepresentation;
    switch (d_pointer->m_representation) {
        case AlphaSelector::NumberFormat::one:
            d_pointer->m_doubleSpinBox->setMinimum(0);
            d_pointer->m_doubleSpinBox->setMaximum(1);
            d_pointer->m_doubleSpinBox->setSuffix(QLatin1String());
            d_pointer->m_doubleSpinBox->setDecimals(2);
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha);
            break;
        case AlphaSelector::NumberFormat::percent:
            d_pointer->m_doubleSpinBox->setMinimum(0);
            d_pointer->m_doubleSpinBox->setMaximum(100);
            d_pointer->m_doubleSpinBox->setSuffix(QStringLiteral(u"%"));
            d_pointer->m_doubleSpinBox->setDecimals(0);
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha * 100);
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            d_pointer->m_doubleSpinBox->setMinimum(0);
            d_pointer->m_doubleSpinBox->setMaximum(255);
            d_pointer->m_doubleSpinBox->setSuffix(QLatin1String());
            d_pointer->m_doubleSpinBox->setDecimals(0);
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha * 255);
            break;
        default:
            throw;
    }
    Q_EMIT representationChanged(d_pointer->m_representation);
}

void AlphaSelector::AlphaSelectorPrivate::setAlphaFromRepresentationFormat(
    qreal newAlphaRepresentation
)
{
    switch (m_representation) {
        case AlphaSelector::NumberFormat::one:
            q_pointer->setAlpha(
                newAlphaRepresentation
            );
            break;
        case AlphaSelector::NumberFormat::percent:
            q_pointer->setAlpha(
                newAlphaRepresentation / static_cast<qreal>(100)
            );
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            q_pointer->setAlpha(
                newAlphaRepresentation / static_cast<qreal>(255)
            );
            break;
        default:
            throw;
    }
}

void AlphaSelector::setColor(const FullColorDescription &newColor)
{
    if (d_pointer->m_color == newColor) {
        return;
    }
    d_pointer->m_color = newColor;
    FullColorDescription first = FullColorDescription(
        d_pointer->m_rgbColorSpace,
        newColor.toLch(),
        FullColorDescription::outOfGamutBehaviour::preserve,
        0
    );
    FullColorDescription second = FullColorDescription(
        d_pointer->m_rgbColorSpace,
        newColor.toLch(),
        FullColorDescription::outOfGamutBehaviour::preserve,
        1
    );
    d_pointer->m_gradientSelector->setColors(first, second);
    Q_EMIT colorChanged(d_pointer->m_color);
}

/** @brief Register this widget as buddy.
 * 
 * This function registers this widget as the focus buddy of a QLabel. It's
 * better using this function than calling directly QLabel::setBuddy(). This
 * function does not set this entire widget as focus buddy, but only the spin
 * box part, which seems reasonable as focus buddies react on keyboard events
 * and the spin box is better adapted to keyboard input than the slider.
 * 
 * @todo Is there a more elegant solution for this problem? Without this
 * function? */
void AlphaSelector::registerAsBuddy(QLabel *label)
{
    label->setBuddy(d_pointer->m_doubleSpinBox);
}

void AlphaSelector::setAlpha(qreal newAlpha)
{
    if (d_pointer->m_alpha == newAlpha) {
        return;
    }
    d_pointer->m_alpha = newAlpha;
    Q_EMIT alphaChanged(d_pointer->m_alpha);
    const QSignalBlocker blockerSpinBox(d_pointer->m_doubleSpinBox);
    switch (d_pointer->m_representation) {
        case AlphaSelector::NumberFormat::one:
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha);
            break;
        case AlphaSelector::NumberFormat::percent:
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha * 100);
            break;
        case AlphaSelector::NumberFormat::twoHundredAndFiftyFive:
            d_pointer->m_doubleSpinBox->setValue(d_pointer->m_alpha * 255);
            break;
        default:
            throw;
    }
    const QSignalBlocker blockerGradient(d_pointer->m_gradientSelector);
    d_pointer->m_gradientSelector->setFraction(d_pointer->m_alpha);
}

}
