#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include "roboimage.h"
#include "math.h"

class RenderArea : public QWidget
{
    Q_OBJECT
public:
    enum Shape { Y, U, V, Blue, Red, Green, H, S, Z, WHEEL, Pixmap };

    RenderArea(RoboImage r1, QWidget *parent = 0);

    void  setRoboImage(RoboImage img) { r = img;}
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
    RoboImage r;

signals:


};

#endif // RENDERAREA_H
