#ifndef AbstractGait_h_DEFINED
#define AbstractGait_h_DEFINED
#include <string>

#include "GaitConstants.h"

class AbstractGait {
public:
    AbstractGait();
    virtual ~AbstractGait();
    std::string toString() const ;

protected:
    void setGaitFromGait(const AbstractGait &other);
    void setGaitFromArrays(const float _stance_config[WP::LEN_STANCE_CONFIG],
                   const float _step_config[WP::LEN_STEP_CONFIG],
                   const float _zmp_config[WP::LEN_ZMP_CONFIG],
                   const float _joint_hack_config[WP::LEN_HACK_CONFIG],
                   const float _sensor_config[WP::LEN_SENSOR_CONFIG],
                   const float _stiffness_config[WP::LEN_STIFF_CONFIG],
                   const float _odo_config[WP::LEN_ODO_CONFIG],
                   const float _arm_config[WP::LEN_ARM_CONFIG]);


    template<const unsigned int length>
    static void addSubComponent(float target[length],
                         const float array1[length],
                         const float array2[length]);
    template<const unsigned int length>
    static void multiplySubComponent(float target[length],
                              const float source[length],
                              const float scalar);

    static void interpolateGaits(AbstractGait &targetGait,
                                 const AbstractGait &startGait,
                                 const AbstractGait &endGait,
                                 const float percentComplete);
    template<const unsigned int length>
    static void
    combineSubComponents(float target[length],
                         const float source1[length],
                         const float source2[length],
                         const float percentSwitched);

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
