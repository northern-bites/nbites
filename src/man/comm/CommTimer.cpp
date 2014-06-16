/**
 * Class to hold time related information.
 * @author Wils Dawson 5/13/12 adapted from old version.
 */

#include <cstdlib>

#include "CommTimer.h"

namespace man {

namespace comm {

CommTimer::CommTimer(llong (*f)())
    : time(f), epoch(time())
{
    // Initialize nextSendDelay so that we immediately send.
    nextSendTime = timestamp();
    // Seed psuedo-random number generator with current time.
    // Should be significanly different on each robot so that they won't align.
    srand(static_cast<int>(nextSendTime));
}

void CommTimer::teamPacketSent()
{
    nextSendTime = timestamp();

    // Now randomize the delay such that is between
    // 190000 us and 220000 us away from now.
    // This means that (if we run at 30 frames/sec) we will send data
    // around once every six frames on average, while providing significant
    // randomization. We will not send duplicates, and may skip more or less
    // than 5 frames. This is necessary to ensure we comply with the 2014 rule
    // that robots send no more than 5 messages per robot per second.

    int random;
    random = rand() / RAND_MAX * 30000;

    nextSendTime += random + 190000;
}

}

}
