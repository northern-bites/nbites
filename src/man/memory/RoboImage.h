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

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include "include/VisionDef.h"

// TODO: move this to sensors; this should be the standard
// way of passing the image around

namespace man {
namespace memory {

class RoboImage {

public:
	typedef boost::shared_ptr<RoboImage> ptr;
	typedef boost::shared_ptr<const RoboImage> const_ptr;

public:
    RoboImage(unsigned int wd = NAO_IMAGE_WIDTH,
    		unsigned int ht = NAO_IMAGE_HEIGHT,
    		const byte* im = NULL,
    		int32_t byte_size = NAO_IMAGE_BYTE_SIZE) :
    width(wd), height(ht), image(im), byte_size(byte_size), timestamp(0) {}

    virtual ~RoboImage(){}

    virtual void updateImagePointer(const byte* _image) {
    	image = _image;
    }

    const byte* getImage() const { return image; }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }

    unsigned int getByteSize() const { return byte_size; }
    int64_t getTimestamp() const { return timestamp; }

    void setWidth(const unsigned w) { width = w; }
    void setHeight(const unsigned h) { height = h; }
    void setByteSize(uint32_t bs) { byte_size = bs; }
    void setTimestamp(int64_t ts) { timestamp = ts; }

protected:
    unsigned int width;
    unsigned int height;
    const uint8_t* image;
    int32_t byte_size;
    int64_t timestamp;
};

}
}
