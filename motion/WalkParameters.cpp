#include "WalkParameters.h"
#include <assert.h>
#include <iostream>
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
    assert(joint_hack_config.size() ==  WP::LEN_HACK_CONFIG);
    assert(sensor_config.size()     ==  WP::LEN_SENSOR_CONFIG);
    assert(stiffness_config.size()  ==  WP::LEN_STIFF_CONFIG);
    assert(odo_config.size()        ==  WP::LEN_ODO_CONFIG);
    assert(arm_config.size()        ==  WP::LEN_ARM_CONFIG);

}
WalkParameters::WalkParameters(
    const float _stance_config[WP::LEN_STANCE_CONFIG],
    const float _step_config[WP::LEN_STEP_CONFIG],
    const float _zmp_config[WP::LEN_ZMP_CONFIG],
    const float _joint_hack_config[WP::LEN_HACK_CONFIG],
    const float _sensor_config[WP::LEN_SENSOR_CONFIG],
    const float _stiffness_config[WP::LEN_STIFF_CONFIG],
    const float _odo_config[WP::LEN_ODO_CONFIG],
    const float _arm_config[WP::LEN_ARM_CONFIG])
    :
    stance_config(_stance_config,&_stance_config[WP::LEN_STANCE_CONFIG]),
    step_config(_step_config,&_step_config[WP::LEN_STEP_CONFIG]),
    zmp_config(_zmp_config,&_zmp_config[WP::LEN_ZMP_CONFIG]),
    joint_hack_config(_joint_hack_config,
                      &_joint_hack_config[WP::LEN_HACK_CONFIG]),
    sensor_config(_sensor_config,&_sensor_config[WP::LEN_STIFF_CONFIG]),
    stiffness_config(_stiffness_config,
                     &_stiffness_config[WP::LEN_STIFF_CONFIG]),
    odo_config(_odo_config,&_odo_config[WP::LEN_ODO_CONFIG]),
    arm_config(_arm_config,&_arm_config[WP::LEN_ARM_CONFIG])
{}


//Default constructor - use at your own risk
WalkParameters::WalkParameters(){
    std::cout << "Danger Mr. Robinson - are you sure you want to default"
        " construct walk parameters?"<<std::endl;
}


vector<float>* WalkParameters::getWalkStance(){
    return new vector<float>(20,0.0f);

}
