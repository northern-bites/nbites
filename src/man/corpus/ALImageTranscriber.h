#ifndef ALIMAGE_TRANSCRIBER_H
#define ALIMAGE_TRANSCRIBER_H

#include "almemoryproxy.h"
#include "albroker.h"
#include "alptr.h"
#include "alloggerproxy.h"

#include "ThreadedImageTranscriber.h"
#include "synchro.h"

class ALImageTranscriber : public ThreadedImageTranscriber {
public:
    ALImageTranscriber(boost::shared_ptr<Synchro> synchro,
                       boost::shared_ptr<Sensors> s,
                       AL::ALPtr<AL::ALBroker> broker);
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
    void registerCamera(AL::ALPtr<AL::ALBroker> broker);
    void initCameraSettings(int whichCam);
    void waitForImage();

private: // member variables
    // Interfaces/Proxies to robot

    AL::ALPtr<AL::ALLoggerProxy> log;
    AL::ALPtr<AL::ALProxy> camera;

    std::string lem_name;

    bool camera_active;

    // Keep a local copy of the image because accessing the one from NaoQi is
    // from the kernel and thus very slow.
    unsigned char *image;

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

};

#endif
