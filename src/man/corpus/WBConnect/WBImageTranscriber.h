#ifndef WBImageTranscriber_h
#define WBImageTranscriber_h

#include "ThreadedImageTranscriber.h"

#include <webots/robot.h>
#include <webots/camera.h>

struct YUV {
    unsigned char Y;
    unsigned char U;
    unsigned char V;
};


class WBImageTranscriber : public ImageTranscriber{
 public:
    WBImageTranscriber(boost::shared_ptr<Sensors> s);
    ~WBImageTranscriber();

    void releaseImage();

 public:
    void waitForImage();

 private:
    void setTwoYUV(unsigned char *image, const int baseIndex,
                   const YUV yuv1, const YUV yuv2);
    const YUV getWBYUVFromRGB(const unsigned char image[], const int baseIndex);

 private: //members
    WbDeviceTag camera;
    unsigned char *image;

    static const int WEBOTS_IMAGE_HEIGHT;
    static const int WEBOTS_IMAGE_WIDTH;
    static const int HEIGHT_SCALE;
    static const int WIDTH_SCALE;
    static const int Y1_OFFSET;
    static const int U_OFFSET;
    static const int V_OFFSET;
    static const int Y2_OFFSET;
};

#endif
