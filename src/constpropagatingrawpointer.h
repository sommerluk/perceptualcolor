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

#ifndef CONSTPROPAGATINGRAWPOINTER_H
#define CONSTPROPAGATINGRAWPOINTER_H

namespace PerceptualColor {

/** @brief A <tt>const</tt> propagating raw pointer
 *
 * With normal raw C++ pointers (and also with <tt>std::unique_ptr</tt>
 * pointers), within <tt>const</tt> functions you can do non-const
 * operations <em>on objects that a pointer points to</em>.
 *
 * <em>This</em> pointer type is different: It propagates the const-ness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-cost access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves similar to raw pointers. For compatibility with raw pointers,
 * it also casts implicitly to the corresponding raw pointer (but only
 * within non-<tt>const</tt> contexts).
 *
 * Think of this template as a simple alternative to
 * <tt>std::experimental::propagate_const&lt; T* &gt;</tt>
 *
 * Example code:
 * @snippet test/testconstpropagatingrawpointer.cpp ConstPropagatingRawPointer Example
 *
 * @sa @ref ConstPropagatingUniquePointer
 *
 * @note There exist very sophisticated implementations like
 * https://github.com/jbcoe/propagate_const but we use nevertheless
 * our own lightwise implementation because this seems to be enough
 * for our limited use case. */
template <typename T>
class ConstPropagatingRawPointer final
{
public:
    /** @brief Default constructor
     *
     * Creates a pointer that points to <tt>nullptr</tt>. */
    explicit ConstPropagatingRawPointer() : m_pointer(nullptr)
    {
    }

    /** @brief Constructor
     *
     * @param pointer Object to which to point */
    explicit ConstPropagatingRawPointer(T *pointer) : m_pointer(pointer)
    {
    }

    /** @brief Default destructor */
    ~ConstPropagatingRawPointer() noexcept = default;

    /** @brief Non-const pointer operator
     *
     * @returns Non-const pointer operator */
    T * operator->()
    {
        return m_pointer;
    }

    /** @brief Const pointer operator
     *
     * @returns Const pointer */
    const T * operator->() const {
        return m_pointer;
    }

    /** @brief Non-const dereference operator
     *
     * @returns Non-const dereference operator */
    T & operator*()
    {
        return *m_pointer;
    }

    /** @brief Const dereference operator
     *
     * @returns Const dereference operator */
    const T & operator*() const
    {
        return *m_pointer;
    }

    /** @brief Cast to a normal raw pointer.
     *
     * This cast is only available within non-<tt>const</tt> contexts. */
    operator T*()
    {
        return m_pointer;
    }

private:
    /** @brief Internal storage for the pointer */
    T * m_pointer;

};

}

#endif // CONSTPROPAGATINGUNIQUEPOINTER_H
