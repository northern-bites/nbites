#include "renderarea.h"

RenderArea::RenderArea(RoboImage r1, QWidget *parent)
    : QWidget(parent),
      r(r1)
{
    shape = Pixmap;
    antialiased = false;
    transformed = false;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    //r.read("/Users/ericchown/nbites/data/frames/graz/ballDist/1.NBFRM");
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
    int red, green, blue;
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
            draw.setCoords(j * 2, i * 2, j* 2 + 1, i * 2 + 1);
            painter.fillRect(draw, col);
        }
    }
}
