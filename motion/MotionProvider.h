
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

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

#include "Kinematics.h"

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
    std::vector<float> getChainJoints(const Kinematics::ChainID id) {
        return nextJoints[id];
    }
    const std::string getName(){return provider_name;}
    const ProviderType getType(){return provider_type;}
protected:
    void setNextChainJoints(const Kinematics::ChainID id,
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
