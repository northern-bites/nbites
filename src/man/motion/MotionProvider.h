
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

#include <boost/shared_ptr.hpp>
#include "Profiler.h"

#include "Kinematics.h"
#include "Sensors.h"           // for SupportFoot enum

enum ProviderType{
    SCRIPTED_PROVIDER,
    WALK_PROVIDER,
    HEAD_PROVIDER,
    NULL_PROVIDER
};

class MotionProvider {
public:
    MotionProvider(ProviderType _provider_type,
				   boost::shared_ptr<Profiler> p)
        : profiler(p),_active(false), _stopping(false),
          nextJoints(Kinematics::NUM_CHAINS,std::vector<float>()),
          nextStiffnesses(Kinematics::NUM_CHAINS,std::vector<float>()),
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
              case NULL_PROVIDER:
                  provider_name = "NullProvider";
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

    //Dangerous, can cause loss of stability
    virtual void hardReset() =0 ;

    const bool isActive() const { return _active; }
    const bool isStopping() const {return _stopping;}
    virtual void calculateNextJointsAndStiffnesses() = 0;
    std::vector<float> getChainJoints(const Kinematics::ChainID id) {
        if(nextJoints[id].size() != Kinematics::chain_lengths[id]){
            std::cout << "Getting joints in " << *this <<" and the length of the "
                 <<id<<"th stiffness vector is "<<nextJoints[id].size()
                      <<" not " <<Kinematics::chain_lengths[id]<<std::endl;
        }
        return nextJoints[id];
    }
    std::vector<float> getChainStiffnesses(const Kinematics::ChainID id){
        if(nextStiffnesses[id].size() != Kinematics::chain_lengths[id]){
            std::cout << "Getting stiffness in " << *this <<" and the length of the "
                 <<id<<"th stiffness vector is "<<nextStiffnesses[id].size()
                      <<" not " <<Kinematics::chain_lengths[id]<<std::endl;
        }
        return nextStiffnesses[id];
    }
    const std::string getName() const {return provider_name;}
    const ProviderType getType() const {return provider_type;}

    virtual const SupportFoot getSupportFoot() const {
        // the default return value for all providers is left foot
        // some providers which can supply more meaningful info, like the
        // walk engine, override this method.
        // TODO: technically, the head provider (which extends this class)
        //       should have no idea what the current support foot is.
        //       As a consolation, you should know that it will never be asked
        //       about it anyway.
        return LEFT_SUPPORT;
    }

protected:
    void setNextChainJoints(const Kinematics::ChainID id,
                            const std::vector <float> &chainJoints) {
        nextJoints[id] = chainJoints;
    }

    void setNextChainStiffnesses(const Kinematics::ChainID id,
				 const std::vector <float> &chainJoints) {
        nextStiffnesses[id] = chainJoints;
    }

    //Method that must be implemented, and called at the end of each frame
    //to set whether the provider is currently active or not
    virtual void setActive() = 0;
    //Method that must be implemented which is called only the first time
    //request stop is called
    virtual void requestStopFirstInstance() = 0;
    void active() { _active = true; }
    void inactive() { _active = false; _stopping = false; }

protected:
	boost::shared_ptr<Profiler> profiler;

private:

    bool _active;
    bool _stopping;
    std::vector < std::vector <float> > nextJoints;
    std::vector < std::vector <float> > nextStiffnesses;

    const ProviderType provider_type;
    std::string provider_name;

public:
    friend std::ostream& operator<< (std::ostream &o, const MotionProvider &mp)
        {
            return o << mp.provider_name;
        }
};

#endif
