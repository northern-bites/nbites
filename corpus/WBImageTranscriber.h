#ifndef WBImageTranscriber_h
#define WBImageTranscriber_h

#include "ThreadedImageTranscriber.h"

class WBImageTranscriber : public ThreadedImageTranscriber{
public:
    WBImageTranscriber(boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<Synchro> synchro);
    ~WBImageTranscriber();

    void releaseImage();
    void run();

private:
    void waitForImage();
};

#endif
