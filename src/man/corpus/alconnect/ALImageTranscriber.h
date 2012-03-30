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

private: // nBites Camera Constants
    // Camera identification
    static const int TOP_CAMERA ;
    static const int BOTTOM_CAMERA ;

    // Camera setup information
    static const int CAMERA_SLEEP_TIME ;
    static const int CAM_PARAM_RETRIES ;

    static const int DEFAULT_CAMERA_RESOLUTION ;
    static const int DEFAULT_CAMERA_FRAMERATE ;
    static const int DEFAULT_CAMERA_BUFFERSIZE ;
    static const int DEFAULT_CAMERA_AUTO_GAIN ;
    static const int DEFAULT_CAMERA_GAIN ;
    static const int DEFAULT_CAMERA_AUTO_WHITEBALANCE ;
    static const int DEFAULT_CAMERA_BLUECHROMA ;
    static const int DEFAULT_CAMERA_REDCHROMA ;
    static const int DEFAULT_CAMERA_BRIGHTNESS ;
    static const int DEFAULT_CAMERA_CONTRAST ;
    static const int DEFAULT_CAMERA_SATURATION ;
    static const int DEFAULT_CAMERA_HUE ;
    static const int DEFAULT_CAMERA_LENSX ;
    static const int DEFAULT_CAMERA_LENSY ;
    static const int DEFAULT_CAMERA_AUTO_EXPOSITION ;
    static const int DEFAULT_CAMERA_EXPOSURE ;
    static const int DEFAULT_CAMERA_HFLIP ;
    static const int DEFAULT_CAMERA_VFLIP ;

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
