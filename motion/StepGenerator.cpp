#include "StepGenerator.h"

StepGenerator::StepGenerator()
    : zmp_ref(list<float>()){

}

const list<float>* StepGenerator::tick() {
    /*
     * Dummy code, puts ref zmp at 0 for 100 frames, and then 
     * executes a slow, sinusoidal pattern.
     */
    static int counter = 0;
    static bool firstFrame = true;
    if (firstFrame) {
        for (int i=0; i < 100; i++) {
            zmp_ref.push_back(0.0f);
        }
        firstFrame = false;
    }

    while(zmp_ref.size() < 41) {
        zmp_ref.push_back(50.0f*sin(counter*180.0f/M_PI/2000));
        counter++;
    }

    zmp_ref.pop_front(); // we want to keep moving in time
    return &zmp_ref;
}


