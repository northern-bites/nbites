#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include "VisionDef.h"

namespace messages {

class ThresholdedImage {
public:
    ThresholdedImage() : image(reinterpret_cast<uint16_t*>
                               (new uint8_t[IMAGE_BYTE_SIZE])) {}

    ~ThresholdedImage() { delete image; }

    // These methods make this class look like a protobuf
    uint16_t* get_mutable_image() { return image; }
    uint16_t* get_image() const { return image; }
    long long get_timestamp() const { return timestamp; }
    void set_timestamp(long long t) { timestamp = t; }
    void Clear() { timestamp = 0; }
    std::string DebugString() const
    {
        std::stringstream debug;
        debug << "Image from time " << timestamp;
        return debug.str();
    }

//private:
    uint16_t* image;
    long long timestamp;
};

}
