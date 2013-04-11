/**
 * The MotionProvider class is meant to be a slight abstraction of objects that
 * the Switchboard will ask to provide new joint values every motion frame.
 * These objects should have the abstract functionality of this class.
 * Example subclasses are a walk engine or a queueing engine.
 */
#pragma once

#include <vector>
#include <string>
#include "MotionCommand.h"

#include <boost/shared_ptr.hpp>

#include "Kinematics.h"

#include "InertialState.pb.h"
#include "FSR.pb.h"

namespace man
{
namespace motion
{
    enum ProviderType
    {
        SCRIPTED_PROVIDER,
        WALK_PROVIDER,
        HEAD_PROVIDER,
        NULL_PROVIDER
    };

    class MotionProvider
    {
    public:
        MotionProvider(ProviderType type)
            : _active(false), _stopping(false),
              nextJoints(Kinematics::NUM_CHAINS, std::vector<float>()),
              nextStiffnesses(Kinematics::NUM_CHAINS, std::vector<float>()),
              provider_type(type)
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
        virtual void requestStop()
        {
            if(!_stopping)
            {
                _stopping = true;
                requestStopFirstInstance();
            }
        }

        //Dangerous, can cause loss of stability
        virtual void hardReset() = 0;

        const bool isActive() const { return _active; }
        const bool isStopping() const { return _stopping; }

        virtual void calculateNextJointsAndStiffnesses(
            std::vector<float>&            sensorAngles,
            const messages::InertialState& sensorInertials,
            const messages::FSR&           sensorFSRs
            ) = 0;

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

        virtual std::vector<float> getInitialStance() { return std::vector<float>(); }

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
} // namespace motion
} // namespace man
