
#include <stdio.h>
#include <time.h>
#include "alvision/alimage.h"
#include "alvision/alvisiondefinitions.h"
#include "altools/alxplatform.h"

#include "manconfig.h"

#include "ALImageTranscriber.h"
#include "corpusconfig.h"
#include "ImageAcquisition.h"

#ifdef DEBUG_ALIMAGE
#  define DEBUG_ALIMAGE_LOOP
#endif

using boost::shared_ptr;
using namespace AL;
using namespace std;

const int ALImageTranscriber::TOP_CAMERA = 0;
const int ALImageTranscriber::BOTTOM_CAMERA = 1;

// Camera setup information
const int ALImageTranscriber::CAMERA_SLEEP_TIME = 200;
const int ALImageTranscriber::CAM_PARAM_RETRIES = 3;

#define OC_PARAMS
#ifdef OC_PARAMS
const int ALImageTranscriber::DEFAULT_CAMERA_RESOLUTION = 14;
const int ALImageTranscriber::DEFAULT_CAMERA_FRAMERATE = 30;
const int ALImageTranscriber::DEFAULT_CAMERA_BUFFERSIZE = 16;
// Color Settings
// Gain: 26 / Exp: 83
// Gain: 28 / Exp: 60
// Gain: 35 / Exp: 40
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_GAIN = 0; // AUTO GAIN OFF
const int ALImageTranscriber::DEFAULT_CAMERA_GAIN = 30;
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_WHITEBALANCE = 0; // AUTO WB OFF
const int ALImageTranscriber::DEFAULT_CAMERA_BLUECHROMA = 113;
const int ALImageTranscriber::DEFAULT_CAMERA_REDCHROMA = 60;
const int ALImageTranscriber::DEFAULT_CAMERA_BRIGHTNESS = 154;
const int ALImageTranscriber::DEFAULT_CAMERA_CONTRAST = 85;
const int ALImageTranscriber::DEFAULT_CAMERA_SATURATION = 140;
const int ALImageTranscriber::DEFAULT_CAMERA_HUE = 0;
// Lens correction
const int ALImageTranscriber::DEFAULT_CAMERA_LENSX = 0;
const int ALImageTranscriber::DEFAULT_CAMERA_LENSY = 0;
// Exposure length
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_EXPOSITION = 0; // AUTO EXPOSURE OFF
const int ALImageTranscriber::DEFAULT_CAMERA_EXPOSURE = 129;
// Image orientation
const int ALImageTranscriber::DEFAULT_CAMERA_HFLIP = 0;
const int ALImageTranscriber::DEFAULT_CAMERA_VFLIP = 0;
#else
// Default Camera Settings
// Basic Settings
const int ALImageTranscriber::DEFAULT_CAMERA_RESOLUTION = 14;
const int ALImageTranscriber::DEFAULT_CAMERA_FRAMERATE = 30;
const int ALImageTranscriber::DEFAULT_CAMERA_BUFFERSIZE = 16;
// Color Settings
// Gain: 26 / Exp: 83
// Gain: 28 / Exp: 60
// Gain: 35 / Exp: 40
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_GAIN = 0; // AUTO GAIN OFF
const int ALImageTranscriber::DEFAULT_CAMERA_GAIN = 27;
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_WHITEBALANCE = 0; // AUTO WB OFF
const int ALImageTranscriber::DEFAULT_CAMERA_BLUECHROMA = 95;
const int ALImageTranscriber::DEFAULT_CAMERA_REDCHROMA = 74;
const int ALImageTranscriber::DEFAULT_CAMERA_BRIGHTNESS = 128;
const int ALImageTranscriber::DEFAULT_CAMERA_CONTRAST = 69;
const int ALImageTranscriber::DEFAULT_CAMERA_SATURATION = 107;
const int ALImageTranscriber::DEFAULT_CAMERA_HUE = 0;
// Lens correction
const int ALImageTranscriber::DEFAULT_CAMERA_LENSX = 0;
const int ALImageTranscriber::DEFAULT_CAMERA_LENSY = 0;
// Exposure length
const int ALImageTranscriber::DEFAULT_CAMERA_AUTO_EXPOSITION = 0; // AUTO EXPOSURE OFF
const int ALImageTranscriber::DEFAULT_CAMERA_EXPOSURE = 105;
// Image orientation
const int ALImageTranscriber::DEFAULT_CAMERA_HFLIP = 0;
const int ALImageTranscriber::DEFAULT_CAMERA_VFLIP = 0;
#endif

