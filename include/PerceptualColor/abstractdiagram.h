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

#ifndef ABSTRACTDIAGRAM_H
#define ABSTRACTDIAGRAM_H

#include "PerceptualColor/perceptualcolorglobal.h"

#include <QWidget>

#include "PerceptualColor/constpropagatinguniquepointer.h"

namespace PerceptualColor
{
/** @brief Base class for LCh diagrams.
 *
 * Provides some elements that are common for all LCh diagrams in this
 * library.
 *
 * @internal
 *
 * @note Qt provides some possibilities to declare that a certain widget
 * has a fixed ration between width and height. You can reimplement
 * <tt>QWidget::hasHeightForWidth()</tt> (indicates that the widget's preferred
 * height depends on its width) and <tt>QWidget::heightForWidth()</tt>
 * (returns the preferred height for this widget, given the width <tt>w</tt>).
 * However, Qt’s layout management makes only very limited use of this
 * information. It is ignored, when the sourrounding window is resized by
 * grapping the window border with the mouse. It is however considered when
 * the sourrounding window is resized by grapping a <tt>QSizeGrip</tt>
 * widget. This behavior is inconsistend and would be suprising for the
 * user. Furthermore, if the widget is yet touching the border of the
 * screen, then the policy cannot be honnored anyway; but it causes
 * flickering. Another possibility is QSizePolicy::setHeightForWidth or
 * QSizePolicy::setWidthForHeight wich seem both to be “only supported for
 * QGraphicsLayout’s subclasses”. Therefore, it’s better not to use at all
 * these features; that’s the only way to provide a consistent and good
 * user experience.
 *
 * @todo Circular diagrams should be right-aligned on RTL layouts.
 *
 * @todo Touchscreen support: Magnify the handle circle, when diagram is
 * used on a touch device?  */
class PERCEPTUALCOLOR_IMPORTEXPORT AbstractDiagram : public QWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE AbstractDiagram(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~AbstractDiagram() noexcept override;

protected:
    QColor focusIndicatorColor() const;
    int gradientMinimumLength() const;
    int gradientThickness() const;
    int maximumPhysicalSquareSize() const;
    qreal maximumWidgetSquareSize() const;
    QSize physicalPixelSize() const;
    QColor handleColorFromBackgroundLightness(qreal lightness) const;
    int handleOutlineThickness() const;
    qreal handleRadius() const;
    int spaceForFocusIndicator() const;
    QImage transparencyBackground() const;

private:
    Q_DISABLE_COPY(AbstractDiagram)

    class AbstractDiagramPrivate;
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class AbstractDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<AbstractDiagramPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestAbstractDiagram;
};

} // namespace PerceptualColor

#endif // ABSTRACTDIAGRAM_H
