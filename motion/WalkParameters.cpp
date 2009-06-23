#include "WalkParameters.h"
#include <assert.h>

using namespace std;

WalkParameters::WalkParameters(const vector<float> &_stance_config,
                                     const vector<float> &_step_config,
                                     const vector<float> &_zmp_config,
                                     const vector<float> &_joint_hack_config,
                                     const vector<float> &_sensor_config,
                                     const vector<float> &_stiffness_config,
                                     const vector<float> &_odo_config,
                                     const vector<float> &_arm_config)
    :stance_config(_stance_config),
     step_config(_step_config),
     zmp_config(_zmp_config),
     joint_hack_config(_joint_hack_config),
     sensor_config(_sensor_config),
     stiffness_config(_stiffness_config),
     odo_config(_odo_config),
     arm_config(_arm_config)
{
    //Double check that the lengths of the vectors are correct
    assert(stance_config.size()     ==  WP::LEN_STANCE_CONFIG);
    assert(step_config.size()       ==  WP::LEN_STEP_CONFIG);
    assert(zmp_config.size()        ==  WP::LEN_ZMP_CONFIG);
    assert(joint_hack_config.size() ==  WP::LEN_JOINT_HACK_CONFIG);
    assert(sensor_config.size()     ==  WP::LEN_SENSOR_CONFIG);
    assert(stiffness_config.size()  ==  WP::LEN_STIFF_CONFIG);
    assert(odo_config.size()        ==  WP::LEN_ODO_CONFIG);
    assert(arm_config.size()        ==  WP::LEN_ARM_CONFIG);

}

