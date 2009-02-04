/**
 * The StepGenerator (will) house a ton of logic relating to how the steps are
 * configured. It must handle the following tasks:
 *  - Provide future ZMP_REF values for both x and y dimensions
 *  - Decide where to place footsteps depending on input x,y,h
 *  - Oversee the behavior of the WalkingLegs which actually do the leg work
 *  - Handle stopping, both as a motion vector, and when requested
 *    to stop by the underlying switchboard/provider.
 *
 * Subtasks:
 *  - Manage the moving coordinate frame while the robot is walking
 *    - There are in fact two moving coordinate frames:
 *      * one of type s which moves with the last ZMPDstep,
 *      * and one of the f type which moves according to which support step is
 *        currently actually active
 *  - Decide when to throw away dated footsteps
 *  - Decide how to add new footsteps (when, etc)
 *
 * TODO:
 *  - Translate all the matrix<float> into bounded arrays (ufmatrix3)
 *  - Build access to work from the Switchboard or Interface. Need
 *    to be able to change gaits, start and stop, etc
 *  - Enable turning - prob. involves some work with Kinematics as well?
 *  - Read through tick_legs, fillZMP, etc with a beginners mind and write
 *    lots of awesomely descriptive and informative comments.
 *  - Decide if the 'hack-ish' stuff in generate step is really a hack or not
 *  - Move some of the parameters in WalkingParameters into Steps. This includes
 *    stuff like step duration, frames in double support, step height, etc
 *
 * MUSINGS ON BETTER DESIGN:
 *  - Each Step could have a list of sub states which it must undergo
 *    A normal step would have just one DBL and one SINGLE in a row
 *    A starting stopping step could have other types instead.
 *    WalkingLeg could then ask the current Step object what should happen next
 *  - The s coordinate frame could be dispensed with -  we could just
 *    express steps in the f coordinate frame, also this might allow
 *    the step constructor to be called  without knowing about HIP_OFF, etc
 *
 * COORDINATE FRAME NOTE:
 *  There are four important coordinate frames:
 *     - initial (i) is the coordinate frame centered where we begin walking
 *         Since we expect not to walk a net distance of more than .5-1.0km in
 *         one go, we don't need to worry about float overflow.
 *     - foot (f) is the coord. frame centered on the supporting leg.
 *         During walking, this frame changes frequently. It is switched
 *         the instant when the swinging leg enters DOUBLE_PERSISTANT
 *     - center of mass (c) is the coordinate frame centered at the robot's com
 *     - step (s) is the coordinate frame relative to which we define a step
 *         Typically this would be HIP_OFFSET to the inside of the step.
 *
 * @author Johannes Strom
 * @author George Slavov
 * @date Jan 7 2009
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

#include <cstdio>
#include <cmath>
#include <list>
#include <algorithm> //for max
using std::list;
using std::max;

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "Structs.h"
#include "PreviewController.h"
#include "WalkingConstants.h"
#include "WalkingLeg.h"
#include "Kinematics.h"
#include "CoordFrame.h"
#include "Sensors.h"
using namespace boost::numeric;
using namespace Kinematics;

//Debugging flags:
#ifdef WALK_DEBUG
#  define DEBUG_CONTROLLER_COM
#endif

typedef boost::tuple<const list<float>*, const list<float>*> zmp_xy_tuple;
typedef boost::tuple<const vector<float>,const vector<float> > WalkLegsTuple;

static unsigned int MIN_NUM_ENQUEUED_STEPS = 3; //At any given time, we need at least 3
                                     //steps stored in future, current lists

class StepGenerator {
public:
    StepGenerator(Sensors * s, const WalkingParameters *params);
    ~StepGenerator();

    zmp_xy_tuple generate_zmp_ref();

    void tick_controller();
    WalkLegsTuple tick_legs();

    bool isDone(){return _done;}

    void setWalkVector(const float _x, const float _y, const float _theta);

private: // Helper methods
    void swapSupportLegs();

    void generateStep(float _x,float _y,
                      float _theta);
    void fillZMP(const boost::shared_ptr<Step> newStep );
    void fillZMPRegular(const boost::shared_ptr<Step> newStep );
    void fillZMPEnd(const boost::shared_ptr<Step> newStep );

    void startRight();
    void startLeft();

    static const ufmatrix3 get_f_fprime(const boost::shared_ptr<Step> step);
    static const ufmatrix3 get_fprime_f(const boost::shared_ptr<Step> step);
    static const ufmatrix3 get_sprime_s(const boost::shared_ptr<Step> step);
    static const ufmatrix3 get_s_sprime(const boost::shared_ptr<Step> step);

    void debugLogging();

private:
    // Walk vector:
    //  * x - forward
    //  * y - lateral (left is positive)
    //  * theta - angular (counter-clockwise is positive)
    // NOTE/TODO: the units of these are not well-defined yet
    float x;
    float y;
    float theta;

    bool _done;

    ufvector3 com_i;
    //ublas::vector<float> com_f;
    // need to store future zmp_ref values (points in xy)
    list<float> zmp_ref_x, zmp_ref_y;
    list<boost::shared_ptr<Step> > futureSteps; //stores steps not yet zmpd
    //Stores currently relevant steps that are zmpd but not yet completed.
    //A step is consider completed (obsolete/irrelevant) as soon as the foot
    //enters into double support (perisistant)
    list<boost::shared_ptr<Step> > currentZMPDSteps;
    boost::shared_ptr<Step> lastQueuedStep;

    //Reference Frames for ZMPing steps
    //These are updated when we ZMP a step - they are the 'future', if you will
    ufmatrix3 si_Transform;
    ufvector3 last_zmp_end_s;

    //Steps for the Walking Leg
    boost::shared_ptr<Step> lastStep_s;
    boost::shared_ptr<Step> supportStep_s;
    boost::shared_ptr<Step> swingingStep_s;
    boost::shared_ptr<Step> supportStep_f;
    boost::shared_ptr<Step> swingingStep_f;
    boost::shared_ptr<Step> swingingStepSource_f;

    //Reference frames for the Walking Leg
    //These are updated in real time, as we are processing steps
    //that are being sent to the WalkingLegs
    //Translation matrix to transfer points in the non-changing 'i'
    //coord. frame into points in the 'f' coord frame
    ufmatrix3 if_Transform;
    ufmatrix3 fc_Transform;
    // These hold the initial position of the left/right foot when they are
    // in support mode. It is relative to the 'i' coord frame.
    ufmatrix3 initStartLeft;
    ufmatrix3 initStartRight;

    Sensors * sensors;
    const WalkingParameters *walkParams;
    bool nextStepIsLeft;

    WalkingLeg leftLeg, rightLeg;

    WalkController *controller_x, *controller_y;
#ifdef DEBUG_CONTROLLER_COM
    FILE* com_log;
#endif
};

#endif
