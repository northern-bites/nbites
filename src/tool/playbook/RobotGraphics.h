
#include <QtGui>
#include <QGraphicsObject>

namespace tool {
namespace playbook {

class RobotGraphics : public QGraphicsObject
{
public:
    RobotGraphics(short x_, short y_, short h_,
                  short role_, const QColor color_);
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setX(int x_);
    void setY(int y_);
    void setH(int h_);
    void setRole(char role_);
    void setColor(QColor color_);

    short getX();
    short getY();
    short getH();
    short getRole();
    QColor getColor();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    short x,y,h,role;
    QColor color;
};

} // namespace playbook
} // namespace tool
