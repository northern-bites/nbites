#pragma once

#include "Kinematics.h"
#include "PMotion.pb.h"
#include "JointNames.h"

namespace man
{
    namespace motion
    {
        enum SupportFoot 
        {
            LEFT_SUPPORT = 0,
            RIGHT_SUPPORT
        };

        namespace MotionConstants {

            enum MotionType {
                WALK = 0,
                STEP,
                DESTINATION,
                KICK,
                BODY_JOINT,
                HEAD_JOINT,
                GAIT,
                SET_HEAD,
                COORD_HEAD,
                STIFFNESS,
                FREEZE,
                UNFREEZE
            };

            const static unsigned int WALK_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
            const static unsigned int STEP_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
            const static unsigned int BODY_JOINT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
            const static unsigned int HEAD_JOINT_NUM_CHAINS = 1;
            const static unsigned int GAIT_NUM_CHAINS = Kinematics::NUM_BODY_CHAINS;
            const static unsigned int STIFFNESS_NUM_CHAINS = Kinematics::NUM_CHAINS;
            const static unsigned int FREEZE_NUM_CHAINS = Kinematics::NUM_CHAINS;


            const static int WALK_CHAINS[WALK_NUM_CHAINS] =
            {Kinematics::LARM_CHAIN,
             Kinematics::LLEG_CHAIN,
             Kinematics::RLEG_CHAIN,
             Kinematics::RARM_CHAIN };

            const static int STEP_CHAINS[STEP_NUM_CHAINS] =
            {Kinematics::LARM_CHAIN,
             Kinematics::LLEG_CHAIN,
             Kinematics::RLEG_CHAIN,
             Kinematics::RARM_CHAIN };

            const static int GAIT_CHAINS[GAIT_NUM_CHAINS] =
            { Kinematics::LARM_CHAIN,
              Kinematics::LLEG_CHAIN,
              Kinematics::RLEG_CHAIN,
              Kinematics::RARM_CHAIN };

            const static int BODY_JOINT_CHAINS[BODY_JOINT_NUM_CHAINS] =
            { Kinematics::LARM_CHAIN,
              Kinematics::LLEG_CHAIN,
              Kinematics::RLEG_CHAIN,
              Kinematics::RARM_CHAIN };

            const static int HEAD_JOINT_CHAINS[HEAD_JOINT_NUM_CHAINS] =
            { Kinematics::HEAD_CHAIN };

            const static int STIFFNESS_CHAINS[STIFFNESS_NUM_CHAINS] =
            { Kinematics::HEAD_CHAIN,
              Kinematics::LARM_CHAIN,
              Kinematics::LLEG_CHAIN,
              Kinematics::RLEG_CHAIN,
              Kinematics::RARM_CHAIN };

            const static int FREEZE_CHAINS[FREEZE_NUM_CHAINS] =
            { Kinematics::HEAD_CHAIN,
              Kinematics::LARM_CHAIN,
              Kinematics::LLEG_CHAIN,
              Kinematics::RLEG_CHAIN,
              Kinematics::RARM_CHAIN };

            static const float DEFAULT_ON_STIFFNESS = 0.85f;
            static const float DEFAULT_OFF_STIFFNESS = -1.0f;
            static const float MAX_STIFFNESS = 1.0f;
            static const float MIN_STIFFNESS = 0.0f;
            static const float NO_STIFFNESS = -1.0f;

            static const bool null_head_mask[Kinematics::NUM_CHAINS] =
            {true, false, false, false, false};
            static const bool null_body_mask[Kinematics::NUM_CHAINS] =
            {false, true, true, true, true};

            static const float SIT_DOWN_ANGLES[] =
            { 0.0f, 0.0f,
              1.57f, 0.0f, -1.13f, -1.0f,
              0.0f, 0.0f, -0.96f,  2.18f,
              -1.22f, 0.0f,  0.0f,   0.0f,
              -0.96f, 2.18f,-1.22f,  0.0f,
              1.57f, 0.0f,  1.13f,  1.01f};

            static const std::vector<float> OFF_STIFFNESSES(
                Kinematics::NUM_JOINTS,
                MotionConstants::DEFAULT_OFF_STIFFNESS);
        } // namespace MotionConstants
    } // namespace motion
} // namespace man
