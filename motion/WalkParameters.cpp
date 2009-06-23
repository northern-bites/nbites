#include "WalkParameters.h"
#include <assert.h>
#include <iostream>
using namespace std;

WalkParameters::WalkParameters(const WalkParameters & other){
    memcpy(stance,other.stance,WP::LEN_STANCE_CONFIG*sizeof(float));
    memcpy(step,other.step,WP::LEN_STEP_CONFIG*sizeof(float));
    memcpy(zmp,other.zmp,WP::LEN_ZMP_CONFIG*sizeof(float));
    memcpy(hack,other.hack,WP::LEN_HACK_CONFIG*sizeof(float));

    memcpy(sensor,other.sensor,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(stiffness,other.stiffness,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(odo,other.odo,WP::LEN_ODO_CONFIG*sizeof(float));
    memcpy(arm,other.arm,WP::LEN_ARM_CONFIG*sizeof(float));
    //std::cout << toString() <<endl;

}
WalkParameters::WalkParameters(const boost::shared_ptr<WalkParameters> other){
    memcpy(stance,other->stance,WP::LEN_STANCE_CONFIG*sizeof(float));
    memcpy(step,other->step,WP::LEN_STEP_CONFIG*sizeof(float));
    memcpy(zmp,other->zmp,WP::LEN_ZMP_CONFIG*sizeof(float));
    memcpy(hack,other->hack,WP::LEN_HACK_CONFIG*sizeof(float));

    memcpy(sensor,other->sensor,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(stiffness,other->stiffness,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(odo,other->odo,WP::LEN_ODO_CONFIG*sizeof(float));
    memcpy(arm,other->arm,WP::LEN_ARM_CONFIG*sizeof(float));
    //std::cout << toString() <<endl;

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
{
    memcpy(stance,_stance_config,WP::LEN_STANCE_CONFIG*sizeof(float));
    memcpy(step,_step_config,WP::LEN_STEP_CONFIG*sizeof(float));
    memcpy(zmp,_zmp_config,WP::LEN_ZMP_CONFIG*sizeof(float));
    memcpy(hack,_joint_hack_config,WP::LEN_HACK_CONFIG*sizeof(float));

    memcpy(sensor,_sensor_config,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(stiffness,_stiffness_config,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(odo,_odo_config,WP::LEN_ODO_CONFIG*sizeof(float));
    memcpy(arm,_arm_config,WP::LEN_ARM_CONFIG*sizeof(float));
    //std::cout << toString() <<endl;
}


//Default constructor - use at your own risk
WalkParameters::WalkParameters(){
    std::cout << "Danger Mr. Robinson - are you sure you want to default"
        " construct walk parameters?"<<std::endl;
}

using namespace WP;
std::string WalkParameters::toString() const {

    string out;
    char temp[200];

    out+="#### STANCE #####\n";
    for(int i =0; i < LEN_STIFF_CONFIG; i++){
        sprintf(temp,"%f,",stance[i]);out+=string(temp);
    }out+="\n#### STEP #####\n";

    for(int i =0; i < LEN_STEP_CONFIG; i++){
        sprintf(temp,"%f,",step[i]);out+=string(temp);
    }out+="\n#### ZMP #####\n";

    for(int i =0; i < LEN_ZMP_CONFIG; i++){
        sprintf(temp,"%f,",zmp[i]);out+=string(temp);
    }out+="\n#### HACK #####\n";

    for(int i =0; i < LEN_HACK_CONFIG; i++){
        sprintf(temp,"%f,",hack[i]);out+=string(temp);
    }out+="\n#### SENSOR #####\n";

    for(int i =0; i < LEN_SENSOR_CONFIG; i++){
        sprintf(temp,"%f,",sensor[i]);out+=string(temp);
    }out+="\n#### STIFF #####\n";

    for(int i =0; i < LEN_STIFF_CONFIG; i++){
        sprintf(temp,"%f,",stiffness[i]);out+=string(temp);
    }out+="\n#### ODO #####\n";

    for(int i =0; i < LEN_ODO_CONFIG; i++){
        sprintf(temp,"%f,",odo[i]);out+=string(temp);
    }out+="\n#### ARM #####\n";

    for(int i =0; i < LEN_ARM_CONFIG; i++){
        sprintf(temp,"%f,",arm[i]);out+=string(temp);
    }out+="\n#########\n";
    return out;
}
