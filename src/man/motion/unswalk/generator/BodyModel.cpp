#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include "BodyModel.hpp"
#include "Generator.hpp"
#include "utils/body.hpp"
#include "utils/matrix_helpers.hpp"

const float BodyModel::COM_HEIGHT = 200;
const float BodyModel::MOTION_DT = 1 / 100.0f;
const float BodyModel::FOOT_LENGTH = 160;
const float BodyModel::L_RATE = 0.5;

BodyModel::BodyModel() : filAccX(0.0),
                         filAccY(0.0),
                         filteredTotalPressure(0.0),
                         filHighZMPF(0.0),
                         filLowZMPF(0.0),
                         filZMPL(0.0)
{
   stepCounter = 0;
   sagittalAcceleration = 0.0;
   forwardR = forwardL = 0.0;

   isLeftPhase = false;
   isOnFront = false;
   lastIsLeftPhase = false;

   // real body model
   isStopped = false;

   //Used to AutoCalibrate Footsensors
	fsLfr = fsLfl = fsLrr = fsLrl = 0.1;                   // keeps maximum foot sensor readings
	fsRfr = fsRfl = fsRrr = fsRrl = 0.1;                   // small value deemed close to zero initially
}

void BodyModel::update(Odometry *odometry,
                       const SensorValues &sensors) {
   // Various Center of Pressure calcs to help stabilise walk (ZMP)
   stepCounter++;
   // Update maximum sensed foot-sensor readings
	float temp = sensors.sensors[Sensors::LFoot_FSR_FrontLeft]; if(fsLfl<temp and fsLfl<5.0) fsLfl = temp;
	temp = sensors.sensors[Sensors::LFoot_FSR_FrontLeft];       if(fsLfl<temp and fsLfl<5.0) fsLfl = temp;
	temp = sensors.sensors[Sensors::LFoot_FSR_FrontRight];      if(fsLfr<temp and fsLfr<5.0) fsLfr = temp;
	temp = sensors.sensors[Sensors::LFoot_FSR_RearLeft];        if(fsLrl<temp and fsLrl<5.0) fsLrl = temp;
	temp = sensors.sensors[Sensors::LFoot_FSR_RearRight];       if(fsLrr<temp and fsLrr<5.0) fsLrr = temp;
	temp = sensors.sensors[Sensors::RFoot_FSR_FrontLeft];       if(fsRfl<temp and fsRfl<5.0) fsRfl = temp;
	temp = sensors.sensors[Sensors::RFoot_FSR_FrontRight];      if(fsRfr<temp and fsRfr<5.0) fsRfr = temp;
	temp = sensors.sensors[Sensors::RFoot_FSR_RearLeft];        if(fsRrl<temp and fsRrl<5.0) fsRrl = temp;
	temp = sensors.sensors[Sensors::RFoot_FSR_RearRight];       if(fsRrr<temp and fsRrr<5.0) fsRrr = temp;
	lastZMPL = ZMPL;
	ZMPL = 0;
	// Calculate ZMPL (left-right) used to eg switch support foot in Walk2014
	float pressureL =
	  +sensors.sensors[Sensors::LFoot_FSR_FrontLeft]/fsLfl
	  + sensors.sensors[Sensors::LFoot_FSR_FrontRight]/fsLfr
	  + sensors.sensors[Sensors::LFoot_FSR_RearLeft]/fsLrl
	  + sensors.sensors[Sensors::LFoot_FSR_RearRight]/fsLrr;
	float pressureR =
	  +sensors.sensors[Sensors::RFoot_FSR_FrontLeft]/fsRfl
	  + sensors.sensors[Sensors::RFoot_FSR_FrontRight]/fsRfr
	  + sensors.sensors[Sensors::RFoot_FSR_RearLeft]/fsRrl
	  + sensors.sensors[Sensors::RFoot_FSR_RearRight]/fsRrr;
	float totalPressure = pressureL + pressureR;
	if (ABS(totalPressure) > 0.000001f) {
			ZMPL =
			(  .080 * sensors.sensors[Sensors::LFoot_FSR_FrontLeft]/fsLfl
			+ .030 * sensors.sensors[Sensors::LFoot_FSR_FrontRight]/fsLfr
			+ .080 * sensors.sensors[Sensors::LFoot_FSR_RearLeft]/fsLrl
			+ .030 * sensors.sensors[Sensors::LFoot_FSR_RearRight]/fsLrr
			- .030 * sensors.sensors[Sensors::RFoot_FSR_FrontLeft]/fsRfl
			- .080 * sensors.sensors[Sensors::RFoot_FSR_FrontRight]/fsRfr
			- .030 * sensors.sensors[Sensors::RFoot_FSR_RearLeft]/fsRrl
			- .080 * sensors.sensors[Sensors::RFoot_FSR_RearRight]/fsRrr) / totalPressure;
		}

   isOnFront = isOnFrontOfFoot(sensors);
   processUpdate(odometry, sensors);
   observationUpdate(odometry, sensors);
}


