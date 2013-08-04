#include "OfflineLocListener.h"

namespace tool{
OfflineLocListener::OfflineLocListener() {}

void OfflineLocListener::run_()
{
    locIn.latch();
    particleIn.latch();
}
} // namespace tool
