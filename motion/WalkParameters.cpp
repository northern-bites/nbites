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
    :stance(_stance_config),
     step(_step_config),
     zmp(_zmp_config),
     hack(_joint_hack_config),
     sensor(_sensor_config),
     stiffness(_stiffness_config),
     odo(_odo_config),
     arm(_arm_config)
{
    //Double check that the lengths of the vectors are correct
    assert(stance.size()     ==  WP::LEN_STANCE_CONFIG);
    assert(step.size()       ==  WP::LEN_STEP_CONFIG);
    assert(zmp.size()        ==  WP::LEN_ZMP_CONFIG);
    assert(hack.size() ==  WP::LEN_HACK_CONFIG);
    assert(sensor.size()     ==  WP::LEN_SENSOR_CONFIG);
    assert(stiffness.size()  ==  WP::LEN_STIFF_CONFIG);
    assert(odo.size()        ==  WP::LEN_ODO_CONFIG);
    assert(arm.size()        ==  WP::LEN_ARM_CONFIG);

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
    stance(_stance_config,&_stance_config[WP::LEN_STANCE_CONFIG]),
    step(_step_config,&_step_config[WP::LEN_STEP_CONFIG]),
    zmp(_zmp_config,&_zmp_config[WP::LEN_ZMP_CONFIG]),
    hack(_joint_hack_config,
                      &_joint_hack_config[WP::LEN_HACK_CONFIG]),
    sensor(_sensor_config,&_sensor_config[WP::LEN_STIFF_CONFIG]),
    stiffness(_stiffness_config,
                     &_stiffness_config[WP::LEN_STIFF_CONFIG]),
    odo(_odo_config,&_odo_config[WP::LEN_ODO_CONFIG]),
    arm(_arm_config,&_arm_config[WP::LEN_ARM_CONFIG])
{}


//Default constructor - use at your own risk
WalkParameters::WalkParameters(){
    std::cout << "Danger Mr. Robinson - are you sure you want to default"
        " construct walk parameters?"<<std::endl;
}


vector<float>* WalkParameters::getWalkStance(){
    return new vector<float>(20,0.0f);

}
