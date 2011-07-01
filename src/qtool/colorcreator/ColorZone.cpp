#include "ColorZone.h"

namespace qtool {
namespace colorcreator {

ColorZone::ColorZone()
{
    YLo = SLo = Hlo = 0;
    YHi = SHi = HHi = 1;
}

ColorZone::ColorZone(const ColorZone& cz)
{
    YLo = cz.YLo;
    YHi = cz.YHi;
    SLo = cz.SLo;
    SHi = cz.SHi;
    Hlo = cz.Hlo;
    HHi = cz.HHi;
}

bool ColorZone::within(ColorSpace c)
{
      float h0 = c.getH() - Hlo;
      h0 -= (float)floor(h0);
      float h1 = HHi - Hlo;
      if (h1 < 1)
        h1 -= (float)floor(h1);
      return c.valid() && YLo <= c.getY() && c.getY() <= YHi && SLo <= c.getS() && c.getS() <= SHi && h0 <= h1;
}

}
}
