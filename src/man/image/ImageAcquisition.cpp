#include "ImageAcquisition.h"
#include "VisionDef.h"

/**
 * The same as _acquire_image_fast, but in C++ for the sake of
 * portability. I blame Mac OS X and its non GNU assembler. --Jack
 */
int ImageAcquisition::acquire_image_fast(uint8_t *table,
                                         const ColorParams &params,
                                         const uint8_t *yuv, uint16_t *out )
{
#ifdef __linux__
    _acquire_image_fast(table, const_cast<ColorParams*>(&params), yuv, out);
#else //TODO: I don't think this accurately reflects the ASM image acquisition

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
            uint16_t yAvg = *(yOut) =
                static_cast<uint16_t>(static_cast<uint16_t>(*(yuv+YOFFSET1)) +
                                      static_cast<uint16_t>(*(yuv+YOFFSET2)));
            uint16_t uAvg = *(uvOut) = static_cast<uint16_t>(*(yuv+UOFFSET));
            uint16_t vAvg = *(uvOut+1) = static_cast<uint16_t>(*(yuv+VOFFSET));

            // HACK. THIS ONLY WORKS FOR 0-256, 128 byte color tables.
            // I hope that we have moved to a more sensible
            // (i.e. single) dev platform before we need to change this part

            // Table offset (table is in VUY order)

            // *2 is to remove lowest bit, so it is bit compatible with
            //     ASM version.
            int offset = 128*128*(vAvg>>2)*2 + 128*(uAvg>>2)*2 + (yAvg>>2);
            *color = *(table + offset);
        }
    }
#endif
    return 0;
}
