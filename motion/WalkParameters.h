#ifndef _WalkParameters_h_DEFINED
#define _WalkParameters_h_DEFINED

#include <vector>
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
    STEP_DURATION,
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
    HIP_STIFF = 0,
    KP_STIFF,
    AP_STIFF,
    AR_STIFF,
    ARM_STIFF,
    LEN_STIFF_CONFIG
};

/**
 * ODO CONFIG
 * xOdoScale  -- odometry calibration for forward direction
 * yOdoScale  -- odometry calibration for lateral direction
 * thetaOdoScale  -- odometry calibration for rotational direction
 */
enum OdoConfig{
    X_ODO_SCALE = 0,
    Y_ODO_SCALE,
    Z_ODO_SCALE,
    LEN_ODO_CONFIG
};

/**
 * ARM CONFIG
 * armAmplitude -- angle amplitude of arm motion
 */
enum ArmConfig{
    ARM_AMPLITUDE = 0,
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
    WalkParameters(const std::vector<float> &_stance_config,
                   const std::vector<float> &_step_config,
                   const std::vector<float> &_zmp_config,
                   const std::vector<float> &_joint_hack_config,
                   const std::vector<float> &_sensor_config,
                   const std::vector<float> &_stiffness_config,
                   const std::vector<float> &_odo_config,
                   const std::vector<float> &_arm_config);
    WalkParameters(const float _stance_config[WP::LEN_STANCE_CONFIG],
                   const float _step_config[WP::LEN_STEP_CONFIG],
                   const float _zmp_config[WP::LEN_ZMP_CONFIG],
                   const float _joint_hack_config[WP::LEN_HACK_CONFIG],
                   const float _sensor_config[WP::LEN_SENSOR_CONFIG],
                   const float _stiffness_config[WP::LEN_STIFF_CONFIG],
                   const float _odo_config[WP::LEN_ODO_CONFIG],
                   const float _arm_config[WP::LEN_ARM_CONFIG]);
    WalkParameters();

    std::vector<float> * getWalkStance();

public:
    std::vector<float> stance,
        step,
        zmp,
        hack,
        sensor,
        stiffness,
        odo,
        arm;
//HACK:
public:
    float motion_frame_length_s;
    float bodyHeight;
    float hipOffsetX;
    float XAngleOffset;
    float stepDuration; // seconds
    float doubleSupportFraction; //Fraction of time spent in double support
    float stepHeight; // in mm
    float footLengthX;
    float dblSupInactivePercentage; // time we aren't moving zmp in dbl support
    float leftSwingHipRollAddition; //angle added to hip during single suppor
    float rightSwingHipRollAddition; // in rad
    float leftZMPSwingOffsetY; //in mm, distance to offset zmp from center of foot
    float rightZMPSwingOffsetY;//in the Y direction (side to side)
    float turnZMPOffsetY; // in mm per radians of turn
    float strafeZMPOffsetY; // in mm per mm of step->y
    float maxXSpeed;
    float maxYSpeed;
    float maxThetaSpeed;
    float sensorFeedback;
    float maxStiffness; //default stiffness
    float kneeStiffness; //knee stiffness when swinging
    float anklePitchStiffness; //ankle pitch stiffness
    float ankleRollStiffness; //ankle roll stiffnesses
    float armStiffness;
    float xOdoScale;
    float yOdoScale;
    float thetaOdoScale;
    float armAmplitude;   //amount, in rads, that the arms should swing
//calculated from parameters
public:
    int stepDurationFrames; //one double + one single support
    int doubleSupportFrames; //num frames to spend in double support
    int singleSupportFrames; //num frames to spend in single support
};
#endif
