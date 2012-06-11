/**
 * @class PaintFieldOverlay
 *
 * Customizible class for drawing objects on a field overlay
 *
 * @author Octavian Neamtu
 * @date April 2012
 *
 */

#pragma once

#include <QtGui>
#include <vector>

#include "Common.h"

#include "image/BMPImage.h"
#include "man/include/FieldConstants.h"

namespace qtool {
namespace image {

class PaintFieldOverlay : public image::BMPImage {

    Q_OBJECT

public:
    PaintFieldOverlay(float scale = 1.0f, QObject *parent = 0);
    virtual ~PaintFieldOverlay(){}

    unsigned getWidth() const { return FIELD_WIDTH*scale; }
    unsigned getHeight() const { return FIELD_HEIGHT*scale; }

    void transformPainterToFieldCoordinates(QPainter& painter);
    void paintDot(QPainter& painter, QColor color, QPoint point, int size = 7);
    void paintEllipseArea(QPainter& painter, QColor color,
                          QPoint center, int x_size, int y_size);
    void paintPolarLine(QPainter& painter, QColor color, int width,
                        QPoint start, int distance, angle::degrees theta);

protected:
    virtual void buildBitmap() = 0;

protected:
    float scale;

};

}
}
