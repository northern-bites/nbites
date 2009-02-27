#ifndef PyLoc_h_DEFINED
#define PyLoc_h_DEFINED
#include <boost/shared_ptr.hpp>
void c_init_localization();
// C++ backend insertion (must be set before import)
//    can only be called once (subsequent calls ignored)
void set_mcl_reference(boost::shared_ptr<MCL> _mcl);
void set_ballEKF_reference(boost::shared_ptr<BallEKF> _ballEKF);
#endif // PyLoc_h_DEFINED