bool BodyModel::isFootOnGround(const SensorValues &sensors) {
   float leftFrontL = sensors.sensors[Sensors::LFoot_FSR_FrontLeft];
   float leftFrontR = sensors.sensors[Sensors::LFoot_FSR_FrontRight];
   float rightFrontL = sensors.sensors[Sensors::RFoot_FSR_FrontLeft];
   float rightFrontR = sensors.sensors[Sensors::RFoot_FSR_FrontRight];
   float leftRearL  = sensors.sensors[Sensors::LFoot_FSR_RearLeft];
   float leftRearR  = sensors.sensors[Sensors::LFoot_FSR_RearRight];
   float rightRearL = sensors.sensors[Sensors::RFoot_FSR_RearLeft];
   float rightRearR = sensors.sensors[Sensors::RFoot_FSR_RearRight];
   if (isLeftPhase) {
      return rightFrontL > 0.01 && rightFrontR > 0.01 &&
             rightRearL > 0.01 && rightRearR > 0.01;
   }
   return leftFrontL > 0.01 &&  leftFrontR > 0.01 &&
          leftRearL > 0.01 && leftRearR > 0.01;
}

bool BodyModel::isOnFrontOfFoot(const SensorValues &sensors) {
   float leftFront = sensors.sensors[Sensors::LFoot_FSR_FrontLeft] +
                     sensors.sensors[Sensors::LFoot_FSR_FrontRight];
   float rightFront = sensors.sensors[Sensors::RFoot_FSR_FrontLeft] +
                      sensors.sensors[Sensors::RFoot_FSR_FrontRight];
   float leftRear  = sensors.sensors[Sensors::LFoot_FSR_RearLeft] +
                     sensors.sensors[Sensors::LFoot_FSR_RearRight];
   float rightRear = sensors.sensors[Sensors::RFoot_FSR_RearLeft] +
                     sensors.sensors[Sensors::RFoot_FSR_RearRight];
   if (isLeftPhase) {
      return rightFront > rightRear;
   }
   return leftFront > leftRear;
}

float BodyModel::getFootZMP(bool isLeft, const SensorValues &sensors) {
   if (walkCycle.isDoubleSupportPhase()) return 0;
   float ZMPF = 0;
   float totalPressure = 0;
   if (isLeft) {
      totalPressure += sensors.sensors[Sensors::LFoot_FSR_FrontLeft]
                         + sensors.sensors[Sensors::LFoot_FSR_FrontRight]
                         + sensors.sensors[Sensors::LFoot_FSR_RearLeft]
                         + sensors.sensors[Sensors::LFoot_FSR_RearRight];
   } else {
      totalPressure += sensors.sensors[Sensors::RFoot_FSR_FrontLeft]
                         + sensors.sensors[Sensors::RFoot_FSR_FrontRight]
                         + sensors.sensors[Sensors::RFoot_FSR_RearLeft]
                         + sensors.sensors[Sensors::RFoot_FSR_RearRight];
   }
   if (isLeft) {
      ZMPF =  
            (75.0f) * sensors.sensors[Sensors::LFoot_FSR_FrontLeft]
          + (-45.0f) * sensors.sensors[Sensors::LFoot_FSR_RearLeft]
          + (75.0f) * sensors.sensors[Sensors::LFoot_FSR_FrontRight]
          + (-45.0f) * sensors.sensors[Sensors::LFoot_FSR_RearRight];
   } else {   
      ZMPF = 
           (75.0f) * sensors.sensors[Sensors::RFoot_FSR_FrontRight]
         + (-45.0f) * sensors.sensors[Sensors::RFoot_FSR_RearRight]
         + (75.0f) * sensors.sensors[Sensors::RFoot_FSR_FrontLeft]
         + (-45.0f) * sensors.sensors[Sensors::RFoot_FSR_RearLeft];
   }
   if (totalPressure == 0) return  0;
   ZMPF /= totalPressure;
   return ZMPF;
}

