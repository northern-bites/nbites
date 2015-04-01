#include <math.h>
#include "Blob.h"

namespace man {
namespace vision {

double Blob::angle()
{
    if (!solved) 
        solve();


    return atan2(mxy, mx - my) / 2;
}

double Blob::principalLength1()
{
    if (!solved) 
        solve();

    return sqrt((mx + my + len) / 2) / sw;
}

double Blob::principalLength2()
{
    if (!solved)
        solve();

    return sqrt((mx + my - len) / 2) / sw;
}

void Blob::solve()
{
    mx = sw * sx2 - sx * sx;
    my = sw * sy2 - sy * sy;
    mxy = 2 * (sw * sxy - sx * sy);
    len = sqrt(mxy * mxy + (mx - my) * (mx - my));
    solved = true;
}

}
}
