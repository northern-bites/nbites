#ifndef WBImageTranscriber_h
#define WBImageTranscriber_h

#include "ThreadedImageTranscriber.h"

#include <webots/robot.h>
#include <webots/camera.h>

class WBImageTranscriber : public ImageTranscriber{
public:
    WBImageTranscriber(boost::shared_ptr<Sensors> s);
    ~WBImageTranscriber();

    void releaseImage();

public:
    void waitForImage();

private: //members
    WbDeviceTag camera;

};

#endif
