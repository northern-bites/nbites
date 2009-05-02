
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
 *  - Build access to work from the Switchboard or Interface. Need
 *    to be able to change gaits, start and stop, etc
 *  - Read through tick_legs, fillZMP, etc with a beginners mind and write
 *    lots of awesomely descriptive and informative comments.
 *  - Move some of the parameters in WalkingParameters into Steps. This includes
 *    stuff like step duration, frames in double support, step height, etc
 *  - Fix the ZMP leg length to be more intelligent. Must depend on if we are
 *    moving left/right, and how much, etc. This depends on the next step in many
 *    ways, so I'm not sure how to do this.
 *  - We need to clip incoming x,y,theta because there is no protection currently
 *  - We need an observer or we need to fix the preview controller's bugs
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
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

#include <cstdio>
#include <cmath>
#include <list>

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "Structs.h"
#include "WalkController.h"
#include "WalkingConstants.h"
#include "WalkingLeg.h"
#include "Kinematics.h"
#include "CoordFrame.h"
#include "Sensors.h"
#include "NBMatrixMath.h"
#include "ZmpEKF.h"
#include "ZmpAccEKF.h"

//Debugging flags:
#ifdef WALK_DEBUG
#  define DEBUG_CONTROLLER_COM
#  define DEBUG_SENSOR_ZMP
#endif

typedef boost::tuple<const std::list<float>*,
                     const std::list<float>*> zmp_xy_tuple;
typedef boost::tuple<std::vector<float>,
                     std::vector<float> > WalkLegsTuple;

static unsigned int MIN_NUM_ENQUEUED_STEPS = 3; //At any given time, we need at least 3
                                     //steps stored in future, current lists

class StepGenerator {
public:
    StepGenerator(boost::shared_ptr<Sensors> s);
    ~StepGenerator();

    void tick_controller();
    WalkLegsTuple tick_legs();

    bool isDone() { return done; }

    void setSpeed(const float _x, const float _y, const float _theta);

    bool resetGait(const WalkingParameters * _wp);

    std::vector <float> getOdometryUpdate();

    void resetHard();

    const SupportFoot getSupportFoot() const {
        return supportFoot;
    }

private: // Helper methods
    zmp_xy_tuple generate_zmp_ref();
    void generate_steps();

    void findSensorZMP();
    float scaleSensors(const float sensorZMP, const float perfectZMP);

    void swapSupportLegs();

    void generateStep(float _x,float _y,
                      float _theta);
    void fillZMP(const boost::shared_ptr<Step> newStep );
    void fillZMPRegular(const boost::shared_ptr<Step> newStep );
    void fillZMPEnd(const boost::shared_ptr<Step> newStep );
    void addStartZMP(const boost::shared_ptr<Step> newStep );

    void startRight();
    void startLeft();

    static const NBMath::ufmatrix3 get_f_fprime(const boost::shared_ptr<Step> step);
    static const NBMath::ufmatrix3 get_fprime_f(const boost::shared_ptr<Step> step);
    static const NBMath::ufmatrix3 get_sprime_s(const boost::shared_ptr<Step> step);
    static const NBMath::ufmatrix3 get_s_sprime(const boost::shared_ptr<Step> step);

    void resetQueues();
    void resetOdometry();
    void debugLogging();
    void updateDebugMatrix();
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

    NBMath::ufvector3 com_i,last_com_c,est_zmp_i;
    //boost::numeric::ublas::vector<float> com_f;
    // need to store future zmp_ref values (points in xy)
    std::list<float> zmp_ref_x, zmp_ref_y;
    std::list<boost::shared_ptr<Step> > futureSteps; //stores steps not yet zmpd
    //Stores currently relevant steps that are zmpd but not yet completed.
    //A step is consider completed (obsolete/irrelevant) as soon as the foot
    //enters into double support (perisistant)
    std::list<boost::shared_ptr<Step> > currentZMPDSteps;
    boost::shared_ptr<Step> lastQueuedStep;

    //Reference Frames for ZMPing steps
    //These are updated when we ZMP a step - they are the 'future', if you will
    NBMath::ufmatrix3 si_Transform;
    NBMath::ufvector3 last_zmp_end_s;

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
    NBMath::ufmatrix3 if_Transform;
    NBMath::ufmatrix3 fc_Transform;
    NBMath::ufmatrix3 ic_Transform; //odometry
    // These hold the initial position of the left/right foot when they are
    // in support mode. It is relative to the 'i' coord frame.
    NBMath::ufmatrix3 initStartLeft;
    NBMath::ufmatrix3 initStartRight;

    boost::shared_ptr<Sensors> sensors;
    const WalkingParameters *walkParams;
    bool nextStepIsLeft;
    // HACK: this variable holds the number of frames we have to wait before
    //       we can start walking (NUM_PREVIEW_FRAMES).
    int waitForController;

    WalkingLeg leftLeg, rightLeg;
    SupportFoot supportFoot;

    WalkController *controller_x, *controller_y;

    ZmpEKF zmp_filter;
    ZmpAccEKF acc_filter;

    NBMath::ufvector4 accInWorldFrame;

#ifdef DEBUG_CONTROLLER_COM
    FILE* com_log;
    NBMath::ufmatrix3 fi_Transform;
#endif
#ifdef DEBUG_SENSOR_ZMP
    FILE* zmp_log;
#endif

};

#endif
