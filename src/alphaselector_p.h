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

#ifndef ALPHASELECTOR_P_H
#define ALPHASELECTOR_P_H

#include "PerceptualColor/gradientselector.h"

#include <QDoubleSpinBox>

namespace PerceptualColor {

/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class AlphaSelector::AlphaSelectorPrivate final
{
public:
    /** @brief Constructor */
    AlphaSelectorPrivate() = default;
    /** @brief Default destructor
     * 
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~AlphaSelectorPrivate() noexcept = default;

    qreal m_alpha;
    FullColorDescription m_color;
    QPointer<QDoubleSpinBox> m_doubleSpinBox;
    QPointer<GradientSelector> m_gradientSelector;
    NumberFormat m_representation;
    QPointer<RgbColorSpace> m_rgbColorSpace;
private:
    Q_DISABLE_COPY(AlphaSelectorPrivate)
};

}

#endif // ALPHASELECTOR_P_H
