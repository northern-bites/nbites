#ifndef COLORZONE_H
#define COLORZONE_H

#include "Image/ColorSpace.h"
class ColorZone
{
public:
    ColorZone();
    ColorZone(const ColorZone& c);
    bool within(ColorSpace c);

    float YLo, YHi;
    float SLo, SHi;
    float Hlo, HHi;
};

#endif // COLORZONE_H
