/**
 * @class BMPImage
 *
 * Wraps a basic bitmap image
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QImage>
#include "ClassHelper.h"

namespace qtool {
namespace image {

class BMPImage : public QObject {
    Q_OBJECT
    ADD_SHARED_PTR(BMPImage);

public:
    BMPImage() {}
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

}
}
