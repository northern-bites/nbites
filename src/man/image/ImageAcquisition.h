#ifndef _IMAGE_ACQUISITION_H_
#define _IMAGE_ACQUISITION_H_

#include <stdint.h>
#include <iostream>

class ColorParams;

extern "C" int _acquire_image(int rowCount,
							  int colCount,
							  int rowPitch,
							  const uint8_t *yuv,
							  uint8_t *out );

extern "C" int _copy_image(const uint8_t *camera, uint8_t *local);

namespace ImageAcquisition {
    int acquire_image(int rowCount, int colCount,int rowPitch,
							const uint8_t *yuv, uint8_t *out );
};

#endif
