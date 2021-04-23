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

#ifndef WHEELCOLORPICKER_H
#define WHEELCOLORPICKER_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include "PerceptualColor/constpropagatinguniquepointer.h"

#include "PerceptualColor/abstractdiagram.h"
#include "PerceptualColor/lchdouble.h"

namespace PerceptualColor
{
class RgbColorSpace;

/** @brief Complete wheel-based color picker widget
 *
 * It is composed of a @ref ColorWheel and, in the middle,
 * a @ref ChromaLightnessDiagram.
 *
 * @internal
 *
 * @todo BUG: Choose HLC x 50 1. Then push the Page-up button. Current
 * behaviour: The chroma value change from 1 to 0. Why?
 *
 * @todo BUG: Current behaviour: This class accepts focus by Tab, but it
 * does not accept focus by Shift-Tab. Expected behaviour: This class
 * accepts focus by both, Tab and Shift-Tab.
 *
 * @todo BUG: When the hue changes and the gamut gets smaller than the
 * current chroma-lightness value, then the marker is not moved into the
 * new gamut. But it should!
 *
 * @todo Is the (double) focus indicator actually good design? Are there
 * better solutions?
 *
 * @todo A click in the inner of the color wheel (also at emtpy places)
 * should trigger to get a focus!
 *
 * @todo Add <tt>whatsThis</tt> value explaining the accepted keys and mouse
 * movements (and also to other widgets). */
class PERCEPTUALCOLOR_IMPORTEXPORT WheelColorPicker : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged() */
    Q_PROPERTY(PerceptualColor::LchDouble currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged USER true)

public:
    Q_INVOKABLE explicit WheelColorPicker(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~WheelColorPicker() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    PerceptualColor::LchDouble currentColor() const;
    virtual QSize minimumSizeHint() const override;
    void setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor);
    virtual QSize sizeHint() const override;

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *  @param newCurrentColor the new current color */
    void currentColorChanged(const PerceptualColor::LchDouble &newCurrentColor);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(WheelColorPicker)

    class WheelColorPickerPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class WheelColorPickerPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<WheelColorPickerPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestWheelColorPicker;
};

} // namespace PerceptualColor

#endif // WHEELCOLORPICKER_H
