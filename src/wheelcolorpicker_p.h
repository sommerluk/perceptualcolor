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

#ifndef WHEELCOLORPICKER_P_H
#define WHEELCOLORPICKER_P_H

// Include the header of the public class of this private implementation.
#include "PerceptualColor/wheelcolorpicker.h"
#include "constpropagatingrawpointer.h"

#include <QPointer>

#include "PerceptualColor/chromalightnessdiagram.h"
#include "PerceptualColor/colorwheel.h"
#include "PerceptualColor/rgbcolorspace.h"

namespace PerceptualColor
{
/** @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class WheelColorPicker::WheelColorPickerPrivate final : public QObject
{
    Q_OBJECT
public:
    WheelColorPickerPrivate(WheelColorPicker *backLink);
    /** @brief Default destructor */
    virtual ~WheelColorPickerPrivate() noexcept = default;

    /** @brief A pointer to the @ref ChromaLightnessDiagram widget. */
    QPointer<ChromaLightnessDiagram> m_chromaLightnessDiagram;
    /** @brief A pointer to the color space. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief A pointer to the @ref ColorWheel widget. */
    QPointer<ColorWheel> m_ColorWheel;

    void resizeChildWidgets();
    static QSize scaleRectangleToDiagonal(const QSize oldRectangle,
                                          const qreal newDiagonal);

public Q_SLOTS:
    void handleFocusChanged(QWidget *old, QWidget *now);

private:
    Q_DISABLE_COPY(WheelColorPickerPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<WheelColorPicker> q_pointer;
};

}

#endif // WHEELCOLORPICKER_P_H