ALImageTranscriber::ALImageTranscriber(shared_ptr<Synchro> synchro,
                                       shared_ptr<Sensors> s,
                                       ALPtr<ALBroker> broker)
    : ThreadedImageTranscriber(s,synchro,"ALImageTranscriber"),
      log(), camera(), lem_name(""), camera_active(false),
      image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
      naoImage(new uint8_t[NAO_IMAGE_BYTE_SIZE]),
      table(new unsigned char[yLimit * uLimit * vLimit]),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit)
{

    try {
        log = broker->getLoggerProxy();
        // Possible values are
        // lowDebug, debug, lowInfo, info, warning, error, fatal
        log->setVerbosity("error");
    }catch (ALError &e) {
        cerr << "Could not create a proxy to ALLogger module" << endl;
    }

#ifdef USE_VISION
    registerCamera(broker);
    if(camera_active) {
        try{
            initCameraSettings(BOTTOM_CAMERA);
        }catch(ALError &e){
            cout << "Failed to init the camera settings:"<<
                e.toString()<< endl;
            camera_active = false;
        }
    } else {
        cout << "\tCamera is inactive!" << endl;
    }
#endif

    initTable("/home/nao/naoqi/lib/naoqi/table.mtb");
}

ALImageTranscriber::~ALImageTranscriber()
{
    delete [] image;
    stop();
}


int ALImageTranscriber::start()
{
    return Thread::start();
}

void ALImageTranscriber::run()
{
    Thread::running = true;
    Thread::trigger->on();

    long long lastProcessTimeAvg = VISION_FRAME_LENGTH_uS;

    struct timespec interval, remainder;
    while (Thread::running) {
        //start timer
        const long long startTime = process_micro_time();

        if (camera_active)
            waitForImage();
        subscriber->notifyNextVisionImage();

#ifdef SAVE_ALL_FRAMES
		if (sensors->isSavingFrames())
		{
			sensors->saveFrame();
		}
#endif

        //stop timer
        const long long processTime = process_micro_time() - startTime;
        //sleep until next frame

        lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

        if (processTime > VISION_FRAME_LENGTH_uS) {
            if (processTime > VISION_FRAME_LENGTH_PRINT_THRESH_uS) {
#ifdef DEBUG_ALIMAGE_LOOP
                cout << "Time spent in ALImageTranscriber loop longer than"
                          << " frame length: " << processTime <<endl;
#endif
            }
            //Don't sleep at all
        } else{
            const long int microSleepTime =
                static_cast<long int>(VISION_FRAME_LENGTH_uS - processTime);
            const long int nanoSleepTime =
                static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

            const long int secSleepTime =
                static_cast<long int>(microSleepTime / (1000*1000));

            // cout << "Sleeping for nano: " << nanoSleepTime
            //      << " and sec:" << secSleepTime << endl;

            interval.tv_sec = static_cast<time_t>(secSleepTime);
            interval.tv_nsec = nanoSleepTime;

            nanosleep(&interval, &remainder);
        }
    }
    Thread::trigger->off();
}

void ALImageTranscriber::stop()
{
    cout << "Stopping ALImageTranscriber" << endl;
    running = false;
#ifdef USE_VISION
    if(camera_active){
        cout << "lem_name = " << lem_name << endl;
        try {
            camera->unsubscribe(lem_name);
        }catch (ALError &e) {
            log->error("Man", "Could not call the unregister method of the NaoCam "
                       "module");
        }
    }
#endif

    Thread::stop();
}

void ALImageTranscriber::registerCamera(ALPtr<ALBroker> broker)
{
    try {
        camera = ALPtr<ALVideoDeviceProxy>(new ALVideoDeviceProxy(broker));
        camera_active =true;
    }catch (ALError &e) {
        log->error("ALImageTranscriber",
                   "Could not create a proxy to NaoCam module");
        camera_active =false;
        return;
    }

    lem_name = "ALImageTranscriber_LEM";
    int format = NAO_IMAGE_SIZE;
    int colorSpace = NAO_COLOR_SPACE;
    int fps = DEFAULT_CAMERA_FRAMERATE;

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  Registering LEM with format=%i colorSpace=%i fps=%i\n", format,
           colorSpace, fps);
