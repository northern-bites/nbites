/**
 * Class to hold time related information.
 * @author Wils Dawson 5/13/12 adapted from old version.
 */

#ifndef CommTimer_H
#define CommTimer_H

#include <vector>

//TODO: include these
//#include "CommDef.h"
typedef long long llong;
//#include "commconfig.h"

class CommTimer
{
public:
    /**
     * Constructor
     * @param f: Pointer to the function we should use for time.
     */
    CommTimer(llong (*f)());

    /**
     * @return: The current time on this robot after starting.
     *          Takes offset into account to try to syncronize
     *          clocks accross robots.
     */
    llong timestamp() {return time() - epoch + teamClockOffset;}

    /**
     * Computes a reasonable random amount of time to wait
     * before sending the next packet. This aids in
     * avoiding regular spectrum interference.
     */
    void teamPacketSent();

    /**
     * @return: true on GO, false on WAIT
     */
    bool timeToSend() {return timestamp() - nextSendTime >= 0;}

    /**
     * Sets the offset for team clock syncronization.
     * @param off: The offset to increase our offset by.
     */
    void setOffset(llong off) {teamClockOffset += off;}

    /**
     * Gets the offset for team clock syncronization.
     */
    llong getOffset() const {return teamClockOffset;}

private:
    llong (*time)();            // Pointer to function that returns current time
    llong epoch;                // Time at start of execution.
    llong nextSendTime;         // The timestamp after which we can send again.
    llong teamClockOffset;      // The amount to offset our clock to match the
                                //     clocks of our teammates.
    bool  shouldUpdate;         // Used to check if we should update our offset.
};

#endif // CommTimer_H
