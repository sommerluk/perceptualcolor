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

#include "perceptualcolorlib_internal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "extendeddoublevalidator.h"
// Second, the private implementation.
#include "extendeddoublevalidator_p.h"

namespace PerceptualColor
{
/** @brief Default constructor */
ExtendedDoubleValidator::ExtendedDoubleValidator(QObject *parent)
    : QDoubleValidator(parent)
    , d_pointer(new ExtendedDoubleValidatorPrivate)
{
}

/** @brief Destructor */
ExtendedDoubleValidator::~ExtendedDoubleValidator() noexcept
{
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::prefix() const
{
    return d_pointer->m_prefix;
}

/** @brief Set the @ref prefix property. */
void ExtendedDoubleValidator::setPrefix(const QString &prefix)
{
    if (prefix != d_pointer->m_prefix) {
        d_pointer->m_prefix = prefix;
        Q_EMIT prefixChanged(prefix);
    }
}

/** @brief Set the @ref suffix property. */
void ExtendedDoubleValidator::setSuffix(const QString &suffix)
{
    if (suffix != d_pointer->m_suffix) {
        d_pointer->m_suffix = suffix;
        Q_EMIT suffixChanged(suffix);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::suffix() const
{
    return d_pointer->m_suffix;
}

QValidator::State ExtendedDoubleValidator::validate(QString &input, int &pos) const
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
    // TODO Bug report, because Qt documentation says at
    // https://doc.qt.io/qt-6/qstring.html#distinction-between-null-and-empty-strings
    // (and at its Qt-5-counterpart):
    //     “All functions except isNull() treat null strings the same
    //      as empty strings.”
    // This is apparently wrong (at least for Qt 5).
    if (!d_pointer->m_prefix.isEmpty()) {
        if (myInput.startsWith(d_pointer->m_prefix)) {
            myInput.remove(0, d_pointer->m_prefix.size());
            myPos -= d_pointer->m_prefix.size();
        } else {
            return QValidator::State::Invalid;
        }
    }
    if (!d_pointer->m_suffix.isEmpty()) {
        if (myInput.endsWith(d_pointer->m_suffix)) {
            myInput.chop(d_pointer->m_suffix.size());
        } else {
            return QValidator::State::Invalid;
        }
    }

    QValidator::State result = QDoubleValidator::validate(myInput, myPos);
    // Following the Qt documentation, QDoubleValidator::validate() is allowed
    // and intended to make changes the arguments passed by reference. We
    // have to write back these changes also in this reimplemented function.
    input = d_pointer->m_prefix + myInput + d_pointer->m_suffix;
    pos = myPos + d_pointer->m_prefix.size();

    return result;
}

} // namespace PerceptualColor
