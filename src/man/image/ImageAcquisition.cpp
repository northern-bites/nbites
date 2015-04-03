#include "ImageAcquisition.h"
#include "VisionDef.h"

#include <tgmath.h> 
#include <algorithm>

#define CPP_ACQUIRE 0

struct ColorClassificationValues {
    short whiteWidth;
    short whiteU;
    short whiteV;
    short whiteMaxCoef;

    short orangeWidth;
    short orangeMaxU;
    short orangeMinV;
    short orangeCoefV;
    short orangeCoefU;

    short greenWidth;
    short greenMaxU;
    short greenMaxV;
    short greenCoef;
};

int ImageAcquisition::acquire_image(int rowCount,
                                    int colCount,
                                    int rowPitch,
                                    const uint8_t *yuv,
                                    uint8_t *out )
{
    ColorClassificationValues* colors = new ColorClassificationValues(); 

    colors->whiteWidth = 20;
    colors->whiteU = -30;
    colors->whiteV = -30;    
    colors->whiteMaxCoef = 6552;  // .2 << 15

    colors->orangeWidth = 30;
    colors->orangeMaxU = 12;
    colors->orangeMinV = 12;
    colors->orangeCoefU = -4915;  // -.15 << 15
    colors->orangeCoefV = 4915;

    colors->greenWidth = 30;
    colors->greenMaxU = -6;
    colors->greenMaxV = -6;
    colors->greenCoef = -3932; // -.12 << 15

#if 0 
    _acquire_image (rowCount, colCount, rowPitch, yuv, out, colors);
#else

    uint16_t *yOut      = (uint16_t*)out;
    uint8_t  *whiteOut  = out       + colCount*rowCount * 2;
    uint8_t  *orangeOut = whiteOut  + colCount*rowCount;
    uint8_t  *greenOut  = orangeOut + colCount*rowCount;

    short orangeWidth1 = (short)((255 << 8) / colors->orangeWidth); // w1 = 255/w     u16.8
    short greenWidth1 = (short)((255 << 8) / colors->greenWidth);
    short whiteWidth1 = (short)((255 << 8) / colors->whiteWidth);

    int y;

    for (int i=0; i < rowCount; i ++, yuv += colCount*4){       // rowPitch? colCount?
        for (int j=0; j < colCount; j++, yuv += 4, yOut++, whiteOut++, orangeOut++, greenOut++){

            // Y Averaging
            *yOut = y = yuv[YOFFSET1] + yuv[rowPitch*4 + YOFFSET1] +
                    yuv[YOFFSET2] + yuv[rowPitch*4 + YOFFSET2];
               
            // Variables used for color calcs
            y >>= 2;
            short u0, u = ((yuv[UOFFSET] + yuv[rowPitch*4 + UOFFSET]) >> 1) - 128;
            short v0, v = ((yuv[VOFFSET] + yuv[rowPitch*4 + VOFFSET]) >> 1) - 128;
            short f1, f2;

            short absU = u > 0 ? u : -u;
            short absV = v > 0 ? v : -v;

            // WHITE CALCS
            u0 = colors->whiteU;
            u0 += (short)((y * colors->whiteMaxCoef) >> 15);
            std::cout << "new u: " << u0 << std::endl;
            f1 = (std::min(std::max((int)(u0 + colors->whiteWidth - absU), 0),
                                    (int)colors->whiteWidth) * whiteWidth1) >> 8;
           
            v0 = colors->whiteV;
            v0 += (short)((y * colors->whiteMaxCoef) >> 15);
            std::cout << "new v: " << v0 << std::endl;
            f2 = (std::min(std::max((int)(v0 + colors->whiteWidth - absV), 0),
                                    (int)colors->whiteWidth) * whiteWidth1) >> 8;

            *whiteOut = std::min(f1, f2);
                
            // ORANGE CALCS
            u0 = colors->orangeMaxU;
            v0 = colors->orangeMinV;

            u0 += (short)((y * colors->orangeCoefU) >> 15);
            f2 = (std::min(std::max((int)(u0 + colors->orangeWidth - u), 0), 
                                    (int)colors->orangeWidth) * orangeWidth1) >> 8;

            v0 += (short)((y * colors->orangeCoefV) >> 15);
            f1 = (std::min(std::max((int)(v - v0), 0),
                                    (int)colors->orangeWidth) * orangeWidth1) >> 8;
            *orangeOut = std::min(f1, f2);

            // GREEN CALCS
            u0 = colors->greenMaxU;
            v0 = colors->greenMaxV;

            u0 += (short)((y * colors->greenCoef) >> 15);
            f1 = (std::min(std::max((int)(u0 + colors->greenWidth - u), 0),
                                    (int)colors->greenWidth) * greenWidth1) >> 8; 

            v0 += (short)((y * colors->greenCoef) >> 15);
            f2 = (std::min(std::max((int)(v0 + colors->greenWidth - v), 0),
                                    (int)colors->greenWidth) * greenWidth1) >> 8;

            *greenOut = std::min(f1, f2);
        }
    }
#endif
    return 0;
}
