/**
 * Renders a BMPImage by transforming it to a QPixmap and attaching
 * it to a QLabel
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include "data/DataTypes.h"
#include "image/BMPYUVImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewer: public QWidget {
    Q_OBJECT
public:

    BMPImageViewer(image::BMPImage::ptr image,
            QWidget *parent = NULL);

    virtual ~BMPImageViewer();

    virtual QSize minimumSizeHint() const {
        return QSize(image->getWidth(), image->getHeight());
    }

protected slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);
    void setupUI();

protected:
    image::BMPImage::ptr image;
    QLabel imagePlaceholder;

};

}
}
