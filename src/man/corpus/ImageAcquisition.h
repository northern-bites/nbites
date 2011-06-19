#include <stdint.h>

class ColorParams;
extern "C" int _acquire_image(uint8_t *table, ColorParams *params,
                              uint8_t *yuv, uint16_t *out );

extern "C" int _copy_image(uint8_t *camera, uint8_t *local);

extern "C" int _acquire_image_fast(uint8_t *table, ColorParams *params,
                                   uint8_t *yuv, uint16_t *out );

namespace ImageAcquisition {
    int acquire_image_fast(uint8_t *table, const ColorParams &params,
                           uint8_t *yuv, uint16_t *out );

};