#endif

    try {
        lem_name = camera->subscribe(lem_name, format,
                                     colorSpace, fps);
        std::cout << "Registered Camera: " << lem_name << " successfully"<<std::endl;
    } catch (ALError &e) {
        cout << "Failed to register camera" << lem_name << endl;
        camera_active = false;
    }

}

void ALImageTranscriber::initCameraSettings(int whichCam)
{

    int currentCam =  camera->getParam(kCameraSelectID);

    if (whichCam != currentCam){
        camera->setParam(kCameraSelectID,whichCam);
        SleepMs(CAMERA_SLEEP_TIME);
        currentCam =  camera->getParam(kCameraSelectID);

        if (whichCam != currentCam){
            cout << "Failed to switch to camera "<<whichCam
                      <<" retry in " << CAMERA_SLEEP_TIME <<" ms" <<endl;
            SleepMs(CAMERA_SLEEP_TIME);
            currentCam =  camera->getParam(kCameraSelectID);
            if (whichCam != currentCam){
                cout << "Failed to switch to camera "<<whichCam
                          <<" ... returning, no parameters initialized" <<endl;
                return;
            }
        }
        cout << "Switched to camera " << whichCam <<" successfully"<<endl;
    }

    // Turn off auto settings
    // Auto exposure
    try {
        camera->setParam(kCameraAutoExpositionID,
                         DEFAULT_CAMERA_AUTO_EXPOSITION);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set AutoExposition");
    }
    int param = camera->getParam(kCameraAutoExpositionID);
    // if that didn't work, then try again
    if (param != DEFAULT_CAMERA_AUTO_EXPOSITION) {
        try {
            camera->setParam(kCameraAutoExpositionID,
                             DEFAULT_CAMERA_AUTO_EXPOSITION);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set AutoExposition AGAIN");
        }
    }
    // Auto white balance
    try {
        camera->setParam(kCameraAutoWhiteBalanceID,
                         DEFAULT_CAMERA_AUTO_WHITEBALANCE);

    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set AutoWhiteBalance");
    }
    param = camera->getParam(kCameraAutoWhiteBalanceID);
    if (param != DEFAULT_CAMERA_AUTO_WHITEBALANCE) {
        try {
            camera->setParam(kCameraAutoWhiteBalanceID,
                             DEFAULT_CAMERA_AUTO_WHITEBALANCE);
        } catch (ALError &e){
            log->error("ALImageTranscriber","Couldn't set AutoWhiteBalance AGAIN");
        }
    }
    // Auto gain
    try {
        camera->setParam(kCameraAutoGainID,
                         DEFAULT_CAMERA_AUTO_GAIN);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set AutoGain");
    }
    param = camera->getParam(kCameraAutoGainID);
    if (param != DEFAULT_CAMERA_AUTO_GAIN) {
        try {
            camera->setParam(kCameraAutoGainID,
                             DEFAULT_CAMERA_AUTO_GAIN);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set AutoGain AGAIN");
        }
    }
    // Set camera defaults
    // brightness
    try {
        camera->setParam(kCameraBrightnessID,
                         DEFAULT_CAMERA_BRIGHTNESS);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set Brightness ");
    }
    param = camera->getParam(kCameraBrightnessID);
    if (param != DEFAULT_CAMERA_BRIGHTNESS) {
        try {
            camera->setParam(kCameraBrightnessID,
                             DEFAULT_CAMERA_BRIGHTNESS);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set BRIGHTNESS AGAIN");
        }
    }
    // contrast
    try {
        camera->setParam(kCameraContrastID,
                         DEFAULT_CAMERA_CONTRAST);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set Contrast");
    }
    param = camera->getParam(kCameraContrastID);
    if (param != DEFAULT_CAMERA_CONTRAST) {
        try {
            camera->setParam(kCameraContrastID,
                             DEFAULT_CAMERA_CONTRAST);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Contrast AGAIN");
        }
    }
    // Red chroma
    try {
        camera->setParam(kCameraRedChromaID,
                         DEFAULT_CAMERA_REDCHROMA);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set RedChroma");
    }
    param = camera->getParam(kCameraRedChromaID);
    if (param != DEFAULT_CAMERA_REDCHROMA) {
        try {
            camera->setParam(kCameraRedChromaID,
                             DEFAULT_CAMERA_REDCHROMA);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set RedChroma AGAIN");
        }
    }
    // Blue chroma
    try {
        camera->setParam(kCameraBlueChromaID,
                         DEFAULT_CAMERA_BLUECHROMA);
    } catch (ALError &e){
        log->error("ALImageTranscriber", "Couldn't set BlueChroma");
    }
    param = camera->getParam(kCameraBlueChromaID);
    if (param != DEFAULT_CAMERA_BLUECHROMA) {
        try {
            camera->setParam(kCameraBlueChromaID,
                             DEFAULT_CAMERA_BLUECHROMA);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set BlueChroma AGAIN");
        }
    }
    // Exposure length
    try {
        camera->setParam(kCameraExposureID,
                         DEFAULT_CAMERA_EXPOSURE);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Exposure");
    }
    param = camera->getParam(kCameraExposureID);
    if (param != DEFAULT_CAMERA_EXPOSURE) {
        try {
            camera->setParam(kCameraExposureID,
                             DEFAULT_CAMERA_EXPOSURE);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Exposure AGAIN");
        }
    }
    // Gain
    try {
        camera->setParam(kCameraGainID,
                         DEFAULT_CAMERA_GAIN);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Gain");
    }
    param = camera->getParam(kCameraGainID);
    if (param != DEFAULT_CAMERA_GAIN) {
        try {
            camera->setParam(kCameraGainID,
                             DEFAULT_CAMERA_GAIN);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Gain AGAIN");
        }
    }
    // Saturation
    try {
        camera->setParam(kCameraSaturationID,
                         DEFAULT_CAMERA_SATURATION);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Saturation");
    }
    param = camera->getParam(kCameraSaturationID);
    if (param != DEFAULT_CAMERA_SATURATION) {
        try {
            camera->setParam(kCameraSaturationID,
                             DEFAULT_CAMERA_SATURATION);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Saturation AGAIN");
        }
    }
    // Hue
    try {
        camera->setParam(kCameraHueID,
                         DEFAULT_CAMERA_HUE);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Hue");
    }
param = camera->getParam(kCameraHueID);
    if (param != DEFAULT_CAMERA_HUE) {
        try {
            camera->setParam(kCameraHueID,
                             DEFAULT_CAMERA_HUE);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Hue AGAIN");
        }
    }
    // Lens correction X
    try {
        camera->setParam(kCameraLensXID,
                         DEFAULT_CAMERA_LENSX);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Lens Correction X");
    }
    param = camera->getParam(kCameraLensXID);
    if (param != DEFAULT_CAMERA_LENSX) {
        try {
            camera->setParam(kCameraLensXID,
                             DEFAULT_CAMERA_LENSX);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Lens Correction X AGAIN");
        }
    }
    // Lens correction Y
    try {
        camera->setParam(kCameraLensYID,
                         DEFAULT_CAMERA_LENSY);
    } catch (ALError &e) {
        log->error("ALImageTranscriber", "Couldn't set Lens Correction Y");
    }
    param = camera->getParam(kCameraLensYID);
    if (param != DEFAULT_CAMERA_LENSY) {
        try {
            camera->setParam(kCameraLensYID,
                             DEFAULT_CAMERA_LENSY);
        } catch (ALError &e){
            log->error("ALImageTranscriber", "Couldn't set Lens Correction Y AGAIN");
        }
    }
}

