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

#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/perceptualcolorlib_global.h"

#include "PerceptualColor/fullcolordescription.h"
#include "PerceptualColor/simplecolorwheel.h"

namespace PerceptualColor {

/** @brief Complete wheel-based color picker widget
 *
 * This is a composite widget: It inherits from the SimpleColorWheel() widget
 * and adds a ChromaLightnessDiagram() in its center.
 *
 * @todo This class should not derive from @ref SimpleColorWheel because it’s
 * not really an IS-A relationship.
 *
 * @todo Add whatsThis value explaining the accepted keys and mouse
 * movements. */
class WheelColorPicker : public SimpleColorWheel
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * This property is provides as an RGB value.
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged()
     */
     Q_PROPERTY(PerceptualColor::FullColorDescription currentColor
        READ currentColor
        WRITE setCurrentColor
        NOTIFY currentColorChanged
        USER true
    )

public:
    Q_INVOKABLE explicit WheelColorPicker(
        const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
        QWidget *parent = nullptr
    );
    virtual ~WheelColorPicker() noexcept override;
    FullColorDescription currentColor() const;
    void setCurrentColor(const FullColorDescription &newCurrentColorRgb);

Q_SIGNALS:
    /** @brief Signal for currentColor() property */
    void currentColorChanged(
        const PerceptualColor::FullColorDescription &newCurrentColor
    );

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Q_DISABLE_COPY(WheelColorPicker)

    class WheelColorPickerPrivate;
    /** @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class WheelColorPickerPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<WheelColorPickerPrivate> d_pointer;

    /** @brief Only for unit tests. */
    friend class TestWheelColorPicker;

};

}

#endif // WHEELCOLORPICKER_H
