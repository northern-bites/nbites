#ifndef MetaGait_h_DEFINED
#define MetaGait_h_DEFINED

/**
 * The Meta Gait brokers between two gaits, the 'cur' and the 'next'.
 * Every frame, it reinterpolates between the two gaits based on
 * a total transition time which is specified in the gaits.
 *
 * TODO (Maybe):
 *   - Use a cycloid to transition instead of a linear function
 *   - Clip the maxTime to the length of a step.
 */

#include "AbstractGait.h"
#include "Gait.h"
class MetaGait : public AbstractGait {
public:
    MetaGait();
    ~MetaGait();

    void tick_gait();
    void setNewGaitTarget(Gait &nextTarget);
    void setStartGait(Gait &newCurGait);
private:
    bool updateGaits();
    float getPercentComplete();
    void resetTransitioning();

private:
    Gait curGait, nextGait, newGait;
    bool newGaitSent;
    unsigned int transitionCounter;
    unsigned int transitionFrames;

};

#endif
