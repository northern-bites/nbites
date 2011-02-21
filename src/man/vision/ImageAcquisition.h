#include <stdint.h>

class ColorParams;
extern "C" int _acquire_image(unsigned char *table, ColorParams *params,
                              unsigned char *yuv, uint16_t *out );

extern "C" int _copy_image(unsigned char *camera, unsigned char *local);

