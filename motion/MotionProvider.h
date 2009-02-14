/**
 * The MotionProvider class is meant to be a slight abstraction of objects that
 * the Switchboard will ask to provide new joint values every motion frame.
 * These objects should have the abstract functionality of this class.
 * Example subclasses are a walk engine or a queueing engine.
 */


#ifndef _MotionProvider_h_DEFINED
#define _MotionProvider_h_DEFINED

#include <vector>
#include "MotionCommand.h"
using namespace std;

#include "Kinematics.h"
using Kinematics::ChainID;

class MotionProvider {
public:
    MotionProvider()
        : _active(false), _stopping(false),
          nextJoints(Kinematics::NUM_CHAINS,vector<float>()) { }
    virtual ~MotionProvider() { }

    //Only pass on the first request to the extending class
    virtual void requestStop(){
        if(!_stopping){
            _stopping = true;
            requestStopFirstInstance();
        }
    }

    const bool isActive() const { return _active; }
    const bool isStopping() const {return _stopping;}
    virtual void calculateNextJoints() = 0;
    vector<float> getChainJoints(const ChainID id){return nextJoints[id];}
	virtual void  setCommand(const MotionCommand *command) =0;

protected:
    void setNextChainJoints(const ChainID id,
                            const vector <float> &chainJoints) {
        nextJoints[id] = chainJoints;
    }

    //Method that must be implemented, and called at the end of each frame
    //to set whether the provider is currently active or not
    virtual void setActive() = 0;
    //Method that must be implemented which is called only the first time
    //request stop is called
    virtual void requestStopFirstInstance() = 0;
    void active() { _active = true; }
    void inactive() { _active = false; _stopping = false; }

private:
    bool _active;
    bool _stopping;
    vector < vector <float> > nextJoints;
};

#endif
