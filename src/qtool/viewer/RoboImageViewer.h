#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QPainter>
#include "data/DataTypes.h"
#include "image/BMPImage.h"

namespace qtool {
namespace viewer {

class RoboImageViewer: public QWidget {
    Q_OBJECT
public:

    RoboImageViewer(image::BMPImage::ptr image,
            QWidget *parent = NULL);
    virtual ~RoboImageViewer();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected slots:
    void updateView();

protected:
    void paintEvent(QPaintEvent *event);

private:
    image::BMPImage::ptr image;

};

}
}

#endif // RENDERAREA_H
