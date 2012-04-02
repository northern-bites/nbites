/**
 * @class BMPImage
 *
 * Wraps a basic bitmap image
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QImage>
#include <QPainter>
#include "ClassHelper.h"

namespace qtool {
namespace image {

class BMPImage : public QObject {
    Q_OBJECT

public:
    BMPImage(QObject* parent = 0) : QObject(parent) {}
    virtual ~BMPImage() {}

    const QImage* getBitmap() const { return &bitmap; }
    QImage* getBitmap() { return &bitmap; }

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
    QImage bitmap;

};

//yo dawg I head you liked BMPImage so we merged two BMPImages in a class that
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

    virtual unsigned getWidth() const { return baseImage->getWidth(); }
    virtual unsigned getHeight() const { return baseImage->getHeight(); }

protected:
    virtual void buildBitmap() {

        baseImage->updateBitmap();
        overlayedImage->updateBitmap();

        if (bitmap.height() < getHeight() || bitmap.width() < getWidth()) {
            bitmap = QImage(getWidth(), getHeight(), QImage::Format_ARGB32_Premultiplied);
        }

        QPainter painter(&bitmap);

        painter.drawImage(0, 0, *(baseImage->getBitmap()));
        if (overlayedImage) {
            painter.drawImage(baseImage->getBitmap()->rect(), *(overlayedImage->getBitmap()));
        }
    }

protected:
    BMPImage* baseImage;
    BMPImage* overlayedImage;
};

}
}
