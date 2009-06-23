#ifndef _WalkParameters_h_DEFINED
#define _WalkParameters_h_DEFINED

#include <vector>
#include <boost/shared_ptr.hpp>
#include "NBMath.h"

namespace WP {
/**
 * STANCE CONFIG holds the following parameters:
 *  bodyHeight      -- CoM Height
 *  bodyOffsetY     -- Forward displacement of CoM
 *  legSeparationY  -- Horizontal distance between feet
 *  bodyRotationY   -- Body angle around Y axis
 *  legRotationY    -- Angle between feet (around Z axis)
 *
 */
enum StanceConfig {
    BODY_HEIGHT=0,
    BODY_OFF_X,
    LEG_SEPARATION_Y,
    BODY_ROT_Y,
    LEG_ROT_Z,
    LEN_STANCE_CONFIG
};

/**
 * STEP CONFIG
 *  stepDuration         -- time allocated for each step
 *  dblSupportPercent   -- fraction of time in double support
 *  stepHeight           -- step height during a step
 *  maxVelX              -- maximum forward velocity of step
 *  maxVelY              -- maximum lateral velocity of step
 *  maxVelTheta          -- maximum angular velocity of step
 */
enum StepConfig{
    DURATION,
    DBL_SUPP_P,
    STEP_HEIGHT,
    MAX_VEL_X,
    MAX_VEL_Y,
    MAX_VEL_THETA,
    LEN_STEP_CONFIG
};

/**
 * ZMP CONFIG holds the following parameters:
 *  footCenterX                   -- footLengthX
 *  doubleSupportStaticPercentage -- zmp static percentage
 *  lZMPOffY                -- left zmp off
 *  rZMPOffY               -- right zmp off
 *  strafeZMPOff               -- turn zmp offset
 *  turnZMPOff                 -- strafe zmp offset
 *
 */
enum ZmpConfig{
    FOOT_CENTER_X=0,
    DBL_SUP_STATIC_P,
    L_ZMP_OFF_Y,
    R_ZMP_OFF_Y,
    STRAFE_ZMP_OFF,
    TURN_ZMP_OFF,
    LEN_ZMP_CONFIG
};

/**
 * JOINT HACK CONFIG
 *  lHipAmplitude  -- magnitude of angular addition to hip during step
 *  rHipAmplitude -- magnitude of angular addition to hip during step
 */
enum JointHackConfig{
    L_HIP_AMP=0,
    R_HIP_AMP,
    LEN_HACK_CONFIG
};

/**
 * SENSOR CONFIG
 * observerScale   -- proportion of observer feedback
 * angleScale      -- proportion of angleXY feedback
 */
enum SensorConfig{
    OBSERVER_SCALE=0,
    ANGLE_SCALE,
    LEN_SENSOR_CONFIG
};

/**
 * STIFFNESS CONFIG
 * hipStiff     -- stiffnesses for the hip
 * KPStiff      -- stiffnesses for knee pitch
 * APStiff      -- stiffnesses for ankle pitch
 * ARStiff      -- stiffnesses for ankle roll
 * armStiff     -- stiffnesses for the arms
 */
enum StiffnessConfig{
    HIP = 0,
    KP,
    AP,
    AR,
    ARM,
    LEN_STIFF_CONFIG
};

/**
 * ODO CONFIG
 * xOdoScale  -- odometry calibration for forward direction
 * yOdoScale  -- odometry calibration for lateral direction
 * thetaOdoScale  -- odometry calibration for rotational direction
 */
enum OdoConfig{
    X_SCALE = 0,
    Y_SCALE,
    THETA_SCALE,
    LEN_ODO_CONFIG
};

/**
 * ARM CONFIG
 * armAmplitude -- angle amplitude of arm motion
 */
enum ArmConfig{
    AMPLITUDE = 0,
    LEN_ARM_CONFIG
};

    //How to convert from Python (deg/cm) to C++ (rad/mm)
    static const float NONE = 1.0f;
    static float STANCE_CONVERSION[LEN_STANCE_CONFIG]= {CM_TO_MM,//com height
                                                        CM_TO_MM,//x off
                                                        CM_TO_MM,//Y separation
                                                        TO_RAD,//angleY
                                                        TO_RAD};//foot angle
    static float STEP_CONVERSION[LEN_STEP_CONFIG]= {NONE,//step time
                                                    NONE,//dblSupFrac
                                                    CM_TO_MM,//step height
                                                    CM_TO_MM,//max vel x
                                                    CM_TO_MM,//max vel y
                                                    CM_TO_MM,};//max vel t
    static float ZMP_CONVERSION[LEN_ZMP_CONFIG]= {CM_TO_MM,//foot center
                                                  NONE,//zmp static perc
                                                  CM_TO_MM,//l zmp off
                                                  CM_TO_MM,//r zmp off
                                                  CM_TO_MM,//strafe zmp off
                                                  CM_TO_MM,};//turn zmp off
    static float HACK_CONVERSION[LEN_HACK_CONFIG]= {TO_RAD,//hip hack l
                                                    TO_RAD};//hip hack r
    static float SENSOR_CONVERSION[LEN_SENSOR_CONFIG]= {NONE,//Observer scale
                                                      NONE};//angle xy scale
    static float STIFF_CONVERSION[LEN_STIFF_CONFIG]= {NONE,//hip
                                                      NONE,//knee
                                                      NONE,//ap
                                                      NONE,//ar
                                                      NONE};//arms
    static float ODO_CONVERSION[LEN_ODO_CONFIG]= {NONE,//xodoscale
                                                  NONE,//yodoscale
                                                  NONE};//thetaodoscale
    static float ARM_CONVERSION[LEN_ARM_CONFIG]= {TO_RAD};//arm amplitude



};//End namespace WP

class WalkParameters{
public:
    WalkParameters(const WalkParameters & other);
    WalkParameters(const boost::shared_ptr<WalkParameters> other);
    WalkParameters(const float _stance_config[WP::LEN_STANCE_CONFIG],
                   const float _step_config[WP::LEN_STEP_CONFIG],
                   const float _zmp_config[WP::LEN_ZMP_CONFIG],
                   const float _joint_hack_config[WP::LEN_HACK_CONFIG],
                   const float _sensor_config[WP::LEN_SENSOR_CONFIG],
                   const float _stiffness_config[WP::LEN_STIFF_CONFIG],
                   const float _odo_config[WP::LEN_ODO_CONFIG],
                   const float _arm_config[WP::LEN_ARM_CONFIG]);

    std::string toString() const ;

private:
    WalkParameters();

public:
    float stance[WP::LEN_STANCE_CONFIG],
        step[WP::LEN_STEP_CONFIG],
        zmp[WP::LEN_ZMP_CONFIG],
        hack[WP::LEN_HACK_CONFIG],
        sensor[WP::LEN_SENSOR_CONFIG],
        stiffness[WP::LEN_STIFF_CONFIG],
        odo[WP::LEN_ODO_CONFIG],
        arm[WP::LEN_ARM_CONFIG];
};
#endif
