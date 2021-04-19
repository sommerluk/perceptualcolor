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

#ifndef COLORPATCH_H
#define COLORPATCH_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QFrame>

#include "PerceptualColor/constpropagatinguniquepointer.h"

namespace PerceptualColor
{
/** @brief A color display widget.
 *
 * This widget simply displays a color. Useful for showing a given
 * color. The instantiation and usage is simple. Example:
 * @snippet test/testcolorpatch.cpp ColorPatch Create widget
 *
 * The default minimum size of this widget is similar to a
 * <tt>QToolButton</tt>. Depending on your use case, you might
 * set a bigger minimum size:
 * @snippet test/testcolorpatch.cpp ColorPatch Bigger minimum size
 *
 * This class is API-compatible with KColorPatch.
 *
 * <!-- NOTE A similar functionality is available as KColorPatch, but this is
 * part of KDELibs4Support which is available in KF5 but should not be used
 * for new code, but only for legacy code. Also, depending on KDELibs4Support
 * would pull-in another dependency. Furthermore, KColorPatch has support for
 * drag-and-drop, which is not desirable for our use case. Therefore, with
 * @ref ColorPatch there is a lightweight alternative (that, by the way,
 * implements the full  API of KColorPatch). -->
 *
 * @todo Base this class on @ref AbstractDiagram to be future-proof? We
 * could draw the frame with QStyle…
 *
 * @todo The design does not work well on some styles (wrong metrics!) */
class ColorPatch : public QFrame
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
     *   will be a special background pattern (and <em>not</em> the default
     *   widget background).
     *
     * @note No color management is applied. The color is used as-is to
     * paint on the canvas provided by the operation system.
     *
     * @sa @ref color() const
     * @sa @ref setColor()
     * @sa @ref colorChanged() */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    Q_INVOKABLE explicit ColorPatch(QWidget *parent = nullptr);
    virtual ~ColorPatch() noexcept override;
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
    void colorChanged(const QColor &color);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Q_DISABLE_COPY(ColorPatch)

    class ColorPatchPrivate;
    /** @internal
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ColorPatchPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorPatchPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestColorPatch;
};

} // namespace PerceptualColor

#endif // COLORPATCH_H
