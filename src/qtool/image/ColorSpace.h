#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <cmath>
#include "Common.h"

class ColorSpace
{
    // holds an rgb value in the form #XXRRGGBB
    typedef unsigned int RGB;

public:
    ColorSpace();

    static byte redFromYUV(byte y, byte u, byte v) {
        return safeFloatToByte(y + 1.13983f * (v - 128.0f));
    }

    static byte blueFromYUV(byte y, byte u, byte v) {
        return safeFloatToByte(y + 2.03211f * (u - 128.0f));
    }

    static byte greenFromYUV(byte y, byte u, byte v) {
        return safeFloatToByte(y - 0.39465f * (u - 128.0f) - 0.58060f * (v - 128.0f));
    }

    //topmost bytes are set to #FF
    static RGB RGBFromYUV(byte y, byte u, byte v) {
        return makeRGB(redFromYUV(y, u, v), greenFromYUV(y, u, v),  blueFromYUV(y, u, v));
    }

    static RGB makeRGB(byte r, byte g, byte b, byte a = 0xFF) {
        RGB result;
        ((byte*) &result)[3] = a;
        ((byte*) &result)[2] = r;
        ((byte*) &result)[1] = g;
        ((byte*) &result)[0] = b;
        return result;
    }

    static RGB makeRGBFromSingleByte(byte single_byte) {
        return makeRGB(single_byte, single_byte, single_byte);
    }

    float getRed() {return red;}
    float getGreen() {return grn;}
    float getBlue() {return blue;}
    float getY() {return 0.299f * red + 0.587f * grn + 0.114f * blue;}
    float getU() {return -0.14713 * red - 0.28886f * grn + 0.436f * blue;}
    float getV() {return 0.615f * red - 0.51499f * grn - 0.10001f * blue;}
    float getH();
    float getS() { return getC() / getZ();}
    float getZ() { return max(max(red, grn), blue); }
    float getC() { return getZ() - min(min(red, grn), blue);}
    bool valid() { return 0 <= red && red < 1 && 0 <= grn && grn < 1 && 0 <= blue && blue < 1;}

    void setRgb(float r, float g, float b) { red = r; grn = g; blue = b;}
    void setRgb(int r, int g, int b) { setRgb((float)r / 256.0f, (float)g / 256.0f, (float)b / 256.0f);}
    void setYuv(float y, float u, float v) { red = y + 1.13983f * v; grn = y - 0.39465f * u - 0.58060f * v;
                                             blue = y + 2.03211f * u;}
    void setYuv(int y, int u, int v) {setYuv((float)y / 256.0f, (float)(u - 128) / 256.0f,
                                             (float)(v - 128) / 256.0f);}
    void setHsz(float h, float s, float z);

    // guards against negative values and values that might overflow in a byte
    static byte safeFloatToByte(float x) {
        return (byte) min(max(x, 0.0f), 255.0f);
    }

    int cvb(float x) {return (int)(min(max(256 * x, 0.0f), 255.0f));}
    int getRb() { return cvb(red);}
    int getGb() { return cvb(grn);}
    int getBb() { return cvb(blue);}
    int getYb() { return cvb(getY());}
    int getUb() { return cvb(getU() + 0.5f);}
    int getVb() { return cvb(getV() + 0.5f);}
    int getHb() { return cvb(getH());}
    int getSb() { return cvb(getS());}
    int getZb() { return cvb(getZ());}

    static float min(float a, float b) {if (a < b) return a; return b;}
    static float max(float a, float b) {if (a > b) return a; return b;}

private:

    float red, grn, blue;
};
#endif // COLORSPACE_H
