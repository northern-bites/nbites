#ifndef GaitConstants_h_DEFINED
#define GaitConstants_h_DEFINED
#include "NBMath.h"


/**
 * Checklist of things you need to update when adding an attribute to a
 * pre-existing Config:
 *IN THIS FILE:
 *  - Add it to the enum. make sure to leave the LEN_XXX_CONFIG as the last
 *    item in the enum
 *  - Add the correct python->c++ conversion constant (e.g. LENGTH,ANGLE,NONE)
 *  - Add a reasonable default to initialize C instances incase walking is
 *    called without getting Gaits sent from Python first.
 *IN ROBOTGAITS
 *  - Fix the tuple lengths to match correctly
 */

/**
 * Checklist of things you need to update when adding a new config:
 *  - Add the enum, the conversion, and the default arrays in this file.
 *  - Update AbstractGait.h/cpp to make sure the constructors and element
 *    copying will include the new array associated with your config
 *  - Maybe other things
 */

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
        TRANS_TIME,
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
        STEP_HEIGHT, //TODO move this to STANCE
		FOOT_LIFT_ANGLE,
        MAX_VEL_X,
        MIN_VEL_X,
        MAX_VEL_Y,
        MAX_VEL_THETA,
        MAX_ACC_X,
        MAX_ACC_Y,
        MAX_ACC_THETA,
		WALKING, //1.0 is walking, everything else is not walking.
        LEN_STEP_CONFIG
    };

    static const float NON_WALKING_GAIT = 0.0f;
    static const float WALKING_GAIT = 1.0f;

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
        FEEDBACK_TYPE=0, //This is a bit bad, since we don't want to interpolate
        GAMMA_X,
        GAMMA_Y,
        SPRING_K_X,
        SPRING_K_Y,
        MAX_ANGLE_X,
        MAX_ANGLE_Y,
        MAX_ANGLE_VEL,
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
        ARM_PITCH,
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

    /**
     * CONVERSION CONSTANTS
     *   - How to convert from Python (deg/cm) to C++ (rad/mm)
     */
    static const float NONE = 1.0f;
    static const float LENGTH = CM_TO_MM;
    static const float ANGLE = TO_RAD;

    static const float STANCE_CONVERSION[LEN_STANCE_CONFIG]=
    {LENGTH,//com height
     LENGTH,//x off
     LENGTH,//Y separation
     ANGLE,//angleY
     ANGLE,//foot angle
     NONE}; //Time to transition to/from new stance
    static const float STEP_CONVERSION[LEN_STEP_CONFIG]=
    {NONE,//step time
     NONE,//dblSupFrac
     LENGTH,//step height
     ANGLE, //foot lift angle
     LENGTH,//max forward vel x
     LENGTH,//max backward vel x
     LENGTH,//max vel y
     ANGLE,//max vel t
     LENGTH,//max acc x
     LENGTH,//max acc y
     ANGLE,//max acc t
     NONE};//Walking or not
    static const float ZMP_CONVERSION[LEN_ZMP_CONFIG]=
    {LENGTH,//foot center
     NONE,//zmp static perc
     LENGTH,//l zmp off
     LENGTH,//r zmp off
     NONE,//strafe zmp off
     NONE,};//turn zmp off
    static const float HACK_CONVERSION[LEN_HACK_CONFIG]=
    {ANGLE,//hip hack l
     ANGLE};//hip hack r
    static const float SENSOR_CONVERSION[LEN_SENSOR_CONFIG]=
    {NONE,//Observer scale
     NONE,//gX
     NONE,//gy
     NONE,//kx
     NONE,//ky
     ANGLE,
     ANGLE,
     ANGLE};//angle vel
    static const float STIFF_CONVERSION[LEN_STIFF_CONFIG]=
    {NONE,//hip
     NONE,//knee
     NONE,//ap
     NONE,//ar
	 NONE,//arms
     NONE};//arm pitch
    static const float ODO_CONVERSION[LEN_ODO_CONFIG]=
    {NONE,//xodoscale
     NONE,//yodoscale
     NONE};//thetaodoscale
    static const float ARM_CONVERSION[LEN_ARM_CONFIG]=
    {ANGLE};//arm amplitude


/**
 * GAIT DEFAULTS. These should normally be sent from Python, but just in case
 * they are defined here in order to avoid breaking any robots.
 */
    static const float STANCE_DEFAULT[LEN_STANCE_CONFIG]=
    {310.0f,//com height
     14.5f,//x off
     100.0f,//Y separation
     0.05f,//angleY
     0.0f,//foot angle
     0.1f};//transition time
    static const float STEP_DEFAULT[LEN_STEP_CONFIG]=
    {0.4f,//step time
     0.25f,//dblSupFrac
     9.0f,//step height
     0.0f,//lift angle
     70.0f,//max forward vel x
     -50.0f,//max backward vel x
     70.0f,//max vel y
     0.35f,//max vel t
     70.0f,//max acc x
     70.0f,//max acc y
     0.35f,//max acc t
     WALKING_GAIT};//Walking or not
    static const float ZMP_DEFAULT[LEN_ZMP_CONFIG]=
    {0.0f,//foot center
     0.4f,//zmp static perc
     4.0f,//l zmp off
     4.0f,//r zmp off
     0.01f,//strafe zmp off
     6.6f,};//turn zmp off
    static const float HACK_DEFAULT[LEN_HACK_CONFIG]=
    {0.1f,//hip hack l
     0.1f};//hip hack r
    static const float SENSOR_DEFAULT[LEN_SENSOR_CONFIG]=
    {0.0f,//Observer scale
     0.0f,//GX
     0.0f,//GY
     0.0f,//KX
     0.0f,//KY
     0.0f,//MAXVELX
     0.0f,//MAXVELY
     0.0f};//angle xy scale
    static const float STIFF_DEFAULT[LEN_STIFF_CONFIG]=
    {0.85f,//hip
     0.3f,//knee
     0.4f,//ap
     0.3f,//ar
     0.2f,//arm
	 0.2f};//arm pitch
    static const float ODO_DEFAULT[LEN_ODO_CONFIG]=
    {1.0f,//xodoscale
     1.0f,//yodoscale
     1.0f};//thetaodoscale
    static const float ARM_DEFAULT[LEN_ARM_CONFIG]=
    {0.0f};//arm amplitude

};//End namespace WP
#endif
