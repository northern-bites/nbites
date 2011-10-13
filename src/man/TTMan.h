#ifndef TTMan_h
#define TTMan_h

/**
 *  This file is the double threaded (TT) version of man,
 *  which runs a thread for both the enactor and the image transcriber
 */

#include "Man.h"
#include "ThreadedMotionEnactor.h"
#include "ThreadedImageTranscriber.h"

class TTMan : public Man {
public:
    TTMan(boost::shared_ptr<Profiler> _profiler,
          boost::shared_ptr<Sensors> _sensors,
          boost::shared_ptr<RoboGuardian> guardian,
          boost::shared_ptr<Transcriber> _transcriber,
          boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
          boost::shared_ptr<ThreadedMotionEnactor> _enactor,
          boost::shared_ptr<Lights> _lights,
          boost::shared_ptr<Speech> _speech);
    ~TTMan();

    void startSubThreads();
    void stopSubThreads();

private:
    boost::shared_ptr<ThreadedImageTranscriber> threadedImageTranscriber;
    boost::shared_ptr<ThreadedMotionEnactor> threadedEnactor;
};

#endif
