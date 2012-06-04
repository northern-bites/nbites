/**
 *
 * @class OfflineTranscriber
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <string>
#include <stdint.h>

#include "corpus/ThreadedImageTranscriber.h"
#include "ClassHelper.h"
#include "memory/MImage.h"
#include "ColorParams.h"

namespace man {
namespace corpus {

class OfflineImageTranscriber: public ThreadedImageTranscriber {

ADD_SHARED_PTR(OfflineImageTranscriber)
    ;

public:
    OfflineImageTranscriber(boost::shared_ptr<Sensors> s,
            memory::MImage::const_ptr mImage);
    virtual ~OfflineImageTranscriber();

    void releaseImage() {
    }
    void run();

    void initTable(const std::string& filename);
    void initTable(byte* other_table) { table = other_table; }


    void acquireNewImage();

public:
    enum {
        y0 = 0, u0 = 0, v0 = 0,
        y1 = 256, u1 = 256, v1 = 256,
        yLimit = 128, uLimit = 128, vLimit = 128,
        tableByteSize = yLimit * uLimit * vLimit
    };

private:
    memory::MImage::const_ptr mImage;
    unsigned char* table;
    ColorParams params;
    uint16_t *image;
};

}
}
