/**
 * The MotionProvider class is meant to be a slight abstraction of objects that
 * the Switchboard will ask to provide new joint values every motion frame.
 * These objects should have the abstract functionality of this class.
 * Example subclasses are a walk engine or a queueing engine.
 */


#ifndef _MotionProvider_h_DEFINED
#define _MotionProvider_h_DEFINED

#include <vector>
#include <string>
#include "MotionCommand.h"
using namespace std;

#include "Kinematics.h"
using Kinematics::ChainID;

enum ProviderType{
    SCRIPTED_PROVIDER,
    WALK_PROVIDER,
    HEAD_PROVIDER
};

class MotionProvider {
public:
    MotionProvider(ProviderType _provider_type)
        : _active(false), _stopping(false),
          nextJoints(Kinematics::NUM_CHAINS,std::vector<float>()),
          provider_type(_provider_type)
          {
              switch(provider_type){
              case SCRIPTED_PROVIDER:
                  provider_name = "ScriptedProvider";
                  break;
              case WALK_PROVIDER:
                  provider_name = "WalkingProvider";
                  break;
              case HEAD_PROVIDER:
                  provider_name = "HeadProvider";
                  break;
              }
          }
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
    std::vector<float> getChainJoints(const ChainID id){return nextJoints[id];}
	virtual void  setCommand(const MotionCommand *command) =0;
    const std::string getName(){return provider_name;}
    const ProviderType getType(){return provider_type;}
protected:
    void setNextChainJoints(const ChainID id,
                            const std::vector <float> &chainJoints) {
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
    std::vector < std::vector <float> > nextJoints;
    const ProviderType provider_type;
    std::string provider_name;

public:
    friend std::ostream& operator<< (std::ostream &o, const MotionProvider &mp)
        {
            return o << mp.provider_name;
        }
};

#endif
