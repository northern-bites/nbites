
#include "RobotGraphics.h"
#include <QColor>

namespace tool {
namespace playbook {

RobotGraphics::RobotGraphics(short x_, short y_, short h_,
                             short role_, const QColor color_) :
    QGraphicsObject(),
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
                          QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    painter->setBrush(color);
    painter->translate(x,y);
    painter->rotate(h);

    QPoint center(0,0);
    QRect robot(0, 0, 18, 35); //TODO: use constants here
    QPoint lineEnd(0.5*18, 0); //      and here

    robot.moveCenter(center);

    painter->drawRect(robot);
    painter->drawLine(center, lineEnd);

    painter->restore();
}

void RobotGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
}

void RobotGraphics::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // If the drag distance is too small, ignore it (elmininates mouse jitters)
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }

    QDrag* drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);

    mime->setColorData(color);
    QPixmap pixmap(18,35); //TODO: use constants here
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.translate(9,17.5); // Probably means it draws centered on the mouse
    paint(&painter,0,0);

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    // maybe set a hot spot if needed?

    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

void RobotGraphics::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
}

void RobotGraphics::setX(int x_) { x = x_; }
void RobotGraphics::setY(int y_) { y = y_; }
void RobotGraphics::setH(int h_) { h = h_; }
void RobotGraphics::setRole(char role_) { role = role_; }
void RobotGraphics::setColor(QColor color_) { color = color_; }

short RobotGraphics::getX() { return x; }
short RobotGraphics::getY() { return y; }
short RobotGraphics::getH() { return h; }
short RobotGraphics::getRole() { return role; }
QColor RobotGraphics::getColor() { return color; }

} // namespace playbook
} // namespace tool
