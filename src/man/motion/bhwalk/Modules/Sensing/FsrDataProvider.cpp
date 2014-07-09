#include "FsrDataProvider.h"

MAKE_MODULE(FsrDataProvider, Sensing)

FsrDataProvider::FsrDataProvider()
{
  LFsrFL = Vector2BH<>(70.25f, 29.9f);
  LFsrFR = Vector2BH<>(70.25f, -23.1f);
  LFsrRL = Vector2BH<>(-30.25f, 29.9f);
  LFsrRR = Vector2BH<>(-29.65f, -19.1f);
  RFsrFL = Vector2BH<>(70.25f, 23.1f);
  RFsrFR = Vector2BH<>(70.25f, -29.9f);
  RFsrRL = Vector2BH<>(-30.25f, 19.1f);
  RFsrRR = Vector2BH<>(-29.65f, -29.9f);
}

void FsrDataProvider::update(FsrDataBH& fsrData)
{
  float leftSum = 0;
  leftSum += theSensorDataBH.data[SensorDataBH::fsrLBL];
  leftSum += theSensorDataBH.data[SensorDataBH::fsrLBR];
  leftSum += theSensorDataBH.data[SensorDataBH::fsrLFL];
  leftSum += theSensorDataBH.data[SensorDataBH::fsrLFR];
  fsrL.add(leftSum);

  if(fsrL.isFilled() && fsrL.getAverageFloat() + fsrWeightOffset >= theRobotModelBH.totalMass) fsrData.leftFootContact = true;
  else fsrData.leftFootContact = false;

  float rightSum = 0;
  rightSum += theSensorDataBH.data[SensorDataBH::fsrRBL];
  rightSum += theSensorDataBH.data[SensorDataBH::fsrRBR];
  rightSum += theSensorDataBH.data[SensorDataBH::fsrRFL];
  rightSum += theSensorDataBH.data[SensorDataBH::fsrRFR];
  fsrR.add(rightSum);

  if(fsrR.isFilled() && fsrR.getAverageFloat() + fsrWeightOffset >= theRobotModelBH.totalMass) fsrData.rightFootContact = true;
  else fsrData.rightFootContact = false;

  if(fsrData.leftFootContact == true && fsrData.rightFootContact == false)
  {
    fsrData.centerOfPressure = LFsrFL * theSensorDataBH.data[SensorDataBH::fsrLFL] +
    LFsrFR * theSensorDataBH.data[SensorDataBH::fsrLFR] +
    LFsrRL * theSensorDataBH.data[SensorDataBH::fsrLBL] +
    LFsrRR * theSensorDataBH.data[SensorDataBH::fsrLBR];

    fsrData.centerOfPressure /= leftSum;
  }
  else if(fsrData.leftFootContact == false && fsrData.rightFootContact == true)
  {
    fsrData.centerOfPressure = RFsrFL * theSensorDataBH.data[SensorDataBH::fsrRFL] +
    RFsrFR * theSensorDataBH.data[SensorDataBH::fsrRFR] +
    RFsrRL * theSensorDataBH.data[SensorDataBH::fsrRBL] +
    RFsrRR * theSensorDataBH.data[SensorDataBH::fsrRBR];

     fsrData.centerOfPressure /= rightSum;
  }



}