/**
 * @class RoboImageViewer
 *
 * Takes an MImage and converts it fast into a ARGB format
 *
 * It does this using the raw YUY2 (YUYV) data and ffmpeg swscale library
 *
 * see
 * http://dranger.com/ffmpeg/tutorial02.html
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QLayout>

#include "data/DataTypes.h"

#include "image/BMPImage.h"

namespace qtool {
namespace image {

class FastYUVToBMPImage : public image::BMPImage {

    Q_OBJECT;

public:
    FastYUVToBMPImage(data::RoboImage::const_ptr rawImage, QObject *parent = 0);
    virtual ~FastYUVToBMPImage() {}

    virtual unsigned getWidth() const {
        return roboImage->get()->width();
    }

    virtual unsigned getHeight() const {
        return roboImage->get()->height();
    }

protected:
    virtual void buildBitmap();

private:
    data::RoboImage::const_ptr roboImage;
    QImage qimage;
//    byte* rgb_image;
};

}
}
