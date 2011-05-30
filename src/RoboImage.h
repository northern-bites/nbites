
#pragma once

#include "colorspace.h"

//TODO: remove this typedef
typedef char byte;

class RoboImage
{

public:
    RoboImage(unsigned int wd = 0, unsigned int ht = 0, const byte* im = NULL) :
    width(wd), height(ht), image(im) {}

    virtual void updateImage(const byte* _image) {
    	image = _image;
    }

    const byte* getImage() const { return image; }

    const unsigned int getWidth() const { return width; }
    const unsigned int getHeight() const { return height; }

    void setWidth(const unsigned w) { width = w; }
    void setHeight(const unsigned h) { height = h; }

protected:
    unsigned int width;
    unsigned int height;
    const byte* image;
};
