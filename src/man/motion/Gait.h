#ifndef _Gait_h_DEFINED
#define _Gait_h_DEFINED

#include <vector>
#include <boost/shared_ptr.hpp>
#include "AbstractGait.h"
#include "GaitConstants.h"

class Gait : public AbstractGait {
public:
    typedef boost::shared_ptr<Gait> ptr;

    Gait(const Gait & other);
    Gait(const AbstractGait & other);
    Gait(const float _stance_config[WP::LEN_STANCE_CONFIG],
	 const float _step_config[WP::LEN_STEP_CONFIG],
	 const float _zmp_config[WP::LEN_ZMP_CONFIG],
	 const float _joint_hack_config[WP::LEN_HACK_CONFIG],
	 const float _sensor_config[WP::LEN_SENSOR_CONFIG],
	 const float _stiffness_config[WP::LEN_STIFF_CONFIG],
	 const float _odo_config[WP::LEN_ODO_CONFIG],
	 const float _arm_config[WP::LEN_ARM_CONFIG]); 

protected:
    Gait();
};


static const Gait DEFAULT_GAIT = Gait(WP::STANCE_DEFAULT,
                                      WP::STEP_DEFAULT,
                                      WP::ZMP_DEFAULT,
                                      WP::HACK_DEFAULT,
                                      WP::SENSOR_DEFAULT,
                                      WP::STIFF_DEFAULT,
                                      WP::ODO_DEFAULT,
                                      WP::ARM_DEFAULT);
#endif
