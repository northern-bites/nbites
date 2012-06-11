
#include "FastYUVToBMPImage.h"

#include <iostream>

namespace qtool {
namespace image {

using namespace data;
using namespace std;
using namespace man::corpus;

FastYUVToBMPImage::FastYUVToBMPImage(RoboImages::const_ptr rawImages,
                                     Camera::Type which, QObject* parent) :
        BMPImage(parent), rawImages(rawImages), which(which),
        rgb_image(NULL), image_convert_context(NULL) {
}

FastYUVToBMPImage::~FastYUVToBMPImage() {
    sws_freeContext(image_convert_context);
    free(rgb_image);
}

void FastYUVToBMPImage::rescaleBuffers() {

    int image_width = rawImages->getPImage(which)->width();
    int image_height = rawImages->getPImage(which)->height();

    if (image_convert_context) {
        sws_freeContext(image_convert_context);
    }

    image_convert_context = sws_getContext(
            image_width,
            image_height,
            PIX_FMT_YUYV422,
            image_width,
            image_height,
            PIX_FMT_BGRA,
            SWS_FAST_BILINEAR,
            NULL, NULL, NULL);

    if (image_convert_context == NULL) {
        std::cerr << "Could not initialize image conversion context "
                <<__FILE__ << __LINE__ << endl;
        sws_freeContext(image_convert_context);
        return;
    }

    int image_size = image_height*image_width*4;
    if (image_size > sizeof(rgb_image)) {
        rgb_image = (byte*) realloc(rgb_image, image_size);
    }

    qimage = QImage(rgb_image, image_width, image_height, QImage::Format_ARGB32_Premultiplied);

}

void FastYUVToBMPImage::buildBitmap() {

    const byte* yuyv_image_data = (byte*) rawImages->getPImage(which)->image().data();

    int image_width = rawImages->getPImage(which)->width();
    int image_height = rawImages->getPImage(which)->height();

    if (!image_width || !image_height)
        return;

    if (image_width > qimage.width() ||
        image_height > qimage.height()) {
        this->rescaleBuffers();
    }

    int yuyv_image_line_size = 2*image_width;
    int rgb_line_size = 4*image_width;

    //TODO: integrate the stride  or format of the image with image info in MImage
    sws_scale(image_convert_context,
            &yuyv_image_data,
            &yuyv_image_line_size,
            0,
            image_height,
            &rgb_image,
            &rgb_line_size
            );

    bitmap.convertFromImage(qimage);
}

}
}
