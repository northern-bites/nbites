
#include <QtGui>

namespace tool {
namespace playbook {

class RobotGraphics : public QGraphicsObject
{
public:
    RobotGraphics(int x_, int y_, int h_, char role_, const QColor color_);
    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setX(int x_);
    void setY(int y_);
    void setH(int h_);
    void setRole(char role_);
    void setColor(QColor color_);

    int getX();
    int getY();
    int getH();
    char getRole();
    QColor getColor();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    int x,y,h;
    char role;
    QColor color;
};

} // namespace playbook
} // namespace tool