float BodyModel::getHorizontalFootZMP(bool isLeft, const SensorValues &sensors) {
   float ZMPF = 0;
   float totalPressure = 0;
   if (isLeft) {
      totalPressure += sensors.sensors[Sensors::LFoot_FSR_FrontLeft]
                         + sensors.sensors[Sensors::LFoot_FSR_FrontRight]
                         + sensors.sensors[Sensors::LFoot_FSR_RearLeft]
                         + sensors.sensors[Sensors::LFoot_FSR_RearRight];
   } else {
      totalPressure += sensors.sensors[Sensors::RFoot_FSR_FrontLeft]
                         + sensors.sensors[Sensors::RFoot_FSR_FrontRight]
                         + sensors.sensors[Sensors::RFoot_FSR_RearLeft]
                         + sensors.sensors[Sensors::RFoot_FSR_RearRight];
   }
   if (isLeft) {
      ZMPF =  
            (20.0f) * sensors.sensors[Sensors::LFoot_FSR_FrontLeft]
          + (20.0f) * sensors.sensors[Sensors::LFoot_FSR_RearLeft]
          + (-20.0f) * sensors.sensors[Sensors::LFoot_FSR_FrontRight]
          + (-20.0f) * sensors.sensors[Sensors::LFoot_FSR_RearRight];
   } else {   
      ZMPF = 
           (-20.0f) * sensors.sensors[Sensors::RFoot_FSR_FrontRight]
         + (-20.0f) * sensors.sensors[Sensors::RFoot_FSR_RearRight]
         + (20.0f) * sensors.sensors[Sensors::RFoot_FSR_FrontLeft]
         + (20.0f) * sensors.sensors[Sensors::RFoot_FSR_RearLeft];
   }
   if (totalPressure == 0) return  0;
   ZMPF /= totalPressure;
   return ZMPF;
}

void BodyModel::simulationUpdate() {}

