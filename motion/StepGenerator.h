/**
 * The StepGenerator (will) house a ton of logic relating to how the steps are
 * configured. It must handle the following tasks:
 *  - Provide future ZMP_REF values for both x and y dimensions
 *  - Decide where to place footsteps depending on input x,y,h
 *  - Handle stopping, both as a motion vector, and when requested
 *    to stop by the underlying switchboard/provider.
 *
 * Subtasks:
 *  - Manage the moving coordinate frame while the robot is walking
 *  - Decide when to throw away dated footsteps
 *  - Decide how to add new footsteps (when, etc)
 *
 *  TODO:
 *  - Figure out what the units are for the walk vector.
 *  - Figure out how to calculate number of required pre-planned steps,
 *    so that we always have enough previewable zmp_ref values.
 *
 *
 *  COORDINATE FRAME NOTE:
 *  There are four important coordinate frames:
 *     - initial (i) is the coordinate frame centered where we begin walking
           Since we expect not to walk a net distance of more than .5-1.0km in
           one go, we don't need to worry about float overflow.
       - foot (f) is the coord. frame centered on the supporting leg.
           During walking, this frame changes frequently. It is switched
           the instant when the swinging leg enters DOUBLE_PERSISTANT
       - center of mass (c) is the coordinate frame centered at the robot's com
       - step (s) is the coordinate frame relative to which we define a step
           Typically this would be HIP_OFFSET to the inside of the step.
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

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
using namespace boost::numeric;
using namespace Kinematics;

typedef boost::tuple<const list<float>*, const list<float>*> zmp_xy_tuple;
typedef boost::tuple<const vector<float>,const vector<float> > WalkLegsTuple;
/**
 * Simple container to hold information about future steps.
 */
struct Step:point<float> {
    float theta;
    float duration;
    Foot foot;
    Step(const float _x, const float _y, const float _theta,
         const float _duration, const Foot _foot)
        : point<float>(_x,_y), theta(_theta), duration(_duration), foot(_foot){}
};

static int MIN_NUM_ENQUEUED_STEPS = 3; //At any given time, we need at least 3
                                     //steps stored in future, current lists

class StepGenerator {
public:
    StepGenerator(const WalkingParameters *params);
    ~StepGenerator();


    zmp_xy_tuple generate_zmp_ref();

    void tick_controller();
    WalkLegsTuple tick_legs();


    void setWalkVector(const float _x, const float _y, const float _theta);

private: // Helper methods
    void generateStep(const float _x, const float _y,
                      const float _theta);
    void fillZMP(const boost::shared_ptr<Step> newStep );

    ublas::matrix<float> get_f_fprime(boost::shared_ptr<Step> step);
    ublas::matrix<float> get_fprime_f(boost::shared_ptr<Step> step);
private:
    // Walk vector:
    //  * x - forward
    //  * y - lateral (left is positive)
    //  * theta - angular (counter-clockwise is positive)
    // NOTE/TODO: the units of these are not well-defined yet
    float x;
    float y;
    float theta;


    ublas::vector<float> com_i;
    //ublas::vector<float> com_f;
    // need to store future zmp_ref values (points in xy)
    list<float> zmp_ref_x, zmp_ref_y;
    list<boost::shared_ptr<Step> > futureSteps; //stores steps not yet zmpd
    //Stores currently relevant steps that are zmpd but not yet completed.
    //A step is consider completed (obsolete/irrelevant) as soon as the foot
    //enters into double support (perisistant)
    list<boost::shared_ptr<Step> > currentZMPDSteps;


    boost::shared_ptr<Step> supportStep_s;
    boost::shared_ptr<Step> swingingStep_s;
    boost::shared_ptr<Step> supportStep_f;
    boost::shared_ptr<Step> swingingStep_f;
    boost::shared_ptr<Step> lastZMPDStep; //Last step turned into ZMP values
    point<float> coordOffsetLastZMPDStep;
    //Translation matrix to transfer points in the non-changing 'i'
    //coord. frame into points in the 'f' coord frame
    ublas::matrix<float> if_Transform;
    ublas::matrix<float> fc_Transform;
    // These hold the initial position of the left/right foot when they are
    // in support mode. It is relative to the 'i' coord frame.
    ublas::matrix<float> initStartLeft;
    ublas::matrix<float> initStartRight;

    const WalkingParameters *walkParams;
    bool nextStepIsLeft;

    WalkingLeg leftLeg, rightLeg;

    WalkController *controller_x, *controller_y;
};

#endif
