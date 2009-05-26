#ifndef WBImageTranscriber_h
#define WBImageTranscriber_h

#include "ThreadedImageTranscriber.h"

class WBImageTranscriber : public ImageTranscriber{
public:
    WBImageTranscriber(boost::shared_ptr<Sensors> s);
    ~WBImageTranscriber();

    void releaseImage();

private:
    void waitForImage();
};

#endif
