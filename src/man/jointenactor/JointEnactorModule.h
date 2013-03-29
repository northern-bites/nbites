/**
 * @brief The joint enactor module communicates synchronously with the
 *        DCM to control the angles and stiffnesses of the joint
 *        actuators.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/dcmproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alerror/alerror.h>
#include <althread/alprocesssignals.h>

#include <string>
#include <iostream>

#include "../sensors/SensorTypes.h"
#include "../newmotion/MotionConstants.h"
#include "../newmotion/Kinematics.h"
#include "RoboGrams.h"

#include "JointAngles.pb.h"

namespace man
{
    namespace jointenactor
    {
    /**
     * @class JointEnactorModule
     */
    class JointEnactorModule : public portals::Module
    {
    public:
        JointEnactorModule(boost::shared_ptr<AL::ALBroker> broker);

        virtual ~JointEnactorModule();

        /**
         * @brief Enables motion (joint angle) commands to be sent
         *        to the DCM.
         */
        void enableMotion();

        /**
         * @brief Disables motion (joint angle) commands from being
         *        sent to the DCM. Note that motion is initially
         *        disabled.
         */
        void disableMotion();

        /**
         * @brief Sets variable saying DCM has gotten most recent joints
         */
        void newJoints();
        void jointsInDCM();



        // Allows clients to set the stiffnesses of all joints.
        portals::InPortal<messages::JointAngles> stiffnessInput_;

        // Accepts motion commands (i.e. joint angles.)
        portals::InPortal<messages::JointAngles> jointsInput_;

    private:
        /**
         * @brief Gets a proxy to the DCM, does necessary initialization,
         *        and connects synchronous callback to the DCM loop.
         */
        void start();

        /**
         * @brief Do initialization of joint and stiffness aliases
         *        before connecting to the DCM.
         */
        void initialize();

        void connectToDCMLoop();

        /**
         * @brief Synchronously called before each iteration of the
         *        DCM loop. All operations must be fast, so as not
         *        to slow down the DCM (e.g., no system calls or
         *        dynamic memory allocation.)
         */
        void DCMPreProcessCallback();

        /**
         * @brief Disconnects from the DCM loop.
         */
        void stop();

        void run_();

        boost::shared_ptr<AL::ALBroker> broker_;
        boost::shared_ptr<AL::DCMProxy> dcmProxy_;

        bool motionEnabled_;
        bool newJoints;

        AL::ALValue jointCommand_;
        AL::ALValue stiffnessCommand_;
        AL::ALProcessSignals::ProcessSignalConnection dcmPreProcessConnection_;

        messages::JointAngles latestJointAngles_;
        messages::JointAngles latestStiffness_;
    };
    } // namespace jointenactor
} // namespace man
