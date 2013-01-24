/**
 * QWidget for showing the objects in Image
 */

#pragma once

#include <QWidget>
#include <QPainter>

#include "ImageConstants.h"
#include "Image.h"

namespace tool{
namespace visionsim{

class ImageView : public QWidget {
    Q_OBJECT;

public:
    ImageView(Image& i, QWidget* parent = 0);
    ~ImageView() {};

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    Image& image;
};

}
}
