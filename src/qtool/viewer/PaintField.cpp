#include "PaintField.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

PaintField::PaintField(QWidget *parent): 
		   QWidget(parent)
{

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

}

QSize PaintField::sizeHint() const
{
  return QSize(800, 800);
}

QSize PaintField::minimumSizeHint() const
{
  return QSize(100, 100);
}

void PaintField::ballDataChanged(const QBrush &brush)
{
  update(); // updates the painting
}

void PaintField::paintEvent(QPaintEvent * /* event */)
 {
     static const QPoint corners[5] = {
         QPoint(70, 70),
         QPoint(670, 70),
         QPoint(670, 470),
         QPoint(70, 470),
	 QPoint(70, 70)
     };
     static const QPoint blueBox[4] = {
       QPoint(70, 160),
       QPoint(130, 160),
       QPoint(130, 380),
       QPoint(70, 380)
     };
     static const QPoint yellowBox[4] = {
       QPoint(670, 160),
       QPoint(610, 160),
       QPoint(610, 380),
       QPoint(670, 380)
     };
		      

     QRect rect(70, 70, 600, 400);

     /*
     int startAngle = 20 * 16;
     int arcLength = 120 * 16;
     */

     QPainter painter(this);
     /*
     painter.setPen(pen);
     painter.setBrush(brush);
     */
 
     painter.drawPolyline(blueBox, 4);
     painter.drawPolyline(yellowBox, 4);
     painter.drawRect(rect);
     painter.drawLine(((rect.topLeft()+rect.topRight())/2),
		      ((rect.bottomLeft()+rect.bottomRight())/2));

     QPoint centerField = QPoint(370, 270);
     painter.drawEllipse (centerField, 60, 60);

     
     /*
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
     */
}

}
}
 
