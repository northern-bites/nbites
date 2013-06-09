
#include "RobotGraphics.h"
#include <QColor>

namespace tool {
namespace playbook {

RobotGraphics::RobotGraphics(int x_, int y_, int h_,
                             char role_, QColor color_) :
    x(x_),
    y(y_),
    h(h_),
    role(role_),
    color(color_)
{
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

QRectF RobotGraphics::boundingRect() const
{
    return QRectF(0,0,18,35);
}

void RobotGraphics::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    painter->setBrush(color);
    painter->translate(x,y);
    painter->rotate(h);

    QPoint center(0,0);
    QRect robot(0, 0, 18, 35);
    QPoint lineEnd(0.125*x, 0);

    robot.moveCenter(center);

    painter->drawRect(robot);
    painter->drawLine(center, lineEnd);

    painter->restore();
}

void mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    return;
}

void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    return;
}

void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    return;
}

void RobotGraphics::setX(int x_) { x = x_; }
void RobotGraphics::setY(int y_) { y = y_; }
void RobotGraphics::setH(int h_) { h = h_; }
void RobotGraphics::setRole(char role_) { role = role_; }
void RobotGraphics::setColor(QColor color_) { color = color_; }

int RobotGraphics::getX() { return x; }
int RobotGraphics::getY() { return y; }
int RobotGraphics::getH() { return h; }
char RobotGraphics::getRole() { return role; }
QColor RobotGraphics::getColor() { return color; }

} // namespace playbook
} // namespace tool
