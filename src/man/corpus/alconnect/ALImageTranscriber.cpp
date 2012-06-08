
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "alvision/alimage.h"
#include "alvision/alvisiondefinitions.h"
#include "altools/alxplatform.h"

#include "manconfig.h"

#include "ALImageTranscriber.h"
#include "corpusconfig.h"
#include "ImageAcquisition.h"

#include "Profiler.h"

#include "Camera.h"

#ifdef DEBUG_ALIMAGE
#  define DEBUG_ALIMAGE_LOOP
#endif

//TODO: make alimagetranscriber part of man::corpus
using namespace man::corpus;

using boost::shared_ptr;
using namespace AL;
using namespace std;

ALImageTranscriber::ALImageTranscriber(shared_ptr<Sensors> s,
                                       shared_ptr<ALBroker> broker)
    : ThreadedImageTranscriber(s,"ALImageTranscriber"),
      log(), camera(), lem_name(""), camera_active(false),
      image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
      table(new unsigned char[yLimit * uLimit * vLimit]),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit)
{

    try {
        log = broker->getLoggerProxy();
        // Possible values are
        // lowDebug, debug, lowInfo, info, warning, error, fatal
        log->setVerbosity("debug");
    }catch (ALError &e) {
        cout << "Could not create a proxy to ALLogger module" << endl;
    }

#ifdef USE_VISION
    registerCamera(broker);
    if(camera_active) {
        try{
            initCameraSettings(Camera::BOTTOM);
        }catch(ALError &e){
            cout << "Failed to init the camera settings:"<<
                e.toString()<< endl;
            camera_active = false;
        }
    } else {
        cout << "\tCamera is inactive!" << endl;
    }
#endif

    initTable("/home/nao/nbites/lib/table/table.mtb");
}

ALImageTranscriber::~ALImageTranscriber()
{
    cout << "ALImageTranscriber destructor" << endl;
    delete [] image;
}


int ALImageTranscriber::start()
{
    return Thread::start();
}

void ALImageTranscriber::run()
{
    long long lastProcessTimeAvg = VISION_FRAME_LENGTH_uS;

    struct timespec interval, remainder;
    while (Thread::running) {
        //start timer
        PROF_ENTER(P_MAIN);
        PROF_ENTER(P_GETIMAGE);
        const long long startTime = monotonic_micro_time();

        if (camera_active)
            waitForImage();
        PROF_EXIT(P_GETIMAGE);
        subscriber->notifyNextVisionImage();

#ifdef SAVE_ALL_FRAMES
		if (sensors->isSavingFrames())
		{
			sensors->saveFrame();
		}
#endif

        //stop timer
        const long long processTime = monotonic_micro_time() - startTime;
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
            //TODO @refactor
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
        PROF_EXIT(P_MAIN);
        PROF_NFRAME();
    }
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

void ALImageTranscriber::registerCamera(boost::shared_ptr<ALBroker> broker)
{
    try {
        camera = boost::shared_ptr<ALVideoDeviceProxy>
            (new ALVideoDeviceProxy(broker));
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
    int fps = Camera::DEFAULT_FRAMERATE;

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

void ALImageTranscriber::setCameraParameter(int paramId, int param) {

    if (param == Camera::KEEP_DEFAULT)
        return ;

    try {
        camera->setParam(paramId, param);
    } catch (ALError &e){
        cerr << "ALImageTranscriber - Error setting parameter with id "
             << paramId << " to value " << param << "with error \n"
             << e.toString() << endl;
    }
    if (camera->getParam(paramId) != param) {
        cout << "Warning: didn't successfully set the parameter with id "
             << paramId << endl;
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

    Camera::Settings settings = Camera::getDefaultSettings();

    setCameraParameter(kCameraAutoExpositionID, settings.auto_exposition);
    setCameraParameter(kCameraAutoWhiteBalanceID, settings.auto_whitebalance);
    setCameraParameter(kCameraAutoGainID, settings.auto_gain);

    setCameraParameter(kCameraBrightnessID, settings.brightness);
    setCameraParameter(kCameraContrastID, settings.contrast);
    setCameraParameter(kCameraRedChromaID, settings.red_chroma);
    setCameraParameter(kCameraBlueChromaID, settings.blue_chroma);
    setCameraParameter(kCameraExposureID, settings.exposure);
    setCameraParameter(kCameraGainID, settings.gain);
    setCameraParameter(kCameraSaturationID, settings.saturation);
    setCameraParameter(kCameraHueID, settings.hue);
}

void ALImageTranscriber::initTable(const string& filename)
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
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting local image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif
        ALImage *ALimage = NULL;

        // Attempt to retrieve the next image
        try {
            ALimage = reinterpret_cast<ALImage*>(camera->getDirectRawImageLocal(lem_name));
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
            // we make a local of the nao image for logging purposes
            if (sensors->getWriteableNaoImage() != NULL) {
                _copy_image(ALimage->getData(), sensors->getWriteableNaoImage());
                ImageAcquisition::acquire_image_fast(table, params,
                                                     sensors->getNaoImage(), image);
            } else {
                ImageAcquisition::acquire_image_fast(table, params,
                                                     ALimage->getData(), image);

            }
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
        int width = ALimage->getWidth();
        int height = ALimage->getHeight();
        int nbLayers = ALimage->getNbLayers();
        int colorSpace = ALimage->getColorSpace();
        long long timeStamp = ALimage->getTimeStamp();
        int seconds = (int)(timeStamp/1000000LL);
        printf("Retrieved an image of dimensions %ix%i, color space %i,"
               "with %i layers and a time stamp of %is \n",
               width, height, colorSpace,nbLayers,seconds);
#endif

        if (image != NULL) {
            // Update Sensors image pointer
            sensors->lockImage();
            sensors->setImage(image);
#ifdef CAN_SAVE_FRAMES
            sensors->notifyNewNaoImage();
#endif
            sensors->releaseImage();
        }

    }catch (ALError &e) {
        log->error("NaoMain", "Caught an error in run():\n" + e.toString());
    }
}


void ALImageTranscriber::releaseImage(){
    if (!camera_active)
        return;

    //Now you have finished with the image, you have to release it in the V.I.M.
    try {
        camera-> releaseDirectRawImage( lem_name );
    } catch( ALError& e) {
        log->error( "ALImageTranscriber",
                    "could not call the releaseImage method of the NaoCam module" );
    }
}
