#include "ImageAcquisition.h"
#include "VisionDef.h"
#include "ColorParams.h"

/**
 * The same as _acquire_image_fast, but in C++ for the sake of
 * portability. I blame Mac OS X and its non GNU assembler. --Jack
 */
#define CPP_ACQUIRE 1
int ImageAcquisition::acquire_image_fast(uint8_t *table,
                                         const ColorParams &params,
                                         const uint8_t *yuv, uint16_t *out )
{
#if defined( __linux__) && !CPP_ACQUIRE
   _acquire_image_fast(table, const_cast<ColorParams*>(&params), yuv, out);
#else
    // This should be bit-identical to the ASM code
    uint16_t *yOut = out;
    uint16_t *uvOut = out + AVERAGED_IMAGE_SIZE;
    uint8_t *color = reinterpret_cast<uint8_t*>(out + AVERAGED_IMAGE_SIZE*3);

    // Y Averaging
    for (int i=0; i < AVERAGED_IMAGE_HEIGHT; ++i,
             yuv += NAO_IMAGE_ROW_OFFSET){ // Skip every other row in the src

        // 4 pixels per pixel (Y U Y V) combo
        for (int j = 0; j < AVERAGED_IMAGE_WIDTH;
             ++j, yuv+=4, yOut++, uvOut+=2, color++) {

            // Sum every 2 Y pixels and write back out
#if 0
            // full average version, not currently used
            uint16_t yAvg = *(yOut) =
                yuv[YOFFSET1] + yuv[NAO_IMAGE_ROW_OFFSET + YOFFSET1] +
                yuv[YOFFSET2] + yuv[NAO_IMAGE_ROW_OFFSET + YOFFSET2];
            uint16_t uAvg = *(uvOut    ) = yuv[UOFFSET] + yuv[NAO_IMAGE_ROW_OFFSET + UOFFSET];
            uint16_t vAvg = *(uvOut + 1) = yuv[VOFFSET] + yuv[NAO_IMAGE_ROW_OFFSET + VOFFSET];
#else
            // half average version (faster)
            uint16_t yAvg = *(yOut) = yuv[YOFFSET1] + yuv[YOFFSET2];
            uint16_t uAvg = *(uvOut    ) = yuv[UOFFSET];
            uint16_t vAvg = *(uvOut + 1) = yuv[VOFFSET];
#endif
           // Table offset (table is in VUY order)

            int offset = params.offset(params.yIndex(yAvg), params.uIndex(uAvg), params.vIndex(vAvg));
            *color = table[offset];
        }
    }
#endif
    return 0;
}