void BodyModel::processUpdate(Odometry *odometry, const SensorValues &sensors) {
   float forwardL, forwardR, leftL, leftR, turnLR, liftL, liftR;
   walkCycle.generateWalk(forwardL, forwardR, leftL, leftR, turnLR, liftL, liftR);
   
   boost::numeric::ublas::matrix<float> b2f =
      kinematics->evaluateDHChain(
         Kinematics::FOOT,
         Kinematics::BODY,
         isLeftPhase ? Kinematics::RIGHT_CHAIN : Kinematics::LEFT_CHAIN);
   
   boost::numeric::ublas::matrix<float> b2fOther =
      kinematics->evaluateDHChain(
         Kinematics::FOOT,
         Kinematics::BODY,
         isLeftPhase ? Kinematics::LEFT_CHAIN : Kinematics::RIGHT_CHAIN);

   boost::numeric::ublas::matrix<float> n2f =
      kinematics->evaluateDHChain(
         Kinematics::FOOT,
         Kinematics::NECK,
         isLeftPhase ? Kinematics::RIGHT_CHAIN : Kinematics::LEFT_CHAIN);


   boost::numeric::ublas::matrix<float> f2w =
                  kinematics->createFootToWorldTransform(
         isLeftPhase ? Kinematics::RIGHT_CHAIN : Kinematics::LEFT_CHAIN);

   boost::numeric::ublas::matrix<float> b2w = boost::numeric::ublas::prod(f2w, b2f);
   boost::numeric::ublas::matrix<float> n2w = boost::numeric::ublas::prod(f2w, n2f);
   boost::numeric::ublas::matrix<float> origin = vec4<float>(0, 0, 0, 1);
   boost::numeric::ublas::matrix<float> result = boost::numeric::ublas::prod(b2f, origin);
   boost::numeric::ublas::matrix<float> rPend = boost::numeric::ublas::prod(b2w, origin);
   boost::numeric::ublas::matrix<float> neckPend = boost::numeric::ublas::prod(n2w, origin);
   // float deg = atan2(neckPend(0, 0) - rPend(0, 0), neckPend(2, 0) - rPend(2, 0));
//   std::cout << neckPend(0, 0) << " : " << rPend(0, 0) << " " << neckPend(1, 0) << " " << neckPend(2, 0) << " " << RAD2DEG(deg) << std::endl;

   // float alpha = sensors.sensors[Sensors::InertialSensor_AngleY];
   // float beta = deg; // sensors.joints.angles[Joints::LHipPitch];
   // float psi = alpha - beta;
   // float psiKinematic = atan2(rPend(0, 0), COM_HEIGHT);
   // float thau = psi - psiKinematic;

   // Calculate the centre of mass, convert to frame of reference of foot
   // (ie to foot then rotated by body lean)
   boost::numeric::ublas::matrix<float> com = kinematics->evaluateMassChain();
   boost::numeric::ublas::matrix<float> comOther = prod(b2fOther, com); // inserted CoM for other foot - BH
   centreOfMassOther.x = comOther(0, 0);                                // "
   centreOfMassOther.y = comOther(1, 0);                                // "
   centreOfMassOther.z = comOther(2, 0);                                // "
   com = prod(b2f, com);
   centreOfMass.x = com(0, 0);
   centreOfMass.y = com(1, 0);
   centreOfMass.z = com(2, 0);

   //std::cout << "COM x: " << centreOfMass.x << " y: " << centreOfMass.y << " z: " << centreOfMass.z << std::endl;


   //the following is left from 2011 and is not used atm

   // Get the ZMP from the supporting foot
   float zmpFootOffset = 0;
   if (!getWalkCycle().isDoubleSupportPhase()) {
      zmpFootOffset = getFootZMP(!isLeftPhase, sensors);
   }

   //prediction update
   pendulumModel.predictNext(1/100.0, zmpFootOffset);

   //std::cout << "##############" << std::endl;
   //std::cout << "prediction:   " << std::endl;
   //std::cout << "x: " << pendulumModel.x << std::endl;
   //std::cout << "dx: " << pendulumModel.dx << std::endl;
   //std::cout << "accX: " << pendulumModel.accX << std::endl;
   //std::cout << "theta: " << pendulumModel.theta << std::endl;
   //std::cout << "t: " << pendulumModel.walkCycle.t << std::endl;
   
   //observation update
   float supportFootPosition = result(0, 0); 
   //float h = result(2, 0);
   //float deg = atan2(supportFootPosition - zmpFootOffset, h);
   WalkCycle currentWalkCycle = walkCycle;
   
   /*
   std::cout << "ZMP :" << zmpFootOffset << std::endl;
   std::cout << "observed x:" << supportFootPosition <<  std::endl;
   std::cout << "observed accX:" << sensors.sensors[Sensors::InertialSensor_AccX] 
             << std::endl;
   */

   float alpha = 0.2;
   float beta = 0.5;
      
   pendulumModel.x = (1 - beta) * pendulumModel.x + (beta) * supportFootPosition;
   if (pendulumModel.walkCycle.leftPhase == currentWalkCycle.leftPhase) {
      pendulumModel.dx = (1 - alpha) * pendulumModel.dx + (alpha) * (pendulumModel.x - lastPendulumModel.x) * 100;
   } else { //switching foot
      pendulumModel.x = -pendulumModel.x/1.2;
   }
   pendulumModel.walkCycle = currentWalkCycle;
   
   lastPendulumModel.x = pendulumModel.x;
   lastPendulumModel.dx = pendulumModel.dx;
  
   /*
   std::cout << "updated:" << std::endl;
   std::cout << "x: " << pendulumModel.x << std::endl;
   std::cout << "dx: " << pendulumModel.dx << std::endl;
   std::cout << "t: " << pendulumModel.walkCycle.t << std::endl;
   */

   /*foot switched
   if (pendulumModel.walkCycle.leftPhase != currentWalkCycle.leftPhase) {
      if (!currentWalkCycle.leftPhase) {
         // switching to left support phase
         pendulumModel.x = pendulumModel.x + forwardR - forwardL;
      } else {
         pendulumModel.x = pendulumModel.x + forwardL - forwardR;
      }

   }*/
}

void BodyModel::observationUpdate(Odometry *odometry, const SensorValues &sensors) {
   /*
   float supportFootPosition = isLeftPhase ? forwardR : forwardL;
   // supportFootPosition += isOnFront ? FOOT_LENGTH/2 : -FOOT_LENGTH/2;
   float r = sqrt(supportFootPosition * supportFootPosition + COM_HEIGHT * COM_HEIGHT);

   float obsDTheta = lastHatDTheta +
                     sensors.sensors[Sensors::InertialSensor_AccX] * cos(theta) * MOTION_DT / r;
   dTheta += (obsDTheta - dTheta) * 0.5;
   */
}

