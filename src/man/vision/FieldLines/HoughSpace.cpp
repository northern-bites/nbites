#include "HoughSpace.h"
#include "HoughSpaceImpl.h"

#include <stdio.h>
#include <climits>
#include <boost/make_shared.hpp>

using namespace std;
using boost::shared_ptr;

namespace man {
namespace vision {
namespace HC = HoughConstants;

boost::shared_ptr<HoughSpace> HoughSpace::create()
{
    return boost::make_shared<HoughSpaceImpl>();
}


HoughSpace::HoughSpace() : acceptThreshold(HC::default_accept_thresh),
                           angleSpread(HC::default_angle_spread)
{
}

}
}
