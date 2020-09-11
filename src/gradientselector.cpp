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

// own header
#include "PerceptualColor/gradientselector.h"

#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QMouseEvent>

namespace PerceptualColor {

GradientSelector::GradientSelector(RgbColorSpace *colorSpace, QWidget *parent)
: AbstractDiagram(parent)
{
    initialize(colorSpace, Qt::Orientation::Vertical);
}

GradientSelector::GradientSelector(
    RgbColorSpace* colorSpace,
    Qt::Orientation orientation,
    QWidget* parent
) : AbstractDiagram(parent)
{
    initialize(colorSpace, orientation);
}

void GradientSelector::initialize(
    RgbColorSpace* colorSpace,
    Qt::Orientation orientation
)
{
    setFocusPolicy(Qt::StrongFocus);
    m_rgbColorSpace = colorSpace;
    setOrientation(orientation); // also updates the size policy
    cmsCIELCh one;
    one.L = 50;
    one.C = 65;
    one.h = 100;
    cmsCIELCh two;
    two.L = 60;
    two.C = 85;
    two.h = 300;
    setColors(
        FullColorDescription(
            m_rgbColorSpace,
            one,
            FullColorDescription::outOfGamutBehaviour::preserve,
            0
        ),
        FullColorDescription(
            m_rgbColorSpace,
            two,
            FullColorDescription::outOfGamutBehaviour::preserve,
            1
        )
    );
}

QSize GradientSelector::sizeHint() const
{
    return minimumSizeHint();
}

QSize GradientSelector::minimumSizeHint() const
{
    QSize temp;
    if (m_orientation == Qt::Orientation::Vertical) {
        temp = QSize(m_gradientThickness, m_gradientMinimumLength);
    } else {
        temp = QSize(m_gradientMinimumLength, m_gradientThickness);
    }
    return temp;
}

qreal GradientSelector::fromWindowCoordinatesToFraction(
    QPoint windowCoordinates
)
{
    qreal temp;
    if (m_orientation == Qt::Orientation::Vertical) {
        temp = (size().height() - windowCoordinates.y()) / static_cast<qreal>(size().height());
    } else {
        if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
            temp = windowCoordinates.x() / static_cast<qreal>(size().width());
        } else {
            temp = (size().width() - windowCoordinates.x()) / static_cast<qreal>(size().width());
        }
    }
    return qBound<qreal>(0, temp, 1);
}

void GradientSelector::mousePressEvent(QMouseEvent* event)
{
    setFraction(
        fromWindowCoordinatesToFraction(
            event->pos()
        )
    );
}

void GradientSelector::mouseReleaseEvent(QMouseEvent* event)
{
    setFraction(
        fromWindowCoordinatesToFraction(
            event->pos()
        )
    );
}

void GradientSelector::mouseMoveEvent(QMouseEvent* event)
{
    setFraction(
        fromWindowCoordinatesToFraction(
            event->pos()
        )
    );
}

qreal GradientSelector::fraction()
{
    return m_fraction;
}

void GradientSelector::setFraction(qreal newFraction)
{
    qreal temp = qBound<qreal>(0, newFraction, 1);
    if (m_fraction != temp) {
        m_fraction = temp;
        update();
        Q_EMIT fractionChanged(temp);
    }
}

void GradientSelector::wheelEvent(QWheelEvent* event)
{
    qreal steps = Helper::standardWheelSteps(event);
    //  Only react on good old vertical wheels, and not on horizontal wheels
    if (steps != 0) {
        setFraction(m_fraction + steps * m_singleStep);
    } else {
        event->ignore(); // don't accept the event and let it up to the default treatment
    }
}

