/**
 * @class PaintGroundTruth
 *
 * A simple class that paints some ground truth data to a bitmap
 * Useful when overlayed on a field
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "BMPImage.h"

#include "overseer/GroundTruth.h"
#include "FieldConstants.h"

#include "Color.h"

namespace qtool {
namespace image {

class PaintGroundTruth : public BMPImage {

    Q_OBJECT

    static const int CIRCLE_DIM = 10;

public:
    PaintGroundTruth(overseer::GroundTruth::const_ptr groundTruth,
                     QObject* parent) :
        BMPImage(parent), groundTruth(groundTruth) {

        bitmap = QPixmap(getWidth(), getHeight());
    }

    virtual unsigned getWidth() const { return FIELD_WIDTH; }
    virtual unsigned getHeight() const { return FIELD_HEIGHT; }

protected:
    virtual void buildBitmap() {

        bitmap.fill(Qt::transparent);

        QPainter painter(&bitmap);

        QColor orange = QColor::fromRgb(Color_RGB[Orange]);
        painter.setPen(orange);
        painter.setBrush(orange);
        painter.drawEllipse(QPoint(groundTruth->get()->ball().x(),
                                   groundTruth->get()->ball().y()),
                            CIRCLE_DIM, CIRCLE_DIM);

        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        for (int i = 0; i < groundTruth->get()->robots_size(); i++) {
              overseer::proto::GroundTruth::Point robot = groundTruth->get()->robots(i);
              painter.drawEllipse(QPoint(robot.x(), robot.y()), CIRCLE_DIM, CIRCLE_DIM);
        }

    }

protected:
    overseer::GroundTruth::const_ptr groundTruth;

};

}
}
