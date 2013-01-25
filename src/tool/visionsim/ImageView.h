/**
 * @class ImageView
 *
 * A QWidget that draws all of the objects in an Image as a simulation
 * of an actual robot image. Is a friend of the Image class, so it can
 * access all of the info about visual objects directly.
 * @see Image.h for info about the objects
 *
 * @author Lizzie Mamantov
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
    // Handles all of the painting
    void paintEvent(QPaintEvent* event);

private:
    // The image that this instance refers to in order to draw
    Image& image;
};

}
}
