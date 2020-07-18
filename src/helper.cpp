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

// Own header
#include "PerceptualColor/helper.h"

#include "PerceptualColor/rgbcolorspace.h"
#include "PerceptualColor/polarpointf.h"

#include <QDebug>
#include <QPainter>

#include <limits>
#include <math.h>

#include <lcms2.h>

namespace PerceptualColor {

namespace Helper {

    QImage transparencyBackground()
    {
        // The width and height of 12 px per square might be a good choise
        // because 12 is a multiple of 2, 3 and 4, which might make scaling
        // with better quality easier. But 12 px is also quite big…
        const int squareSize = 6;
        QImage temp(squareSize * 2, squareSize * 2, QImage::Format_RGB32);
        temp.fill(QColor(102, 102, 102));
        QPainter painter(&temp);
        QColor foregroundColor(153, 153, 153);
        painter.fillRect(0, 0, squareSize, squareSize, foregroundColor);
        painter.fillRect(squareSize, squareSize, squareSize, squareSize, foregroundColor);
        return temp;
    }

    /** @brief Convert to LCh
     * 
     * Convenience function
     * @param lab a point in Lab color space
     * @returns the same point in LCh representation */
    cmsCIELCh toLch(const cmsCIELab &lab)
    {
        cmsCIELCh temp;
        cmsLab2LCh(&temp, &lab);
        return temp;
    }

    /** @brief Convert to Lab
     * 
     * Convenience function
     * @param lch a point in Lab color space
     * @returns the same point in LCh representation */
    cmsCIELab toLab(const cmsCIELCh &lch)
    {
        cmsCIELab temp;
        cmsLCh2Lab(&temp, &lch);
        return temp;
    }


    /** @brief Search the nearest non-transparent neighbor pixel
    * 
    * TODO @todo This code is a terribly inefficient implementation of a “nearest neigbor search”. See
    * https://stackoverflow.com/questions/307445/finding-closest-non-black-pixel-in-an-image-fast
    * for a better approach.
    * 
    * @param originalPoint The point for which you search the nearest neigbor,
    * expressed in the coordinate system of the image. This point may be within
    * or outside the image.
    * @param image The image in which the nearest neigbor is searched.
    * Must contain at least one pixel with an alpha value that is fully opaque.
    * @returns
    * \li If originalPoint itself is within the image and a
    * non-transparent pixel, it returns originalPoint.
    * \li Else, if there is are non-transparent pixels in the image, the nearest non-transparent
    * pixel is returned. (If there are various nearest neigbors at the same distance, it is
    * undefined which one is returned.)
    * \li Else there are no non-transparent pixels, and simply the point <tt>0, 0</t> is returned,
    * but this is a very slow case.
    */
    QPoint nearestNeighborSearch(const QPoint originalPoint, const QImage &image) {
        // test for special case: originalPoint itself is within the image and non-transparent
        if (image.valid(originalPoint)) {
            if (image.pixelColor(originalPoint).alpha() == 255) {
                return originalPoint;
            }
        }

        // No special case. So we have to actually perfor a nearest-neighbor-search.
        int x;
        int y;
        int currentBestX;
        int currentBestY;
        int currentBestDistanceSquare = std::numeric_limits<int>::max();
        int temp;
        for (x = 0; x < image.width(); x++) {
            for (y = 0; y < image.height(); y++) {
                if (image.pixelColor(x, y).alpha() == 255) {
                    temp = pow(originalPoint.x() - x, 2) + pow(originalPoint.y() - y, 2);
                    if (temp < currentBestDistanceSquare) {
                        currentBestX = x;
                        currentBestY = y;
                        currentBestDistanceSquare = temp;
                    }
                }
            }
        }
        if (currentBestDistanceSquare == std::numeric_limits<int>::max()) {
            return QPoint(0, 0);
        } else {
            return QPoint(currentBestX, currentBestY);
        }
    }

    /** @brief Number of steps done by a wheel event
     * 
     * As of Qt documentation, there are different mouse wheels which can give
     * different step sizes. This function provides a standard way to calculate
     * the step size for a standard mouse.
     * 
     * @param event the QWheelEvent
     * @returns the number of steps in y axis (this is the “normal” mouse
     * wheel). The value is positive of up-steps and negative for down-steps.
     * It might not be an integer. */
    qreal wheelSteps(QWheelEvent *event)
    {
        return event->angleDelta().y() / static_cast<qreal>(120);
    }
}

}
