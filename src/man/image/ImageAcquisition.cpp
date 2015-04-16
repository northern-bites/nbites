#include "ImageAcquisition.h"
#include "VisionDef.h"

#include <tgmath.h> 
#include <algorithm>

#define CPP_ACQUIRE 0

int ImageAcquisition::acquire_image(int rowCount,           // Row count of each of the output images
                                    int colCount,           // Column count of each of the output images
                                    int rowPitch,           // Number of [y,u,y,v] quadruplets in each input row (usually 320)
                                    const uint8_t *yuv,     // Pointer to first byte of input image
                                    uint8_t *out,           // Pointer to first byte of output images destination
                                    ColorClassificationValues *colors )       // Pointer to first byte of ColorParameters struct
{

#if 0 
    _acquire_image (rowCount, colCount, rowPitch, yuv, out, colors);
#else

    ColorClassificationValues *color = (ColorClassificationValues*) colors;
    uint16_t *yOut      = (uint16_t*)out;
    uint8_t  *whiteOut  = out       + colCount*rowCount * 2;
    uint8_t  *orangeOut = whiteOut  + colCount*rowCount;
    uint8_t  *greenOut  = orangeOut + colCount*rowCount;

    short orangeWidth1 = (short)((255 << 8) / color->orangeWidth); // w1 = 255/w     u16.8
    short greenWidth1 = (short)((255 << 8) / color->greenWidth);
    short whiteWidth1 = (short)((255 << 8) / color->whiteWidth);

    int y;

    for (int i=0; i < rowCount; i ++, yuv += 4 * (2 * rowPitch - colCount)){ 
        for (int j=0; j < colCount; j++, yuv += 4, yOut++, whiteOut++, orangeOut++, greenOut++){

            // Y Averaging
            *yOut = y = yuv[YOFFSET1] + yuv[rowPitch*4 + YOFFSET1] +
                        yuv[YOFFSET2] + yuv[rowPitch*4 + YOFFSET2];
               
            // Variables used for color calcs
            short u0, u = yuv[UOFFSET] + yuv[rowPitch*4 + UOFFSET] - 256;
            short v0, v = yuv[VOFFSET] + yuv[rowPitch*4 + VOFFSET] - 256;
            short f1, f2;

            short absU = u > 0 ? u : -u;
            short absV = v > 0 ? v : -v;

            // WHITE CALCS
            u0 = color->whiteU;

            u0 += (short)((y * color->whiteCoef) >> 16);

            f1 = (std::min(std::max((int)(u0 - absU), 0),
                                    (int)color->whiteWidth) * whiteWidth1) >> 8;

            v0 = color->whiteV;
            v0 += (short)((y * color->whiteCoef) >> 16);

            f2 = (std::min(std::max((int)(v0 - absV), 0),
                                    (int)color->whiteWidth) * whiteWidth1) >> 8;

            *whiteOut = std::min(f1, f2);
                
            // ORANGE CALCS
            u0 = color->orangeMaxU;
            v0 = color->orangeMinV;

            u0 += (short)((y * color->orangeCoefU) >> 16);
            f1 = (std::min(std::max((int)(u0 - u), 0), 
                                    (int)color->orangeWidth) * orangeWidth1) >> 8;

            v0 += (short)((y * color->orangeCoefV) >> 16);
            f2 = (std::min(std::max((int)(v - v0), 0),
                                    (int)color->orangeWidth) * orangeWidth1) >> 8;
            *orangeOut = std::min(f1, f2);

            // GREEN CALCS
            u0 = color->greenMaxU;
            v0 = color->greenMaxV;

            u0 += (short)((y * color->greenCoef) >> 16);
            f1 = (std::min(std::max((int)(u0 - u), 0),
                                    (int)color->greenWidth) * greenWidth1) >> 8; 

            v0 += (short)((y * color->greenCoef) >> 16);
            f2 = (std::min(std::max((int)(v0 - v), 0),
                                    (int)color->greenWidth) * greenWidth1) >> 8;

            *greenOut = std::min(f1, f2);
        }
    }
#endif
    return 0;
}