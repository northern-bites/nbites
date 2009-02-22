#ifndef PYMOTION_H_DEFINED
#define PYMOTION_H_DEFINED

void c_init_motion();
// C++ backend insertion (must be set before import)
//    steals a reference to the supplied MotionInterface
//    can only be called once (subsequent calls ignored)
void set_motion_interface (MotionInterface *_interface);

#endif
