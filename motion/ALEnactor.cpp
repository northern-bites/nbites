#include "ALEnactor.h"

#include <iostream>

#ifdef NAOQI1
void ALEnactor::run() {
    std::cout << "ALEnactor::run()" << std::endl;

    mproxy->setBodyStiffness(0.85f, 0.1f);

    while (running) {

        // Get the angles we want to go to this frame from the switchboard
		vector<float> result = switchboard->getNextJoints();
 		for (int i=0; i<result.size();i++) {
 			cout << "result of joint " << i << " is " << result.at(i) << endl;
 		}
		 mproxy->post.gotoBodyAngles(result,
                                     MOTION_FRAME_LENGTH_S,
                                     AL::ALMotionProxy::INTERPOLATION_LINEAR);

        // TODO: This is probably wrong!!!!1!ONE
        // We probably want to sleep webots time and this sleeps real time.
        usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS));
    }
}
#endif//NAOQI1
