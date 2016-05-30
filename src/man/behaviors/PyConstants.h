// Wrapper for PyNogginConstants.h.

#ifndef PyConstants_h_DEFINED
#define PyConstants_h_DEFINED

#include "PyNogginConstants.h"
#include "DebugConfig.h"
#include "../vision/Hough.h"
#include "../../share/logshare/SExpr.h"

using nbl::SExpr;

void c_init_noggin_constants();
void get_config_params();
bool get_defensive_strategy(SExpr params);
bool get_defender_home(SExpr params);
float get_max_speed(SExpr params);
float get_min_speed(SExpr params);

#endif //PyConstants_h_DEFINED
