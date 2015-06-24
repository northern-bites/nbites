#ifndef nbites_thumbnails_h
#define nbites_thumbnails_h

namespace resconvert {
    const int YUV422_PixelSize = 2;
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
 
    static inline int indexm(int col, int row, ImageResolution res) {
        return ((row * ImageWidth[res] + col) >> 1 );
    }
    
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