void GradientSelector::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Plus: 
            setFraction(m_fraction + m_singleStep);
            break;
        case Qt::Key_Down:
        case Qt::Key_Minus: 
            setFraction(m_fraction - m_singleStep);
            break;
        case Qt::Key_Left:
            if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
                setFraction(m_fraction - m_singleStep);
            } else {
                setFraction(m_fraction + m_singleStep);
            }
            break;
        case Qt::Key_Right:
            if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
                setFraction(m_fraction + m_singleStep);
            } else {
                setFraction(m_fraction - m_singleStep);
            }
            break;
        case Qt::Key_PageUp:
            setFraction(m_fraction + m_pageStep);
            break;
        case Qt::Key_PageDown:
            setFraction(m_fraction - m_pageStep);
            break;
        case Qt::Key_Home:
            setFraction(0);
            break;
        case Qt::Key_End:
            setFraction(1);
            break;
        default:
            /* Quote from Qt documentation:
             * 
             *     “If you reimplement this handler, it is very important that
             *      you call the base class implementation if you do not act
             *      upon the key.
             * 
             *      The default implementation closes popup widgets if the
             *      user presses the key sequence for QKeySequence::Cancel
             *      (typically the Escape key). Otherwise the event is
             *      ignored, so that the widget's parent can interpret it.“ */
            QWidget::keyPressEvent(event);
    }
}

qreal GradientSelector::singleStep()
{
    return m_singleStep;
}

qreal GradientSelector::pageStep()
{
    return m_pageStep;
}

void GradientSelector::setSingleStep(qreal newSingleStep)
{
    m_singleStep = newSingleStep;
}

void GradientSelector::setPageStep(qreal newPageStep)
{
    m_pageStep= newPageStep;
}

// TODO It would be better to have an arrow outside the slider. This
// could be conformant with the current QStyle, and would guarantee
// a consistent contrast between the arrow and its background.
// TODO When zoom factor is 1,25, then background scaling is 1,25².
void GradientSelector::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    // We do not paint directly on the widget, but on a QImage buffer first:
    // Render anti-aliased looks better. But as Qt documentation says:
    //
    //      “Renderhints are used to specify flags to QPainter that may or
    //       may not be respected by any given engine.”
    //
    // Painting here directly on the widget might lead to different
    // anti-aliasing results depending on the underlying window system. This is
    // especially problematic as anti-aliasing might shift or not a pixel to the
    // left or to the right. So we paint on a QImage first. As QImage (at
    // difference to QPixmap and a QWidget) is independant of native platform
    // rendering, it guarantees identical anti-aliasing results on all
    // platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should use
    //       the platform independent QImage as paint device; i.e. using QImage
    //       will ensure that the result has an identical pixel representation
    //       on any platform.”
    QImage paintBuffer(size(), QImage::Format_ARGB32_Premultiplied);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);

    painter.setTransform(getTransform());
    if (!m_gradientImageReady) {
        updateGradientImage();
    }
    painter.drawImage(0, 0, m_gradientImage);
    

    int actualLength;
    if (m_orientation == Qt::Orientation::Vertical) {
        actualLength = size().height();
    } else {
        actualLength = size().width();
    }
    
    QPolygonF arrowPolygon;
    const qreal cursorPosition = actualLength * m_fraction;
    const qreal arrowSize = 6;
    arrowPolygon
        << QPointF(cursorPosition, arrowSize)
        << QPointF(cursorPosition + arrowSize, 0)
        << QPointF(cursorPosition - arrowSize, 0);
    painter.setBrush(QBrush(Qt::black));
    QPen pen(Qt::transparent);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawPolygon(arrowPolygon);
    arrowPolygon = QPolygonF(); // re-initialize
    arrowPolygon
        << QPointF(cursorPosition, m_gradientThickness - arrowSize)
        << QPointF(cursorPosition + arrowSize, m_gradientThickness)
        << QPointF(cursorPosition - arrowSize, m_gradientThickness);
    painter.setBrush(QBrush(Qt::white));
    painter.drawPolygon(arrowPolygon);
    if (hasFocus()) {
        pen.setWidth(2);
        pen.setColor(
            palette().color(QPalette::Highlight)
        );
        painter.setPen(pen);
        painter.drawLine(
            qRound(cursorPosition + arrowSize + 1),
            0,
            qRound(cursorPosition + arrowSize + 1),
            m_gradientThickness
        );
        painter.drawLine(
            qRound(cursorPosition - arrowSize),
            0,
            qRound(cursorPosition - arrowSize),
            m_gradientThickness
        );
    }

    // Paint the buffer to the actual widget
    QPainter(this).drawImage(0, 0, paintBuffer);
}

