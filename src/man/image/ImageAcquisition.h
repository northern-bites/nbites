#ifndef _IMAGE_ACQUISITION_H_
#define _IMAGE_ACQUISITION_H_

#include <stdint.h>
#include <iostream>

struct Colors;

extern "C" int _acquire_image(   const unsigned char* source,
                                int width, int height, int pitch,
                                const Colors* colors,
                                unsigned char* dest,
                                unsigned char* colorTable = 0);

extern "C" int _copy_image(const uint8_t *camera, uint8_t *local);

namespace ImageAcquisition {
    int acquire_image(  const unsigned char* source,
                        int width, int height, int pitch,
                        const Colors* colors,
                        unsigned char* dest,
                        unsigned char* colorTable = 0);
};

#endif
