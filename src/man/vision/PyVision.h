#ifndef PyVision_h_DEFINED
#define PyVision_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "Vision.h"

void c_init_vision();

void set_vision_pointer (boost::shared_ptr<Vision> vision_ptr);

#endif




