#include "Color.h"

namespace qtool {
namespace image {

Color::Color()
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
    h = 6.0f * (h - (float)floor(h));
    float a = z * (1 - s);
    float b = z * (1 - s * (1 - fabs(((int)h + 5) % 2 - 1)));

    switch ((int)h)
    {
    case 0:
        red = z;
        grn = b;
        blue = a;
        break;

    case 1:
        red = b;
        grn = z;
        blue = a;
        break;

    case 2:
        red = a;
        grn = z;
        blue = b;
        break;

    case 3:
        red = a;
        grn = b;
        blue = z;
        break;

    case 4:
        red = b;
        grn = a;
        blue = z;
        break;

    case 5:
        red = z;
        grn = a;
        blue = b;
        break;
    }
}

}
}
