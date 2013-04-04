#include "Color.h"

namespace qtool {
namespace image {

Color::Color() : red(0), grn(0), blue(0)
{
}

float Color::getH()
{
    float h;
    if (getC() == 0.0f)
    {
        return 0.5f;
    }
    if (red == getZ())
    {
        h = ((float)(grn - blue) / getC() + 6.0f);
        while (h > 6.0f)
        {
            h = h - 6.0f;
        }
    } else if (grn == getZ())
    {
        h = (blue - red) / getC() + 2.0f;
    } else {
        h = (red - grn) / getC() + 4.0f;
    }
    return h / 6.0f;
}

void Color::setHsz(float h, float s, float z)
{
    //h is an angle, so it wraps around, so when it's 1 it should be 0
    h = h - floor(h);

    //h is an angle; this determines which slice of the circle we are
    //on when the circle is split in 6
    float h1 = 6.0f * h;

    //f is the angle offset within the current slice
    float f = h1 - (int)(h1) + (int)(h1)%2;

    float chroma = z*s;
    float x = chroma*(1 - fabs(f - 1));

    switch ((int)h1)
    {
    case 0:
        red = chroma;
        grn = x;
        blue = 0;
        break;

    case 1:
        red = x;
        grn = chroma;
        blue = 0;
        break;

    case 2:
        red = 0;
        grn = chroma;
        blue = x;
        break;

    case 3:
        red = 0;
        grn = x;
        blue = chroma;
        break;

    case 4:
        red = x;
        grn = 0;
        blue = chroma;
        break;

    case 5:
        red = chroma;
        grn = 0;
        blue = x;
        break;
    }

    float m = z - chroma;
    red+=m; grn+=m; blue+=m;
}

}
}
