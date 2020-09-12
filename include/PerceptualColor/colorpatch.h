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

#ifndef COLORPATCH_H
#define COLORPATCH_H

/** @file
 * 
 * Declaration of the @ref PerceptualColor::ColorPatch class and its
 * members. */

#include "PerceptualColor/abstractdiagram.h"

namespace PerceptualColor {

/** @brief A color display widget.
 * 
 * This widget simply displays a color. Useful for showing a selected
 * color. The instantiation and usage is simple. Example:
 * @snippet test/testcolorpatch.cpp ColorPatch Create widget
 * 
 * This class is API-compatible with KColorPatch.
 * 
 * @note A similar functionality is available as KColorPatch, but this is part
 * of KDELibs4Support which is available in KF5 but should not be used for new
 * code, but only for legacy code. Also, depending on KDELibs4Support would
 * pull-in another dependency. Furthermore, KColorPatch has support for
 * drag-and-drop, which is not desirable for our use case. Therefore, with
 * @ref ColorPatch there is a lightweight alternative (that, by the way,
 * implements the full  API of KColorPatch). */
class ColorPatch : public AbstractDiagram
{
    Q_OBJECT

    /** @brief The color that is displayed
     * 
     * Default value is an invalid color.
     * 
     * - If the color is invalid, nothing is displayed. The frame is empty:
     *   Only the default widget background is visible within the frame.
     * - If the color is valid, the widget frame is filled with this color.
     *   If this color is not fully opaque, the background behind the color
     *   will be a special background pattern (and @em not the default widget
     *   background).
     * 
     * @note No color management is applied. The color is used as-is to
     * paint on the canvas provided by the operation system.
     * 
     * @sa @ref color()
     * @sa @ref setColor()
     * @sa @ref colorChanged()
     * @sa @ref m_color
     */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit ColorPatch(QWidget *parent = nullptr);
    /** @brief Getter for property @ref color
     *  @returns the property @ref color */
    QColor color() const;
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
    
public Q_SLOTS:
    void setColor(const QColor &newColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref color.
     * 
     * @param color the new color */
    void colorChanged (const QColor &color);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    Q_DISABLE_COPY(ColorPatch)

    /** @brief Internal storage for property @ref color
     * 
     * QColor automatically initializes with an invalid color, just like it
     * should be for the property @ref color, so no need to initialize here
     * manually. */
    QColor m_color;
};

}

#endif // COLORPATCH_H
