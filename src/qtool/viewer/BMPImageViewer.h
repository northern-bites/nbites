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
#include "image/FastYUVToBMPImage.h"

namespace qtool {
namespace viewer {

class BMPImageViewer: public QWidget {
    Q_OBJECT
public:

    BMPImageViewer(image::BMPImage* image,
            QWidget *parent = NULL);

    virtual ~BMPImageViewer();

    virtual QSize minimumSizeHint() const {
        return QSize(image->getWidth(), image->getHeight());
    }

	virtual unsigned getWidth() { return image->getWidth();}
	virtual unsigned getHeight() { return image->getHeight();}


public slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);
    void setupUI();

protected:
    image::BMPImage* image;
    QLabel imagePlaceholder;

};

class RoboImageViewer: public BMPImageViewer {
    Q_OBJECT

public:
    RoboImageViewer(data::RoboImage::const_ptr rawImage, QWidget* parent = 0) :
        BMPImageViewer(new image::FastYUVToBMPImage(rawImage, parent), parent) {

        }
};

}
}
