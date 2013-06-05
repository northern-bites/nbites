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
    // 16,667 us and 50,000 us away from now.
    // This means that (if we run at 30 frames/sec) we will send data
    // once per frame on average, while providing significant randomization.
    // We could send duplicates or skip frames with this method, but we will
    // not send more than one duplicate or skip more than one frame in a row.
    // NOTE: Since rand() generates a value between 0 and some undefined
    //       large maximum (at least 32767), to get a truly random distribution,
    //       we generate random numbers until we get one less than 33,333.

    int random;
    // Truly random is too slow. Do this instead.
    random = rand() % 33333;
/*    do
    {
        random = rand();
    } while (random > 33333); // Ensure nextSendTime doesn't exceed 50,000
*/

    nextSendTime += random + 16667;
}

}

}
