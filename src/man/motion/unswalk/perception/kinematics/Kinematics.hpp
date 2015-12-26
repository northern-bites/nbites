#pragma once

#include <utility>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include <perception/kinematics/Parameters.hpp>
#include <perception/kinematics/Pose.hpp>
#include <perception/vision/Camera.hpp>

#include <utils/matrix_helpers.hpp>
#include <types/RRCoord.hpp>
#include <types/JointValues.hpp>
#include <types/SensorValues.hpp>

#include "CKF.hpp"

#define CAMERA_DH_CHAIN_LEN 12
#define HEAD_DH_CHAIN_LEN 3
#define ARM_DH_CHAIN_LEN 14
#define LEG_DH_CHAIN_LEN 19

class UNSWKinematics {
   public:
      friend class KinematicsAdapter;
      friend class KinematicsCalibrationSkill;
      friend class CameraPoseTab;
      UNSWKinematics();

      enum Link {
         FOOT = 0,
         BODY = 8,
         CAMERA = 12,
         NECK = 9
      };

      enum Chain {
         LEFT_CHAIN = 0,
         RIGHT_CHAIN = 1
      };

      /* Creates a Pose with the current evaluated DH Chain */
      Pose getPose();

      void updateDHChain();

      boost::numeric::ublas::matrix<float>
      evaluateDHChain(Link from, Link to, Chain foot, bool top = true);

      boost::numeric::ublas::matrix<float> evaluateMassChain();

      boost::numeric::ublas::matrix<float>
      createBodyToFootOnGroundTransform(Chain foot, boost::numeric::ublas::matrix<float> b2f);

      boost::numeric::ublas::matrix<float>
      createCameraToFootTransform(Chain foot, bool top);
      
      boost::numeric::ublas::matrix<float>
      createNeckToFootTransform(Chain foot);

      boost::numeric::ublas::matrix<float>
      createFootToWorldTransform(Chain foot, bool top = true);

      boost::numeric::ublas::matrix<float>
      createCameraToWorldTransform(Chain foot, bool top);
      
      boost::numeric::ublas::matrix<float>
      createNeckToWorldTransform(Chain foot);

      boost::numeric::ublas::matrix<float>
      createWorldToFOVTransform(
         const boost::numeric::ublas::matrix<float> &m);

      boost::numeric::ublas::matrix<float>
      fovToImageSpaceTransform(
         const boost::numeric::ublas::matrix<float> &transform,
         const boost::numeric::ublas::matrix<float> &point, bool top);

      void determineBodyExclusionArray(
         const boost::numeric::ublas::matrix<float> &m,
         int16_t *points, bool top);

      Chain determineSupportChain();

      void setSensorValues(UNSWSensorValues sensorValues);

      std::pair<int, int> calculateHorizon(
         const boost::numeric::ublas::matrix<float> &m);

      Parameters<float> parameters;
   private:
      // CKF is not really tuned and not being used at the moment
      //CKF ckf;
      UNSWSensorValues sensorValues;
      Chain supportChain;

      boost::numeric::ublas::matrix<float> transformLTop[CAMERA_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformLBot[CAMERA_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformRTop[CAMERA_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformRBot[CAMERA_DH_CHAIN_LEN];
      
      boost::numeric::ublas::matrix<float> cameraPanInverseHack;

      // DH matrices for mass
      boost::numeric::ublas::matrix<float> transformHB[HEAD_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformRAB[ARM_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformLAB[ARM_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformRFB[LEG_DH_CHAIN_LEN];
      boost::numeric::ublas::matrix<float> transformLFB[LEG_DH_CHAIN_LEN];

      // Contains the masses and centre position of each joint
      std::vector<float> masses;
      std::vector<boost::numeric::ublas::matrix<float> > massesCom;

      //Parameters<float> parameters; making public

      std::vector<std::vector<boost::numeric::ublas::matrix<float> > >
      bodyParts;
};

