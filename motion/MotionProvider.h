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
using Kinematics::ChainID;

class MotionProvider {
public:
    MotionProvider()
        : _active(false), nextJoints(Kinematics::NUM_CHAINS,vector<float>()) { }
    virtual ~MotionProvider() { }

    virtual void requestStop() = 0;
    const bool isActive() const { return _active; }
    virtual void calculateNextJoints() = 0;
    vector<float> getChainJoints(const ChainID id){return nextJoints[id];}

protected:
    void setNextChainJoints(const ChainID id,
                            const vector <float> &chainJoints) {
		std::cout << "chain " << id << " at(0) is " << chainJoints.at(0) << endl;
        nextJoints[id] = chainJoints;
    }

    void active() { _active = true; }
    void inactive() { _active = false; }

private:
    bool _active;
    vector < vector <float> > nextJoints;
};

#endif
