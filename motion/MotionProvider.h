/**
 * The MotionProvider class is meant to be a slight abstraction of objects that
 * the Switchboard will ask to provide new joint values every motion frame.
 * These objects should have the abstract functionality of this class.
 * Example subclasses are a walk engine or a queueing engine.
 */


#ifndef _MotionProvider_h_DEFINED
#define _MotionProvider_h_DEFINED

#include <vector>
using namespace std;

#include "Kinematics.h"

class MotionProvider {
public:
    MotionProvider()
        : _active(false), nextJoints(Kinematics::NUM_CHAINS, 0) { }
    virtual ~MotionProvider() { }

    virtual void requestStop() = 0;
    const bool isActive() const { return _active; }
    virtual void calculateNextJoints() = 0;
    const vector <float> getNextJoints() { return nextJoints; }

protected:
    void setNextJoints(const vector <float> &joints) { nextJoints = joints; }
    void active() { _active = true; }
    void inactive() { _active = false; }

private:
    bool _active;
    vector <float> nextJoints;
};

#endif
