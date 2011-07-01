/*
 * Wrapper for all vision information needed in Python. Exposes VisualBall,
 * VisualFieldObject, VisualFieldEdge, VisualCrossbar(unused), VisualLine,
 * VisualCorner, VisualRobot, FieldLines, and the main Vision class through
 * which all of the information is accessible.
 *
 */

#ifndef PyVision_h_DEFINED
#define PyVision_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "Vision.h"

void c_init_vision();

void set_vision_pointer (boost::shared_ptr<Vision> vision_ptr);

#endif




