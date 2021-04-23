// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2013 Torbjörn Klatt <opensource@torbjoern-klatt.de>
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

#ifndef CONSTPROPAGATINGUNIQUEPOINTER_H
#define CONSTPROPAGATINGUNIQUEPOINTER_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <memory>

namespace PerceptualColor
{
/** @brief A <tt>const</tt> propagating <tt>std::unique_ptr</tt>
 *
 * With normal <tt>std::unique_ptr</tt> pointers (and also with raw
 * C++ pointers), within <tt>const</tt> functions you can do non-const
 * operations <em>on objects that a pointer points to</em>.
 *
 * <em>This</em> pointer type is different: It propagates the const-ness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-cost access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves like <tt>std::unique_ptr</tt> (from which it inherits).
 *
 * Think of this template as a simple alternative to
 * <tt>std::experimental::propagate_const&lt; std::unique_ptr&lt;T&gt; &gt;</tt>
 *
 * Example code:
 * @snippet test/testconstpropagatinguniquepointer.cpp example
 *
 * @note A @ref ConstPropagatingUniquePointer pointer variable itself
 * may not be const! @internal (Otherwise, this would make <tt>const</tt>
 * <em>all</em> access even to non-const functions of the pointed
 * object.) @endinternal
 *
 * @internal
 *
 * @sa @ref ConstPropagatingRawPointer
 * @sa This code is based on the idea in
 * <a href="http://torbjoernk.github.io/deep_const_ptr/">deep_const_ptr</a>
 * which is a more general implementation of this concept, that does not
 * provide a <tt>std::unique_ptr</tt> but acts as a wrapper template around
 * smart pointers in general. It lacks however the dereference operator.
 *
 * @todo Would it be better to include (or link to)
 * https://github.com/jbcoe/propagate_const instead of having our own
 * implementation? Or remove propagate_const header from this library? */
template<typename T> class PERCEPTUALCOLOR_IMPORTEXPORT ConstPropagatingUniquePointer final : public std::unique_ptr<T>
{
public:
    /** @brief Default constructor
     *
     * Creates a pointer that points to <tt>nullptr</tt>. */
    explicit ConstPropagatingUniquePointer()
        : std::unique_ptr<T>(nullptr)
    {
    }

    /** @brief Constructor
     *
     * @param pointer Object to which to point */
    explicit ConstPropagatingUniquePointer(typename std::unique_ptr<T>::element_type *pointer)
        : std::unique_ptr<T>(pointer)
    {
    }

    /** @brief Default destructor
     *
     * This destructor is not marked as <tt>override</tt> because the
     * base class’s destructor is not virtual.*/
    ~ConstPropagatingUniquePointer() noexcept = default;

    /** @brief Non-const pointer operator
     *
     * @returns Non-const pointer operator */
    typename std::unique_ptr<T>::element_type *operator->()
    {
        return std::unique_ptr<T>::operator->();
    }

    /** @brief Const pointer operator
     *
     * @returns Const pointer */
    const typename std::unique_ptr<T>::element_type *operator->() const
    {
        return std::unique_ptr<T>::operator->();
    }

    /** @brief Non-const dereference operator
     *
     * @returns Non-const dereference operator */
    typename std::unique_ptr<T>::element_type &operator*()
    {
        return std::unique_ptr<T>::operator*();
    }

    /** @brief Const dereference operator
     *
     * @returns Const dereference operator */
    const typename std::unique_ptr<T>::element_type &operator*() const
    {
        return std::unique_ptr<T>::operator*();
    }
};

} // namespace PerceptualColor

#endif // CONSTPROPAGATINGUNIQUEPOINTER_H
