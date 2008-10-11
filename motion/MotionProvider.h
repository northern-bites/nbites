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

class MotionProvider {
public:
    MotionProvider() { }
    virtual ~MotionProvider() { }

    virtual void requestStop() = 0;
    bool isActive() const { return active; }
    virtual void calculateNextJoints() = 0;
    const vector <float> getNextJoints() { return nextJoints; }

protected:
    void setNextJoints(const vector <float> &joints) { nextJoints = joints; }
    void active() { active = true; }
    void inactive() { active = false; }

private:
    bool active;
    vector <float> nextJoints;
};

#endif
