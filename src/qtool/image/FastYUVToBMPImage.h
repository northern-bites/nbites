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

#include "man/corpus/Camera.h"

//these defines fix a bug in one of the ffmpeg include
#ifdef __cplusplus
extern "C" {
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include <libswscale/swscale.h>
}
#endif //__cplusplus

namespace qtool {
namespace image {

class FastYUVToBMPImage : public image::BMPImage {

    Q_OBJECT;

public:
    FastYUVToBMPImage(data::RoboImages::const_ptr rawImages,
                      man::corpus::Camera::Type which,
                      QObject *parent = 0);
    virtual ~FastYUVToBMPImage();

    virtual unsigned getWidth() const {
        return rawImages->getPImage(which)->width();
    }

    virtual unsigned getHeight() const {
        return rawImages->getPImage(which)->width();
    }

    //in case the raw dimensions changed
    void rescaleBuffers();

protected:
    virtual void buildBitmap();

private:
    data::RoboImages::const_ptr rawImages;
    man::corpus::Camera::Type which;
    QImage qimage;
    byte* rgb_image;
    SwsContext* image_convert_context;
};

}
}
