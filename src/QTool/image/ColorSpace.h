#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <math.h>

class ColorSpace
{
public:
    ColorSpace();
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

    float min(float a, float b) {if (a < b) return a; return b;}
    float max(float a, float b) {if (a > b) return a; return b;}


    float red, grn, blue;
};
#endif // COLORSPACE_H
