
#include "PaintFieldObjects.h"

namespace qtool {
namespace image {

PaintFieldOverlay::PaintFieldOverlay(float scale, QObject* parent)
    : BMPImage(parent), scale(scale) {
    bitmap = QImage(FIELD_WIDTH*scale, FIELD_HEIGHT*scale, QImage::Format_ARGB32_Premultiplied);
}

void PaintFieldOverlay::transformPainterToFieldCoordinates(QPainter& painter) {
    // translate and flip the coords to make the Qt and robot coord systems match up
    painter.translate(0, FIELD_HEIGHT*scale);
    painter.scale(scale, -1*scale);
}
void PaintFieldOverlay::unTransform(QPainter& painter){
	//put the painter back the deafult Qt way
	painter.scale(1/scale, -1/scale);
	painter.translate(0, -FIELD_HEIGHT*scale);
}
void PaintFieldOverlay::paintDot(QPainter& painter, QColor color, QPoint point, int size) {

    painter.setPen(QColor("black"));
    painter.setBrush(color);
    painter.drawEllipse(point, size, size);
}

void PaintFieldOverlay::paintEllipseArea(QPainter& painter, QColor color,
                                         QPoint center, int x_size, int y_size) {

    painter.setPen(color);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, x_size, y_size);
}

void PaintFieldOverlay::paintPolarLine(QPainter& painter, QColor color, int width,
                                       QPoint start, int distance, angle::degrees theta) {

    angle::radians theta_rad = theta * TO_RAD;
    QPoint cartesian_vector = QPoint(distance * cos(theta_rad), distance * sin(theta_rad));

    QPen pen;
    pen.setWidth(width);
    pen.setColor(color);
    painter.setPen(pen);
    painter.drawLine(start, start + cartesian_vector);
}

}
}
