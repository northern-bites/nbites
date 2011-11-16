#ifndef IMAGE_TRANSCRIBER_H
#define IMAGE_TRANSCRIBER_H

#include <boost/shared_ptr.hpp>
#include <string>

#include "Sensors.h"
#include "ImageSubscriber.h"

//TODO: @oneamtu make this a subclass of Provider and ImageSubscriber a
// subclass of Subscriber
class ImageTranscriber {
 public:
 ImageTranscriber(boost::shared_ptr<Sensors> s)
     : sensors(s) { }
    virtual ~ImageTranscriber() { }

    virtual void setSubscriber(ImageSubscriber *_subscriber) {
        subscriber = _subscriber;
    }

    virtual void releaseImage() = 0;

    virtual void initTable(const std::string& path) = 0;

 protected:
    boost::shared_ptr<Sensors> sensors;
    ImageSubscriber *subscriber;
};

#endif
