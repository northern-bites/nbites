#include "renderarea.h"
#include <QMouseEvent>

RenderArea::RenderArea(RoboImage r1, QLabel *inf, QWidget *parent)
    : QWidget(parent),
      r(r1),
      info(inf)
{
    shape = Pixmap;
    antialiased = false;
    transformed = false;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
    edgediff = 10;
}

void RenderArea::mouseMoveEvent(QMouseEvent *event) {
    int x = event->x() / 2;
    int y = event->y() / 2;
    QString xS;
    xS.setNum(x);
    QString yS;
    yS.setNum(y);
    QString yy;
    yy.setNum(r.getY(x, y));
    QString u;
    u.setNum(r.getU(x, y));
    QString v;
    v.setNum(r.getV(x, y));
    QString temp = "x, y: "+ xS+" "+yS+"\nYUV: "+yy+" "+u+" "+v;
    info->setText(temp);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(640, 480);
}

void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RenderArea::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{

    QRect rect(10, 20, 80, 60);

    QPainterPath path;
    /*path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);*/

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    QTextStream out(stdout);
    QRect draw;
    int red, green, blue, edge;
    bool found;
    for (int i = 0; i < r.getHeight(); i++)
    {
        for (int j = 0; j < r.getWidth(); j++)
        {
            float radius = 120.0f * 120.0f;
            float h, s, v;
            float dist = (120 - i) * (120 - i) + (160 - j) * (160 - j);
            switch (shape) {
            case V:
                red = green = blue = r.getV(j, i);
                break;
            case Blue:
                red = green = 0;
                blue = r.getBlue(j, i);
                break;
            case Y:
                red = green = blue = r.getY(j, i);
                break;
            case U:
                red = green = blue = r.getU(j, i);
                break;
            case Green:
                red = blue = 0;
                green = r.getGreen(j, i);
                break;
            case Red:
                blue = green = 0;
                red = r.getRed(j, i);
                break;
            case H:
                red = green = blue = r.getH(j, i);
                break;
            case S:
                red = green = blue = r.getS(j, i);
                break;
            case Z:
                red = green = blue = r.getZ(j, i);
                break;
            case EDGE:
                red = green = blue = r.getY(j, i);
                found = false;
                if (j > 0 && i > 1) {
                    edge = abs(r.getY(j - 1, i) - r.getY(j, i));
                    edge = max(abs(r.getY(j, i) - r.getY(j, i - 1)), edge);
                    if (edge  > edgediff) {
                        red = 255;
                        green = 0;
                        blue = 0;
                        found = true;
                    }
                    edge = abs(r.getU(j - 1, i) - r.getU(j, i));
                    edge = max(abs(r.getU(j, i) - r.getU(j, i - 1)), edge);
                    if (edge > edgediff) {
                        green = 255;
                        blue = 0;
                        if (!found) {
                            red = 0;
                        }
                        found = true;
                    }
                    edge = abs(r.getV(j - 1, i) - r.getV(j, i));
                    edge = max(abs(r.getV(j, i) - r.getV(j, i - 1)), edge);
                    if (edge > edgediff) {
                        blue = 255;
                        if (!found) {
                            red = 0;
                            green = 0;
                        }
                    }
                }
                break;
            case WHEEL:

                if (dist < radius)
                {
                    s = dist / radius;
                    h = atan2(160 - j, 120 - i) / (2.0f * 3.14159f);
                    v = 0.5;
                    ColorSpace c;
                    c.setHsz(h, s, v);
                    red = c.getRb();
                    green = c.getGb();
                    blue = c.getBb();
                } else
                {
                    red = green = blue = 0;
                }
                break;
            case Pixmap:
                red = r.getRed(j, i);
                green = r.getGreen(j, i);
                blue = r.getBlue(j, i);
                break;
            }
            QColor col(red, green, blue);
            painter.setPen(col);
            draw.setCoords(j, i, j + 1, i + 1);
            painter.fillRect(draw, col);
        }
    }
}
