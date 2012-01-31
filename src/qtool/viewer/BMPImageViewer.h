/**
 * Renders a BMPImage
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include "data/DataTypes.h"
#include "image/BMPImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewer: public QWidget {
    Q_OBJECT
public:

    BMPImageViewer(image::BMPImage::ptr image,
            QWidget *parent = NULL);
    virtual ~BMPImageViewer();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);

protected:
    image::BMPImage::ptr image;
    QLabel imagePlaceholder;

};

}
}
