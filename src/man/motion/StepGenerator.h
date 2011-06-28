
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 * The StepGenerator house a large portion of logic relating to how the steps are
 * configured. It must handle the following tasks:
 *  - Provide future ZMP_REF values for both x and y dimensions
 *  - Decide where to place footsteps depending on setSteps, setSpeed or setDist
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
 *   - There is a BIG bug which is makes stopping take an extra step. See README
 *     for details. The gist is that ZMP preview values are generated
 *     for when the step we create becomes the support step.
 *     Since when we initially generate the step, we want it as a destination,
 *     when we move a step into currentZMPDSteps, we are committing not only
 *     to having the robot place its foot there, but ALSO to taking another FULL
 *     step with the other leg (since the ZMP values are generated as such)
 *
 * MUSINGS ON BETTER DESIGN:
 *  - Each Step could have a list of sub states which it must undergo
 *    A normal step would have just one DBL and one SINGLE in a row
 *    A starting stopping step could have other types instead.
 *    WalkingLeg could then ask the current Step object what should happen next
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
 * @updated August 2009
 *
 * @author Nathan Merritt
 * @updated November 2010
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

#include <cstdio>
#include <math.h>
#include <list>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "Structs.h"
#include "WalkController.h"
#include "WalkingConstants.h"
#include "WalkingLeg.h"
#include "WalkingArm.h"
#include "Kinematics.h"
#include "CoordFrame.h"
#include "Sensors.h"
#include "NaoPose.h"
#include "NBMatrixMath.h"
#include "ZmpEKF.h"
#include "ZmpAccExp.h"

//Debugging flags:
#ifdef WALK_DEBUG
#  define DEBUG_CONTROLLER_COM
#  define DEBUG_SENSOR_ZMP
#endif

// ZMP Preview Queue Debugging
#define DEBUG_ZMP_REF

typedef boost::tuple<const std::list<float>*,
                     const std::list<float>*> zmp_xy_tuple;
typedef boost::tuple<LegJointStiffTuple,
                      LegJointStiffTuple> WalkLegsTuple;
typedef boost::tuple<ArmJointStiffTuple,
                     ArmJointStiffTuple> WalkArmsTuple;

static unsigned int MIN_NUM_ENQUEUED_STEPS = 3; //At any given time, we need at least 3
                                     //steps stored in future, current lists

class StepGenerator {
public:
    StepGenerator(boost::shared_ptr<Sensors> s,
                  boost::shared_ptr<NaoPose> p,
                  const MetaGait * _gait);
    ~StepGenerator();

    void tick_controller();
    WalkLegsTuple tick_legs();
    WalkArmsTuple tick_arms();

    bool isDone() const { return done; }

    void setSpeed(const float _x, const float _y, const float _theta);
    int setDestination(const float rel_x, const float rel_y, const float rel_theta,
		       float gain = 1.0f);
    void takeSteps(const float _x, const float _y, const float _theta,
                   const int _numSteps);

    std::vector <float> getOdometryUpdate();

    void resetHard();

    static std::vector<float>*
    getDefaultStance(const Gait & wp);

    const SupportFoot getSupportFoot() const {
        return supportFoot;
    }

private: // Helper methods
    zmp_xy_tuple generate_zmp_ref();

    void findSensorZMP();

    // checks reliability of Sensor measurements, scales sensorZMP
    float scaleSensors(const float sensorZMP, const float perfectZMP);

    void swapSupportLegs();

    void generateStep(float _x,float _y,
                      float _theta);
    void fillZMP(const Step::ptr newStep );
    void fillZMPRegular(const Step::ptr newStep );
    void fillZMPEnd(const Step::ptr newStep);

    void resetSteps(const bool startLeft);

    static const NBMath::ufmatrix3 get_f_fprime(const Step::ptr step);
    static const NBMath::ufmatrix3 get_fprime_f(const Step::ptr step);
    static const NBMath::ufmatrix3 get_sprime_s(const Step::ptr step);
    static const NBMath::ufmatrix3 get_s_sprime(const Step::ptr step);

    const bool decideStartLeft(const float lateralVelocity,
                               const float radialVelocity);
    void clearFutureSteps();

    void resetQueues();
    void resetOdometry(const float initX, const float initY);
    void updateOdometry(const std::vector<float> &deltaOdo);
    void debugLogging();
    void update_FtoI_transform();
private:
    // Walk vector:
    //  * x - forward
    //  * y - lateral (left is positive)
    //  * theta - angular (counter-clockwise is positive)
    // NOTE/TODO: the units of these are not well-defined yet
    float x;
    float y;
    float theta;

    bool done;

    bool hasDestination;
    bool brokenSensorWarning;

    SensorAngles sensorAngles;

    NBMath::ufvector3 com_i,joints_com_i,last_com_c,com_f,est_zmp_i;
    //boost::numeric::ublas::vector<float> com_f;
    // need to store future zmp_ref values (points in xy)
    std::list<float> zmp_ref_x, zmp_ref_y;
    std::list<Step::ptr > futureSteps; //stores steps not yet zmpd
    //Stores currently relevant steps that are zmpd but not yet completed.
    //A step is consider completed (obsolete/irrelevant) as soon as the foot
    //enters into double support (perisistant)
    std::list<Step::ptr > currentZMPDSteps;
    Step::ptr lastQueuedStep;

    //Reference Frames for ZMPing steps
    //These are updated when we ZMP a step - they are the 'future', if you will
    NBMath::ufmatrix3 si_Transform;
    NBMath::ufvector3 last_zmp_end_s;

    //Steps for the Walking Leg
    Step::ptr lastStep_s;
    Step::ptr supportStep_s;
    Step::ptr swingingStep_s;
    Step::ptr supportStep_f;
    Step::ptr swingingStep_f;
    Step::ptr swingingStepSource_f;

    //Reference frames for the Walking Leg
    //These are updated in real time, as we are processing steps
    //that are being sent to the WalkingLegs
    //Translation matrix to transfer points in the non-changing 'i'
    //coord. frame into points in the 'f' coord frame
    //We also maintain their inverses
    NBMath::ufmatrix3 if_Transform;
    NBMath::ufmatrix3 fi_Transform;
    NBMath::ufmatrix3 fc_Transform;
    NBMath::ufmatrix3 cf_Transform;
    NBMath::ufmatrix3 cc_Transform; //odometry

    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
    const MetaGait *gait;
    bool nextStepIsLeft;

    WalkingLeg leftLeg, rightLeg;
    WalkingArm leftArm, rightArm;

    SupportFoot supportFoot;

    WalkController *controller_x, *controller_y;

    ZmpEKF zmp_filter;
    ZmpAccExp acc_filter;

    NBMath::ufvector4 accInWorldFrame;

#ifdef DEBUG_CONTROLLER_COM
    FILE* com_log;
#endif
#ifdef DEBUG_SENSOR_ZMP
    FILE* zmp_log;
#endif
#ifdef DEBUG_ZMP_REF
    FILE* zmp_ref_log;
#endif

};

#endif
