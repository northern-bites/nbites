/**
 *
 * @class RoboImage
 *
 * stores a pointer to the byte array representing the robot image
 * and other various useful information
 *
 * @author Octavian Neamtu
 * @e-mail oneamtu89@gmail.com
 *
 */
#pragma once

#include "include/VisionDef.h"

class RoboImage
{

public:
    RoboImage(unsigned int wd = NAO_IMAGE_WIDTH,
    		unsigned int ht = NAO_IMAGE_HEIGHT,
    		const byte* im = NULL) :
    width(wd), height(ht), image(im) {}

    virtual void updateImage(const byte* _image) {
    	image = _image;
    }

    const byte* getImage() const { return image; }

    const unsigned int getWidth() const { return width; }
    const unsigned int getHeight() const { return height; }

    const unsigned int getByteSize() const { return NAO_IMAGE_BYTE_SIZE; }

    void setWidth(const unsigned w) { width = w; }
    void setHeight(const unsigned h) { height = h; }

protected:
    unsigned int width;
    unsigned int height;
    const byte* image;
};
