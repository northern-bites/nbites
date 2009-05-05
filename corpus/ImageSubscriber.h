#ifndef IMAGE_SUBSCRIBER_H
#define IMAGE_SUBSCRIBER_H

class ImageSubscriber {
public:
    virtual void notifyNextVisionImage() = 0;
};

#endif
