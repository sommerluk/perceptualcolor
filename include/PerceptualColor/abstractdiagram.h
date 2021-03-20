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

#include <QWidget>

#include "PerceptualColor/constpropagatinguniquepointer.h"
#include "PerceptualColor/perceptualcolorlib_global.h"

namespace PerceptualColor {

/** @brief Base class for LCh diagrams.
 *
 * Provides some elements that are common for all LCh diagrams in this
 * library.
 *
 * @anchor MeasurementDetails <b>Measurement details</b>
 *
 * This class and its child classes deal simultaniously with various
 * coordinate systems and unit of measurement.
 *
 * <b>Units of measurement</b>
 *
 * - <em>Device-indepentend pixel/coordinates</em> are the  unit of
 *   measurement for widgets, windows, screens, mouse events and so on in Qt.
 * - <em>Physical pixel/coordinates</em> are the unit that measures actual
 *   physical screen pixel.
 *
 * The conversion factor between these two units of measurement is
 * <tt>QPaintDevice::devicePixelRatioF()</tt>. It is usually <tt>1</tt> on
 * classic low resolution screens, and bigger than <tt>1</tt> on high
 * resolution screens. See https://doc.qt.io/qt-6/highdpi.html for more
 * details on Qt’s High DPI support.
 *
 * <b>Coordinate points versus pixel positions</b>
 *
 * - <em>Coordinate points</em> are points in the mathematical sense, that
 *   means they have zero surface. They should always be represented by
 *   floating point data types; this is necessary to allow conversions
 *   without having too much rounding errors.
 * - <em>Pixel positions</em> describe the position of a particular pixel.
 *   Pixel are surfaces, not points. A pixel is a square of the width and
 *   length <tt>1</tt>. The pixel at position <tt>QPoint(x, y)</tt> is the
 *   square with the top-left edge at coordinate point <tt>QPoint(x, y)</tt>
 *   and the botton-right edge at coordinate point <tt>QPoint(x+1, y+1)</tt>.
 *
 * Some functions (like mouse events work with pixel positions), other
 * functions (like antialiased floatting-point drawing operations) work
 * with coordinate points. It’s important to always distinguish correctly
 * these two different concepts. See https://doc.qt.io/qt-6/coordsys.html
 * for more details on Qt’s coordinate systems.
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
 * @todo Non-private members (like @ref handleRadius) should not be
 * <tt>constexpr</tt> to make sure that changes of these values does
 * not require recomplining the application to take effect!?
 *
 * @todo Touchscreen support: Magnify the handle circle, when diagram is
 * used on a touch device?  */
class AbstractDiagram : public QWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE AbstractDiagram(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~AbstractDiagram() noexcept override;

protected:
    QColor focusIndicatorColor() const;
    int maximumPhysicalSquareSize() const;
    qreal maximumWidgetSquareSize() const;
    QSize physicalPixelSize() const;
    int handleOutlineThickness() const;
    qreal handleRadius() const;
    int spaceForFocusIndicator() const;
    QImage transparencyBackground() const;
    int wheelThickness() const;

private:
    Q_DISABLE_COPY(AbstractDiagram)

    class AbstractDiagramPrivate;
    /** @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class AbstractDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<AbstractDiagramPrivate> d_pointer;

    /** @brief Only for unit tests. */
    friend class TestAbstractDiagram;

};

}

#endif // ABSTRACTDIAGRAM_H
