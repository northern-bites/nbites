#ifndef nbimage_rescon_h
#define nbimage_rescon_h

#include <stdint.h>
#include <iostream>

namespace rescon {
    const int YUV422_PixelSize = 2;
    
    
    //Gives image (Pixel) row/col for index of x type.
#define rowm(index, res) (index / (ImageWidth[res] >> 1))
#define rowp(index, res) (index / (ImageWidth[res]     ))
#define rows(index, res) (index / (ImageWidth[res] << 1))
    
#define colm(index, res) (index % (ImageWidth[res] >> 1))
#define colp(index, res) (index % (ImageWidth[res]    ))
#define cols(index, res) (index % (ImageWidth[res] << 1))
    
    //Gives index of x type for image (Pixel) row/col
#define indexm(col, row, res) ((row * ImageWidth[res] + col) >> 1 )
#define indexp(col, row, res) ((row * ImageWidth[res] + col)      )
#define indexs(col, row, res) ((row * ImageWidth[res] + col) << 1 )
    
    typedef uint32_t YUVMacroPixel;
    typedef uint16_t YUVPixel;
    typedef uint8_t YUVSubPixel;
    
    typedef enum {
        R080_060 = 0,
        R160_120 = 1,
        R320_240 = 2,
        R640_480 = 3
    } ImageResolution;
    
    const size_t ImageBufferSize[] = {
        (80  * 60  * YUV422_PixelSize),
        (160 * 120 * YUV422_PixelSize),
        (320 * 240 * YUV422_PixelSize),
        (640 * 480 * YUV422_PixelSize)
    };
    
    const int ImageWidth[] = {
        80,
        160,
        320,
        640
    };
    
    const int ImageHeight[] = {
        60,
        120,
        240,
        480
    };
    
    static inline void resDownPck(ImageResolution fromRes, const YUVSubPixel * from, ImageResolution toRes, YUVSubPixel * to) {
        const YUVMacroPixel * fromPnt = (const YUVMacroPixel *) from;
        YUVMacroPixel * toPnt = (YUVMacroPixel *) to;
        int ratio = ImageWidth[fromRes] / ImageWidth[toRes];
        
        for (int y = 0; y < ImageHeight[toRes]; ++y) {
            for (int x = 0; x < ImageWidth[toRes]; ++x) {
                int ti = indexm(x, y, toRes);
                int fi = indexm(x * ratio, y * ratio, fromRes);
                toPnt[ti] = fromPnt[fi];
            }
        }
    }
}

#endif