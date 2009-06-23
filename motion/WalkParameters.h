#ifndef _WalkParameters_h_DEFINED
#define _WalkParameters_h_DEFINED

#include <vector>

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
    LEN_JOINT_HACK_CONFIG
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

}//End namespace WP

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


public:
    std::vector<float> stance_config,
        step_config,
        zmp_config,
        joint_hack_config,
        sensor_config,
        stiffness_config,
        odo_config,
        arm_config;
};
#endif
