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
    static const int TOP_CAMERA = 0;
    static const int BOTTOM_CAMERA = 1;

    // Camera setup information
    static const int CAMERA_SLEEP_TIME = 200;
    static const int CAM_PARAM_RETRIES = 3;

    // Default Camera Settings
    // Basic Settings
    static const int DEFAULT_CAMERA_RESOLUTION = 14;
    static const int DEFAULT_CAMERA_FRAMERATE = 30;
    static const int DEFAULT_CAMERA_BUFFERSIZE = 16;
    // Color Settings
    // Gain: 26 / Exp: 83
    // Gain: 28 / Exp: 60
    // Gain: 35 / Exp: 40
    static const int DEFAULT_CAMERA_AUTO_GAIN = 0; // AUTO GAIN OFF
    static const int DEFAULT_CAMERA_GAIN = 26;
    static const int DEFAULT_CAMERA_AUTO_WHITEBALANCE = 0; // AUTO WB OFF
    static const int DEFAULT_CAMERA_BLUECHROMA = 128;
    static const int DEFAULT_CAMERA_REDCHROMA = 68;
    static const int DEFAULT_CAMERA_BRIGHTNESS = 140;
    static const int DEFAULT_CAMERA_CONTRAST = 64;
    static const int DEFAULT_CAMERA_SATURATION = 128;
    static const int DEFAULT_CAMERA_HUE = 0;
    // Lens correction
    static const int DEFAULT_CAMERA_LENSX = 0;
    static const int DEFAULT_CAMERA_LENSY = 0;
    // Exposure length
    static const int DEFAULT_CAMERA_AUTO_EXPOSITION = 0; // AUTO EXPOSURE OFF
    static const int DEFAULT_CAMERA_EXPOSURE = 83;
    // Image orientation
    static const int DEFAULT_CAMERA_HFLIP = 0;
    static const int DEFAULT_CAMERA_VFLIP = 0;

};

#endif
