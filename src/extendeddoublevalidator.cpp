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

// Own header
#include "PerceptualColor/extendeddoublevalidator.h"

namespace PerceptualColor {

/** @brief Default constructor */
ExtendedDoubleValidator::ExtendedDoubleValidator(
    QObject *parent
) : QDoubleValidator(parent)
{
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::prefix() const
{
    return m_prefix;
}

/** @brief Set the @ref prefix property. */
void ExtendedDoubleValidator::setPrefix(const QString &prefix)
{
    if (prefix != m_prefix) {
        m_prefix = prefix;
        Q_EMIT prefixChanged(prefix);
    }
}

/** @brief Set the @ref suffix property. */
void ExtendedDoubleValidator::setSuffix(const QString &suffix)
{
    if (suffix != m_suffix) {
        m_suffix = suffix;
        Q_EMIT suffixChanged(suffix);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::suffix() const
{
    return m_suffix;
}

QValidator::State ExtendedDoubleValidator::validate(
    QString &input,
    int &pos
) const
{
    QString myInput = input;
    int myPos = pos;
    
    // IF (m_prefix.isEmpty && !m_prefix.isNull)
    // THEN input.startsWith(m_prefix)
    // →  will be true IF !input.isEmpty
    // →  will be false IF input.isEmpty
    // This is inconsistent. Therefore, we test is m_prefix is empty.
    // If not, we do nothing.
    // The same also applies to suffix.
    if (!m_prefix.isEmpty()) {
        if (myInput.startsWith(m_prefix)) {
            myInput.remove(0, m_prefix.size());
            myPos -= m_prefix.size();
        } else {
            return QValidator::State::Invalid;
        }
    }
    if (!m_suffix.isEmpty()) {
        if (myInput.endsWith(m_suffix)) {
            myInput.chop(m_suffix.size());
        } else {
            return QValidator::State::Invalid;
        }
    }
    
    QValidator::State result = QDoubleValidator::validate(myInput, myPos);
    // Following the Qt documentation, QDoubleValidator::validate() is allowed
    // and intended to make changes the arguments passed by reference. We
    // have to write back these changes also in this reimplemented function.
    input = m_prefix + myInput + m_suffix;
    pos = myPos + m_prefix.size();
    
    return result;
}

}
