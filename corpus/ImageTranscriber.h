#ifndef IMAGE_TRANSCRIBER_H
#define IMAGE_TRANSCRIBER_H

#include <boost/shared_ptr.hpp>

#include "Sensors.h"
#include "ImageSubscriber.h"

class ImageTranscriber {
public:
    ImageTranscriber(boost::shared_ptr<Sensors> s)
        : sensors(s) { }
    virtual ~ImageTranscriber() { }

    virtual void setSubscriber(ImageSubscriber *_subscriber) {
        subscriber = _subscriber;
    }

    virtual void releaseImage() = 0;

protected:
    boost::shared_ptr<Sensors> sensors;
    //void(ImageSubscriber::*imageCallback)();
    ImageSubscriber *subscriber;
};

#endif
