
#include "AbstractGait.h"
#include <iostream>
#include <string.h>
using namespace std;
AbstractGait::AbstractGait(){}
AbstractGait::~AbstractGait(){}


void AbstractGait::setGaitFromArrays(
    const float _stance[WP::LEN_STANCE_CONFIG],
    const float _step[WP::LEN_STEP_CONFIG],
    const float _zmp[WP::LEN_ZMP_CONFIG],
    const float _hack[WP::LEN_HACK_CONFIG],
    const float _sensor[WP::LEN_SENSOR_CONFIG],
    const float _stiffness[WP::LEN_STIFF_CONFIG],
    const float _odo[WP::LEN_ODO_CONFIG],
    const float _arm[WP::LEN_ARM_CONFIG]){
    memcpy(stance,_stance,WP::LEN_STANCE_CONFIG*sizeof(float));
    memcpy(step,_step,WP::LEN_STEP_CONFIG*sizeof(float));
    memcpy(zmp,_zmp,WP::LEN_ZMP_CONFIG*sizeof(float));
    memcpy(hack,_hack,WP::LEN_HACK_CONFIG*sizeof(float));

    memcpy(sensor,_sensor,WP::LEN_SENSOR_CONFIG*sizeof(float));
    memcpy(stiffness,_stiffness,WP::LEN_STIFF_CONFIG*sizeof(float));
    memcpy(odo,_odo,WP::LEN_ODO_CONFIG*sizeof(float));
    memcpy(arm,_arm,WP::LEN_ARM_CONFIG*sizeof(float));

}


void AbstractGait::setGaitFromGait(const AbstractGait &other){
    setGaitFromArrays(other.stance,
                      other.step,
                      other.zmp,
                      other.hack,
                      other.sensor,
                      other.stiffness,
                      other.odo,
                      other.arm);
}

template<const unsigned int length>
void AbstractGait::addSubComponent(float target[length],
                     const float array1[length],
                     const float array2[length]){
    for(unsigned int i =0; i < length; i++){
        target[i] = array1[i] + array2[i];
    }
}

template<const unsigned int length>
void AbstractGait::multiplySubComponent(float target[length],
                                        const float source[length],
                                        const float scalar){
    for(unsigned int i =0; i < length; i++){
        target[i] =source[i]*scalar;
    }
}


void AbstractGait::interpolateGaits(AbstractGait &targetGait,
                                    const AbstractGait &startGait,
                                    const AbstractGait &endGait,
                                    const float percentComplete){

    if(percentComplete == 0.0f){
        targetGait = startGait;
        return;
    }
    if(percentComplete == 1.0f){
        targetGait = endGait;
        return;
    }
    //NOTE attributes which are interpolated COMPLETE  _should_ be
    // also stored as an attribute of the step when it is created
    // from a gait.

    const float COMPLETE = 1.0f;
    //For each component of the gait, make a new combination depending
    //on how far in the switching process we are.
    //Some gait components are associated with steps,
    //so they aren't interpolated
    combineSubComponents<WP::LEN_STANCE_CONFIG>
        (targetGait.stance,startGait.stance,endGait.stance,percentComplete);
    combineSubComponents<WP::LEN_STEP_CONFIG>
        (targetGait.step,startGait.step,endGait.step,COMPLETE);
    combineSubComponents<WP::LEN_ZMP_CONFIG>
        (targetGait.zmp,startGait.zmp,endGait.zmp,COMPLETE);
    combineSubComponents<WP::LEN_HACK_CONFIG>
        (targetGait.hack,startGait.hack,endGait.hack,percentComplete);
    combineSubComponents<WP::LEN_SENSOR_CONFIG>
        (targetGait.sensor,startGait.sensor,endGait.sensor,percentComplete);
    combineSubComponents<WP::LEN_STIFF_CONFIG>
        (targetGait.stiffness,startGait.stiffness,
         endGait.stiffness,percentComplete);
    combineSubComponents<WP::LEN_ODO_CONFIG>
        (targetGait.odo,startGait.odo,endGait.odo,percentComplete);
    combineSubComponents<WP::LEN_ARM_CONFIG>
        (targetGait.arm,startGait.arm,endGait.arm,percentComplete);
}


template<const unsigned int length>
void AbstractGait::combineSubComponents(float target[length],
                                    const float source1[length],
                                    const float source2[length],
                                    const float percentSwitched){
    float temp1[length];
    float temp2[length];


    const float source1Contribution = 1.0f - percentSwitched;
    const float source2Contribution = percentSwitched;

    // cout << " *** New recombination ** sC1,sC2 ="<<source1Contribution
    //      <<","<<source2Contribution<<endl;
    // cout << "Source1 is:"<<endl<<"   [";
    // for(unsigned int  i = 0; i <  length; i++){
    //     cout << source1[i]<<",";
    // }cout<<"]"<<endl;
    // cout << "Source2 is:"<<endl<<"   [";
    // for(unsigned int  i = 0; i <  length; i++){
    //     cout << source2[i]<<",";
    // }cout<<"]"<<endl;

    if(percentSwitched == 0.0f){
        memcpy(target,source1,sizeof(float)*length);
        return;
    }
    if(percentSwitched == 1.0f){
        memcpy(target,source2,sizeof(float)*length);
        return;
    }
    multiplySubComponent<length>(temp1,source1, source1Contribution);
    multiplySubComponent<length>(temp2,source2, source2Contribution);
    addSubComponent<length>(target, temp1, temp2);
    //     cout << "Result is:"<<endl<<"   [";
    // for(unsigned int  i = 0; i <  length; i++){
    //     cout << target[i]<<",";
    // }cout<<"]"<<endl<<endl;
}

using namespace WP;
std::string AbstractGait::toString() const {

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
