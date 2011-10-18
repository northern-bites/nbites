#include "PaintField.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

PaintField::PaintField(QWidget *parent):
    QWidget(parent){

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

}

QSize PaintField::sizeHint() const
{
  return QSize(400, 200);
}

QSize PaintField::minimumSizeHint() const
{
  return QSize(100, 100);
}

void PaintField::ballDataChanged(const QBrush &brush)
 {

   update(); // updates the painting
 }

void RenderArea::paintEvent(QPaintEvent * /* event */)
 {
     static const QPoint points[4] = {
         QPoint(10, 80),
         QPoint(20, 10),
         QPoint(80, 30),
         QPoint(90, 70)
     };

     QRect rect(10, 20, 80, 60);

     QPainterPath path;
     path.moveTo(20, 80);
     path.lineTo(20, 30);
     path.cubicTo(80, 0, 50, 50, 80, 80);

     int startAngle = 20 * 16;
     int arcLength = 120 * 16;

     QPainter painter(this);
     painter.setPen(pen);
     painter.setBrush(brush);

     switch (shape) {
     case Line:
       painter.drawLine(rect.bottomLeft(), rect.topRight());
       break;
     case Polyline:
       painter.drawPolyline(points, 4);
       break;
     case Polygon:
       painter.drawPolygon(points, 4);
       break;
     case Rect:
       painter.drawRect(rect);
       break;
     case Ellipse:
       painter.drawEllipse(rect);
       break;
     case Arc:
       painter.drawArc(rect, startAngle, arcLength);
       break;
     case Path:
       painter.drawPath(path);
       break;
     case Pixmap:
       painter.drawPixmap(10, 10, pixmap);
     }

}
}
 
