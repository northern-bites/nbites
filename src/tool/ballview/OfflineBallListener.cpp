#include "OfflineBallListener.h"

namespace tool {

OfflineBallListener::OfflineBallListener() {}

void OfflineBallListener::run_()
{
    ballIn.latch();
}

}