Qt::Orientation	GradientSelector::orientation() const
{
    return m_orientation;
}

void GradientSelector::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;
    if (m_orientation == Qt::Orientation::Vertical) {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    } else {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    m_gradientImageReady = false;
    updateGeometry(); // notify the layout system the the geometry has changed
}


void GradientSelector::setColors(const FullColorDescription &col1, const FullColorDescription &col2)
{
    if (col1 == m_firstColor && col2 == m_secondColor) {
        return;
    }
    m_firstColor = col1;
    m_secondColor = col2;
    m_gradientImageReady = false;
    update();
}

void GradientSelector::setFirstColor(const FullColorDescription &col)
{
    setColors(col, m_secondColor);
}

void GradientSelector::setSecondColor(const FullColorDescription &col)
{
    setColors(m_firstColor, col);
}

QPair<cmsCIELCh, qreal> GradientSelector::intermediateColor(const cmsCIELCh &firstColor, const cmsCIELCh &secondColor, qreal fraction)
{
    cmsCIELCh color;
    qreal alpha;
    color.L = firstColor.L + (secondColor.L - firstColor.L) * fraction;
    color.C = firstColor.C + (secondColor.C - firstColor.C) * fraction;
    color.h = firstColor.h + (secondColor.h - firstColor.h) * fraction;
    alpha = m_firstColor.alpha() + (m_secondColor.alpha() - m_firstColor.alpha()) * fraction;
    return QPair<cmsCIELCh, qreal>(color, alpha);
}

QTransform GradientSelector::getTransform() const
{
    QTransform transform;
    if (m_orientation == Qt::Orientation::Vertical) {
        transform.translate(0, size().height());
        transform.rotate(270);
    } else {
        if (layoutDirection() == Qt::RightToLeft) {
            transform.translate(size().width(), 0);
            transform.scale(-1, 1);
        }
    }
    return transform;
}

void GradientSelector::updateGradientImage()
{
    int actualLength;
    int i;
    if (m_orientation == Qt::Orientation::Vertical) {
        actualLength = size().height();
    } else {
        actualLength = size().width();
    }
    QImage temp(actualLength, 1, QImage::Format_ARGB32_Premultiplied);
    temp.fill(Qt::transparent); // Initialize the image with transparency.
    cmsCIELCh firstColor = m_firstColor.toLch();
    cmsCIELCh secondColor = m_secondColor.toLch();
    if (qAbs(firstColor.h - secondColor.h) > 180) {
        if (firstColor.h > secondColor.h) {
            secondColor.h += 360;
        } else {
            secondColor.h -= 360;
        }
    }
    QPair<cmsCIELCh, qreal> color;
    FullColorDescription fullColor;
    for (i = 0; i < actualLength; ++i) {
        color = intermediateColor(
            firstColor,
            secondColor,
            i / static_cast<qreal>(actualLength)
        );
        // TODO the in-gamut test fails because of rounding errors for
        // full-chroma-colors. How can we support ignore out-of-gamut
        // colors? How should they be rendered? Not identical to transparent,
        // right?
        fullColor = FullColorDescription(
            m_rgbColorSpace,
            color.first,
            FullColorDescription::outOfGamutBehaviour::preserve,
            color.second
        );
        temp.setPixelColor(i, 0, fullColor.toRgbQColor());
    }
    QImage result = QImage(
        actualLength,
        m_gradientThickness,
        QImage::Format_ARGB32_Premultiplied
    );
    QPainter painter(&result);
    painter.fillRect(
        0,
        0,
        actualLength,
        m_gradientThickness,
        QBrush(transparencyBackground(devicePixelRatioF()))
    );
    for (i = 0; i < m_gradientThickness; ++i) {
        painter.drawImage(0, i, temp);
    }
    m_gradientImage = result;
    m_gradientImageReady = true;
}

void GradientSelector::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_gradientImageReady = false;
}

}
