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
  return QSize(800, 600);
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
		      
     QRect field(0, 0, 740, 540);
     QRect playArea(70, 70, 600, 400);
     QRect blueGoal(30, 200, 40, 140);
     QRect yellowGoal(670, 200, 40, 140);

     /*
     int startAngle = 20 * 16;
     int arcLength = 120 * 16;
     */

     QPainter painter(this);
     /*
       painter.setPen(pen);
     */
     painter.fillRect(field, Qt::darkGreen);

     painter.setBrush(Qt::NoBrush);
     painter.setPen(Qt::white);
     painter.drawPolyline(blueBox, 4);
     painter.drawPolyline(yellowBox, 4);
     painter.drawRect(playArea);
     painter.drawLine(((playArea.topLeft()+
			playArea.topRight())/2),
		      ((playArea.bottomLeft()+
			playArea.bottomRight())/2));

     QPoint centerField = QPoint(370, 270);
     painter.drawEllipse (centerField, 60, 60);
     
     painter.drawLine(250, 265, 250, 275);
     painter.drawLine(245, 270, 255, 270);
     painter.drawLine(485, 270, 495, 270);
     painter.drawLine(490, 265, 490, 275);

     QPoint topBluePost = QPoint(70, 200);
     QPoint bottomBluePost = QPoint(70, 340);
     QPoint topYellowPost = QPoint(670, 200);
     QPoint bottomYellowPost = QPoint(670, 340);
     
     int radius = 5;

     painter.setBrush(Qt::blue);
     painter.setPen(Qt::blue);
     painter.drawEllipse(topBluePost, radius, radius);
     painter.drawEllipse(bottomBluePost, radius, radius);
     painter.setBrush(Qt::yellow);
     painter.setPen(Qt::yellow);
     painter.drawEllipse(topYellowPost, radius, radius);
     painter.drawEllipse(bottomYellowPost, radius, radius);

     QBrush goals = QBrush(Qt::white, Qt::CrossPattern);
     painter.setBrush(goals);
     painter.setPen(Qt::blue);
     painter.drawRect(blueGoal);
     painter.setPen(Qt::yellow);
     painter.drawRect(yellowGoal);

     
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
 
