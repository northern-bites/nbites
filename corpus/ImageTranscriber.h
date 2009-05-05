#ifndef IMAGE_TRANSCRIBER_H
#define IMAGE_TRANSCRIBER_H

#include <boost/shared_ptr.hpp>

#include "ImageSubscriber.h"

class ImageTranscriber {
public:
    ImageTranscriber(boost::shared_ptr<Sensors> s)
        : sensors(s) { }
    virtual ~ImageTranscriber();

    virtual void setNewImageCallback(void (ImageSubscriber::*_imageCallback)()){
        imageCallback = _imageCallback;
    }

private:
    boost::shared_ptr<Sensors> sensors;
    void(ImageSubscriber::*imageCallback)();
};

#endif
