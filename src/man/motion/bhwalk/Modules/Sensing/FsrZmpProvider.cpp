#include "FsrZmpProvider.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Pose3D.h"
#include <float.h>

MAKE_MODULE(FsrZmpProvider, Sensing)

FsrZmpProvider::FsrZmpProvider()
{
  fsrBasePositions[SensorDataBH::fsrLFL - SensorDataBH::fsrLFL]= Vector3BH<>(70.25f, 29.9f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrLFR - SensorDataBH::fsrLFL]= Vector3BH<>(70.25f, -23.1f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrLBL - SensorDataBH::fsrLFL]= Vector3BH<>(-30.25f, 29.9f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrLBR - SensorDataBH::fsrLFL]= Vector3BH<>(-29.65f, -19.1f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrRFL - SensorDataBH::fsrLFL]= Vector3BH<>(70.25f, 23.1f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrRFR - SensorDataBH::fsrLFL]= Vector3BH<>(70.25f, -29.9f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrRBL - SensorDataBH::fsrLFL]= Vector3BH<>(-30.25f, 19.1f, -theRobotDimensions.heightLeg5Joint);
  fsrBasePositions[SensorDataBH::fsrRBR - SensorDataBH::fsrLFL]= Vector3BH<>(-29.65f, -29.9f, -theRobotDimensions.heightLeg5Joint);
}

void FsrZmpProvider::update(FsrZmpBH& fsrZmp)
{
  DECLARE_DEBUG_DRAWING3D("module:FsrZmpProvider:fsrs", "origin");
  DECLARE_DEBUG_DRAWING3D("module:FsrZmpProvider:zmp", "origin");
  DECLARE_DEBUG_DRAWING("module:FsrZmpProvider:cop", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:FsrZmpProvider:copLeft", "drawingOnField");
  DECLARE_DEBUG_DRAWING("module:FsrZmpProvider:copRight", "drawingOnField");
  float sum = FLT_EPSILON; //avoid division by zero;
  Vector3BH<> fsrPositions[8];
  calculateFsrPosition(MassCalibrationBH::ankleLeft, 0, fsrPositions);
  calculateFsrPosition(MassCalibrationBH::ankleRight, 4, fsrPositions);
  
  for(int i = 0; i < 8; ++i)
  {
    sum += theSensorDataBH.data[SensorDataBH::fsrLFL + i];
  }
  fsrZmp.pressure = sum;
  Vector3BH<> zmp;
  //calculate zmp relative to robot origin in 3D
  for(int i = 0; i < 8; ++i)
  {
    zmp += fsrPositions[i] * (theSensorDataBH.data[SensorDataBH::fsrLFL + i]/sum);
  }
  fsrZmp.cop = zmp;
  
  //calculate center of pressure for left foot in 2D
  sum = FLT_EPSILON; //avoid division by zero
  fsrZmp.copLeft.x = 0;
  fsrZmp.copLeft.y = 0;
  for(int i = 0; i < 4; ++i)
  {
    sum += theSensorDataBH.data[SensorDataBH::fsrLFL + i];
  }
  fsrZmp.pressureLeft = sum;
  for(int i = 0; i < 4; ++i)
  {
    fsrZmp.copLeft.x += fsrBasePositions[i].x * (theSensorDataBH.data[SensorDataBH::fsrLFL + i]/sum);
    fsrZmp.copLeft.y += fsrBasePositions[i].y * (theSensorDataBH.data[SensorDataBH::fsrLFL + i]/sum);
  }  
  
  //calculate center of pressure for right foot in 2D
  fsrZmp.copRight.x = 0;
  fsrZmp.copRight.y = 0;
  sum = FLT_EPSILON; //avoid division by zero
  for(int i = 4; i < 8; ++i)
  {
    sum += theSensorDataBH.data[SensorDataBH::fsrLFL + i];
  }
  fsrZmp.pressureRight = sum;
  for(int i = 4; i < 8; ++i)
  {
    fsrZmp.copRight.x += fsrBasePositions[i].x * (theSensorDataBH.data[SensorDataBH::fsrLFL + i]/sum);
    fsrZmp.copRight.y += fsrBasePositions[i].y * (theSensorDataBH.data[SensorDataBH::fsrLFL + i]/sum);
  }  
  
  COMPLEX_DRAWING("module:FsrZmpProvider:copLeft",
    for(int i = 0; i < 4; ++i)
    {
      CROSS("module:FsrZmpProvider:copLeft", fsrBasePositions[i].x * 8, fsrBasePositions[i].y * 8, 20, 4, Drawings::ps_solid, ColorClasses::blue);        
    }
    CROSS("module:FsrZmpProvider:copLeft", fsrZmp.copLeft.x * 8, fsrZmp.copLeft.y * 8, 20, 4, Drawings::ps_solid, ColorClasses::red);
    DRAWTEXT("module:FsrZmpProvider:copLeft", 100, 100, 50, ColorClasses::black, "Pressure: " << fsrZmp.pressureLeft);
  );
  COMPLEX_DRAWING("module:FsrZmpProvider:copRight",
    for(int i = 4; i < 8; ++i)
    {
      CROSS("module:FsrZmpProvider:copRight", fsrBasePositions[i].x * 8, fsrBasePositions[i].y * 8, 20, 4, Drawings::ps_solid, ColorClasses::blue);        
    }
    CROSS("module:FsrZmpProvider:copRight", fsrZmp.copRight.x * 8, fsrZmp.copRight.y * 8, 20, 4, Drawings::ps_solid, ColorClasses::red);
    DRAWTEXT("module:FsrZmpProvider:copRight", 100, 100, 50, ColorClasses::black, "Pressure: " << fsrZmp.pressureRight);
  );  
  
  LINE3D("module:FsrZmpProvider:zmp", 0, 0, 0, zmp.x, zmp.y, zmp.z, 10, ColorClasses::blue);
  COMPLEX_DRAWING("module:FsrZmpProvider:cop",
    for(int i = 0; i < 8; ++i)
    {
      CROSS("module:FsrZmpProvider:cop", fsrPositions[i].x * 4, fsrPositions[i].y * 4, 20, 4, Drawings::ps_solid, ColorClasses::blue);
    } 
    CROSS("module:FsrZmpProvider:cop", zmp.x * 4, zmp.y * 4, 20, 4, Drawings::ps_solid, ColorClasses::red);
  );
}

void FsrZmpProvider::calculateFsrPosition(MassCalibrationBH::Limb limb, int startIndex, Vector3BH<> (&positions)[8]) const
{
  for(int i = startIndex; i < startIndex+4; ++i)
  {
    positions[i] = fsrBasePositions[i];
    positions[i] = theRobotModelBH.limbs[limb].rotation * positions[i] + theRobotModelBH.limbs[limb].translation;
    LINE3D("module:FsrZmpProvider:fsrs", 0, 0, 0, positions[i].x, positions[i].y, positions[i].z, 10, ColorClasses::red);
  }  
}