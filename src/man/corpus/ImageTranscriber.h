#ifndef IMAGE_TRANSCRIBER_H
#define IMAGE_TRANSCRIBER_H

#include <boost/shared_ptr.hpp>
#include <string>

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
    virtual void initTable(const std::string& path) = 0;

    //a bit hackish, but we need this to update the vision body angles
    Sensors::ptr getSensors() const { return sensors; }

 protected:
    boost::shared_ptr<Sensors> sensors;
    ImageSubscriber *subscriber;
};

#endif
