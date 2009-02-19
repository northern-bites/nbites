 
#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

/**
 * The walk provider is the central connection point between all the parts
 * of the walking system. It ties together the controller, and inverse
 * kinematics.
 *
 * The walk provider should be asked to calculateNextJoints each timestep
 * It in turn asks for a new CoM target from the controller, and supervises
 * the left and right `WalkingLegs` in reaching that target.
 *
 * At the end, it updates the super class' joints for each leg chain
 * which are read by the switchboard.
 */

#include <vector>
using namespace std;

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "WalkingConstants.h"
#include "StepGenerator.h"
#include "WalkCommand.h"

typedef boost::tuple<const vector<float>,const vector<float> > WalkLegsTuple;

//NOTE: we need to get passed a reference to the switchboard so we can
//know the length of a motion frame!!
class WalkProvider : public MotionProvider {
public:
    WalkProvider(boost::shared_ptr<Sensors> s);
    virtual ~WalkProvider();

    void requestStopFirstInstance();
    void calculateNextJoints();

	void setCommand(const MotionCommand* command)
        {
            setCommand(reinterpret_cast<const WalkCommand*>(command));
        }
	void setCommand(const WalkCommand * command);

private:
    virtual void setActive();

    boost::shared_ptr<Sensors> sensors;

    const WalkingParameters * walkParameters;
    StepGenerator stepGenerator;
    bool pendingCommands;

    mutable pthread_mutex_t walk_command_mutex;
    const WalkCommand * nextCommand;

    //Temp solution to arms
    vector<float> rarm_angles, larm_angles;
};

#endif

