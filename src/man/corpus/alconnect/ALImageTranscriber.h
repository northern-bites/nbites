#ifndef ALIMAGE_TRANSCRIBER_H
#define ALIMAGE_TRANSCRIBER_H

#include "alproxies/almemoryproxy.h"
#include "alcommon/albroker.h"
#include "alproxies/alloggerproxy.h"
#include "alproxies/alvideodeviceproxy.h"
#include "alerror/alerror.h"

#include "ThreadedImageTranscriber.h"
#include "synchro/synchro.h"
#include "ColorParams.h"

class ALImageTranscriber : public ThreadedImageTranscriber {
public:
    ALImageTranscriber(boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<AL::ALBroker> broker);
    virtual ~ALImageTranscriber();

private:
    ALImageTranscriber(const ALImageTranscriber &other);
    void operator= (const ALImageTranscriber &other);

public:

    int start();
    void run();
    void stop();
    void releaseImage();

private: // helper methods
    void registerCamera(boost::shared_ptr<AL::ALBroker> broker);
    void setCameraParameter(int paramId, int param);
    void initCameraSettings(int whichCam);
    void initTable(const std::string& path);
    void initTable(unsigned char* buffer);
    void waitForImage();

private: // member variables
    // Interfaces/Proxies to robot

    boost::shared_ptr<AL::ALLoggerProxy> log;
    boost::shared_ptr<AL::ALVideoDeviceProxy> camera;

    std::string lem_name;

    bool camera_active;

    // Keep a local copy of the image because accessing the one from
    // NaoQi is very slow.
    uint16_t *image;

    unsigned char *table;
    ColorParams params;

private:
    // Camera setup information
    static const int CAMERA_SLEEP_TIME = 200;
    static const int DEFAULT_FRAMERATE;

    enum {
        y0 = 0,
        u0 = 0,
        v0 = 0,

        y1 = 256,
        u1 = 256,
        v1 = 256,

        yLimit = 128,
        uLimit = 128,
        vLimit = 128,

        tableByteSize = yLimit * uLimit * vLimit
    };

};

#endif
