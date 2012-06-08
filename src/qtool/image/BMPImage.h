/**
 * @class BMPImage
 *
 * Wraps a basic bitmap image
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QPixmap>
#include <QPainter>
#include "ClassHelper.h"

namespace qtool {
namespace image {

class BMPImage : public QObject {
    Q_OBJECT

public:
    BMPImage(QObject* parent = 0) : QObject(parent) {}
    virtual ~BMPImage() {}

    const QPixmap* getBitmap() const { return &bitmap; }
    QPixmap* getBitmap() { return &bitmap; }

    virtual unsigned getWidth() const = 0;
    virtual unsigned getHeight() const = 0;

public slots:
    void updateBitmap() {
        this->buildBitmap();
        emit bitmapBuilt();
    }

signals:
    void bitmapBuilt();

protected:
    virtual void buildBitmap() = 0;

protected:
    QPixmap bitmap;

};

//yo dawg I heard you liked BMPImage so we merged two BMPImages in a class that
//inherits from BMPImage

class OverlayedImage : public BMPImage {
    Q_OBJECT

public:
    OverlayedImage(BMPImage* baseImage,
                   BMPImage* overlayedImage,
                   QObject* parent = 0) :
       BMPImage(parent),
       baseImage(baseImage),
       overlayedImage(overlayedImage) {

       }

    virtual unsigned getWidth() const {
		if(baseImage->getWidth()>overlayedImage->getWidth())
			return baseImage->getWidth();
		else return overlayedImage->getWidth();
	}
    virtual unsigned getHeight() const {
		if(baseImage->getHeight()>overlayedImage->getHeight())
			return baseImage->getHeight();
		else return overlayedImage->getHeight();
	}

protected:
    virtual void buildBitmap() {

        baseImage->updateBitmap();

        if (bitmap.height() < getHeight() || bitmap.width() < getWidth()) {
            bitmap = QPixmap(getWidth(), getHeight());
        }

        QPainter painter(&bitmap);

        painter.drawPixmap(0, 0, *(baseImage->getBitmap()));
        if (overlayedImage) {
            overlayedImage->updateBitmap();
            painter.drawPixmap(baseImage->getBitmap()->rect(), *(overlayedImage->getBitmap()));
        }
    }

protected:
    BMPImage* baseImage;
    BMPImage* overlayedImage;
};

}
}
