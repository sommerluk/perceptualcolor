// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2020 Lukas Sommer sommerluk@gmail.com
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

#ifndef MULTISPINBOXSECTIONCONFIGURATION_P_H
#define MULTISPINBOXSECTIONCONFIGURATION_P_H

#include "PerceptualColor/perceptualcolorglobal.h"
#include "perceptualcolorinternal.h"

// Include the header of the public class of this private implementation.
#include "PerceptualColor/multispinboxsectionconfiguration.h"

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class MultiSpinBoxSectionConfiguration::MultiSpinBoxSectionConfigurationPrivate final
{
public:
    /** @brief Internal storage of the @ref decimals() property */
    int m_decimals = 2;
    /** @brief Internal storage of the @ref m_isWrapping() property */
    bool m_isWrapping = false;
    /** @brief Internal storage of the @ref maximum() property */
    double m_maximum = 99.99;
    /** @brief Internal storage of the @ref minimum() property */
    double m_minimum = 0;
    /** @brief Internal storage of the @ref prefix() property */
    QString m_prefix;
    /** @brief Internal storage of the @ref singleStep() property */
    double m_singleStep = 1;
    /** @brief Internal storage of the @ref suffix() property */
    QString m_suffix;
};

} // namespace PerceptualColor

#endif // MULTISPINBOXSECTIONCONFIGURATION_P_H