void ALImageTranscriber::initTable(string filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("initTable() FAILED to open filename: %s", filename.c_str());
#ifdef OFFLINE
        exit(0);
#else
        return;
#endif
    }

    // actually read the table into memory
    // Color table is in VUY ordering
    int rval;
    for(int v=0; v < vLimit; ++v){
        for(int u=0; u< uLimit; ++u){
            rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                         sizeof(unsigned char), yLimit, fp);
        }
    }

#ifndef OFFLINE
    printf("Loaded colortable %s\n",filename.c_str());
#endif

    fclose(fp);
}

void ALImageTranscriber::initTable(unsigned char* buffer)
{
    memcpy(table, buffer, yLimit * uLimit * vLimit);
}


void ALImageTranscriber::waitForImage ()
{
    try {
#ifndef MAN_IS_REMOTE
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting local image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif
        ALImage *ALimage = NULL;

        // Attempt to retrieve the next image
        try {
            ALimage =
                reinterpret_cast<ALImage*>(
                    camera->getDirectRawImageLocal( lem_name));
        }catch (ALError &e) {
            log->error("NaoMain",
                       "Could not call the getImageLocal method of the "
                       "NaoCam module");
        }



        if (ALimage != NULL){
            // Synchronize noggin's information about joint angles with the motion
            // thread's information
            //we need to update the joint values before we copy the image
            //since it takes a long time to copy it
            sensors->updateVisionAngles();

            sensors->lockImage();
#ifdef CAN_SAVE_FRAMES
#ifdef USE_MEMORY
            sensors->setRawNaoImage(ALimage->getData());
            ImageAcquisition::_acquire_image_fast(table, &params, const_cast<uint8_t*>(sensors->getNaoImage()), image);
#else
            _copy_image(ALimage->getData(), naoImage);
            ImageAcquisition::acquire_image_fast(table, params,
                    	naoImage, image);
#endif
#else
            ImageAcquisition::acquire_image_fast(table, params,
                                                 ALimage->getData(), image);
#endif
            sensors->releaseImage();
            //we're done with the aldebaran buffer
            this->releaseImage();

        } else {
            cout << "\tALImage from camera was null!!" << endl;
        }

#ifdef DEBUG_IMAGE_REQUESTS
        //You can get some informations of the image.
        int width = ALimage->fWidth;
        int height = ALimage->fHeight;
        int nbLayers = ALimage->fNbLayers;
        int colorSpace = ALimage->fColorSpace;
        long long timeStamp = ALimage->fTimeStamp;
        int seconds = (int)(timeStamp/1000000LL);
        printf("Retrieved an image of dimensions %ix%i, color space %i,"
               "with %i layers and a time stamp of %is \n",
               width, height, colorSpace,nbLayers,seconds);
#endif

#else//Frame is remote:
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting remote image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif
        ALValue ALimage;
        ALimage.arraySetSize(7);

        // Attempt to retrive the next image
        try {
            ALimage = camera->getDirectRawImageRemote(lem_name);
        }catch (ALError &e) {
            log->error("NaoMain",
                       "Could not call the getImageRemote method of the "
                       "NaoCam module");
        }

        //image = static_cast<const unsigned char*>(ALimage[6].GetBinary());
        memcpy(&image[0], ALimage[6].GetBinary(), IMAGE_BYTE_SIZE);
#ifdef DEBUG_IMAGE_REQUESTS
        //You can get some informations of the image.
        int width = (int) ALimage[0];
        int height = (int) ALimage[1];
        int nbLayers = (int) ALimage[2];
        int colorSpace = (int) ALimage[3];
        long long timeStamp = ((long long)(int)ALimage[4])*1000000LL +
            ((long long)(int)ALimage[5]);
        int seconds = (int)(timeStamp/1000000LL);
        printf("Retrieved an image of dimensions %ix%i, color space %i,"
               "with %i layers and a time stamp of %is \n",
               width, height, colorSpace,nbLayers,seconds);
#endif

#endif//IS_REMOTE

        if (image != NULL) {
            // Update Sensors image pointer
            sensors->lockImage();
            sensors->setImage(image);
#ifdef CAN_SAVE_FRAMES
            sensors->setNaoImage(naoImage);
#endif
            sensors->releaseImage();
        }

    }catch (ALError &e) {
        log->error("NaoMain", "Caught an error in run():\n" + e.toString());
    }
}


void ALImageTranscriber::releaseImage(){
#ifndef MAN_IS_REMOTE
    if (!camera_active)
        return;

    //Now you have finished with the image, you have to release it in the V.I.M.
    try
        {
            camera-> releaseDirectRawImage( lem_name );
        }catch( ALError& e)
        {
            log->error( "ALImageTranscriber",
                        "could not call the releaseImage method of the NaoCam module" );
        }
#endif
}
