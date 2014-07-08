/*
* @file BallTaking.cpp
* @author Thomas Muender
*/

#include "BallTaking.h"
#include "Tools/InverseKinematic.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Geometry.h"

/*
 * decide phase execution
 */
void BallTaking::update(BallTakingOutputBH& output)
{
  output.isLeavingPossible = false;
  ballData(output);
  output.isTakable = takable.isFilled() && takable.getAverage() == 1;

  if(theMotionSelectionBH.ratios[MotionRequestBH::takeBall] > 0)
  { 
    //finished
    if(phaseLeavingPossible && phase == 5)
    {
      output.isLeavingPossible = true;
      //todo odometry offset

      side = 0;
      phase = 0;
      phaseStart = 0;
      phaseLeavingPossible = false;

      return;
    }
    else if(phaseLeavingPossible)
    {
      phase++;
      phaseStart = theFrameInfoBH.time;
      phaseLeavingPossible = false;
    }
     
         if(phase == 0) phaseZero(output);
    else if(phase == 1) phaseOne(output);
    else if(phase == 2) phaseTwo(output);
    else if(phase == 3) phaseThree(output);
    else if(phase == 4) phaseFour(output);
    else if(phase == 5) phaseFive(output);

    int hardness = 90;
    output.jointHardness.hardness[JointDataBH::LHipRoll] = hardness;
    output.jointHardness.hardness[JointDataBH::LHipPitch] = hardness;
    output.jointHardness.hardness[JointDataBH::LKneePitch] = hardness;
    output.jointHardness.hardness[JointDataBH::LAnklePitch] = hardness;
    output.jointHardness.hardness[JointDataBH::LAnkleRoll] = hardness;
    output.jointHardness.hardness[JointDataBH::RHipRoll] = hardness;
    output.jointHardness.hardness[JointDataBH::RHipPitch] = hardness;
    output.jointHardness.hardness[JointDataBH::RKneePitch] = hardness;
    output.jointHardness.hardness[JointDataBH::RAnklePitch] = hardness;
    output.jointHardness.hardness[JointDataBH::RAnkleRoll] = hardness;
    
    output.angles[JointDataBH::LShoulderPitch] = JointDataBH::ignore;
    output.angles[JointDataBH::LShoulderRoll] = JointDataBH::ignore;
    output.angles[JointDataBH::RShoulderPitch] = JointDataBH::ignore;
    output.angles[JointDataBH::RShoulderRoll] = JointDataBH::ignore;
    output.angles[JointDataBH::LElbowRoll] = JointDataBH::ignore;
    output.angles[JointDataBH::LElbowYaw] = JointDataBH::ignore;
    output.angles[JointDataBH::RElbowRoll] = JointDataBH::ignore;
    output.angles[JointDataBH::RElbowYaw] = JointDataBH::ignore;
    
    output.angles[JointDataBH::HeadPitch] = JointDataBH::ignore;
    output.angles[JointDataBH::HeadYaw] = JointDataBH::ignore;
  }
}


/*
 * calculate ball data
 */
void BallTaking::ballData(BallTakingOutputBH& output)
{
  //crossing y axis
  Geometry::getIntersectionOfLines(Geometry::Line(theBallModelBH.estimate.position, (theBallModelBH.estimate.velocity - theBallModelBH.estimate.position)), Geometry::Line(Vector2BH<>(0,0), Vector2BH<>(0,1)), crossing);
  //float ttr = theBallModelBH.estimate.timeForDistance((crossing - theBallModelBH.estimate.position).abs(), -0.4f);
  moved.add(theBallModelBH.estimate.velocity.abs());
  
  bool between = crossing.y < 90.f && crossing.y > -90.f && theBallModelBH.estimate.velocity.abs() > 1.f;
  bool notClose = theBallModelBH.estimate.position.abs() > 300.f;
  bool stopsBehind = theBallModelBH.estimate.getEndPosition(-0.4f).x < -150.f;
  bool angle = std::fabs(theBallModelBH.estimate.velocity.angle()) > 1.92f;
  bool moving = moved.getAverageFloat() > 10.f;
  bool wasSeen = theFrameInfoBH.getTimeSince(theBallModelBH.timeWhenLastSeen) < 100.f;
  //bool time = ttr > 0.3f && ttr < 1.5f;
  
  valid = between && notClose && stopsBehind && angle && moving && wasSeen; //&& time;
  takable.add(valid ? 1 : 0);
}

/*
 * choose side 2
 */
void BallTaking::phaseZero(BallTakingOutputBH& output)
{
  InverseKinematic::calcLegJoints(standLeftFoot, standRightFoot, output, theRobotDimensionsBH, 0.5f);
  
  if(valid)
  {
    side += crossing.y;
    c++;
  }
  else c--;
  if(c < 0) c = 0;
  
  //leaving possible ?
  if(c > 50)
  {
    c = 0;
    phaseLeavingPossible = true;
  }
  
  //abort
  if(theFrameInfoBH.getTimeSince(theBallModelBH.timeWhenLastSeen) > 1000 || (moved.isFilled() && moved.getAverageFloat() < 1))
  {
    phase = 5;
    phaseLeavingPossible = true;
  }
}


/*
 * bank in
 */
void BallTaking::phaseOne(BallTakingOutputBH& output)
{
  int t = theFrameInfoBH.getTimeSince(phaseStart);
  float tp = (float)t / phaseDuration;

  y = sinUp(tp) * xFactor;
  rotX = sinUp(tp) * rotXFactor;
  zStand = sinUp(tp) * zStandFactor;
  rotZ = sinUp(tp) * rot;
  y2 = sinUp(tp) * y2Factor;

  if(side > 0) //left swing 
  {
    targetLeftFootPositon = Pose3DBH(standLeftFoot.translation.x + (y2 * 80), standLeftFoot.translation.y + y + (y2 * 80), standLeftFoot.translation.z + zStand);
    targetRightFootPositon = Pose3DBH(standRightFoot.translation.x, standRightFoot.translation.y + y, standRightFoot.translation.z + zStand);

    targetLeftFootPositon.rotateZ(rotZ);
    
    output.angles[JointDataBH::RHipRoll] = -rotX;
    output.angles[JointDataBH::LHipRoll] = rotX - y2;
  }
  else //right swing
  {
    targetLeftFootPositon = Pose3DBH(standLeftFoot.translation.x, standLeftFoot.translation.y - y, standLeftFoot.translation.z + zStand);
    targetRightFootPositon = Pose3DBH(standRightFoot.translation.x + (y2 * 80), standRightFoot.translation.y - y - (y2 * 80), standRightFoot.translation.z + zStand);

    targetRightFootPositon.rotateZ(-rotZ);
    
    output.angles[JointDataBH::RHipRoll] = rotX - y2;
    output.angles[JointDataBH::LHipRoll] = -rotX;
  }
  if(!InverseKinematic::calcLegJoints(targetLeftFootPositon, targetRightFootPositon, output, theRobotDimensionsBH, 0.5f))
    OUTPUT_WARNING("not reachable 1");

  //leaving possible ?
  if(theFrameInfoBH.getTimeSince(phaseStart) >= phaseDuration)
  {
    leftEndPhaseOne = targetLeftFootPositon;
    rightEndPhaseOne = targetRightFootPositon;
    phaseLeavingPossible = true;
  }
}


/*
 * wait for taking
 */
void BallTaking::phaseTwo(BallTakingOutputBH& output)
{
  if(!InverseKinematic::calcLegJoints(leftEndPhaseOne, rightEndPhaseOne, output, theRobotDimensionsBH, 0.5f))
    OUTPUT_WARNING("not reachable 2");

  bool time = theFrameInfoBH.getTimeSince(theBallModelBH.timeWhenLastSeen) > 500;
  bool position = theBallModelBH.estimate.position.x < 0;
  bool moving = moved.isFilled() && moved.getAverageFloat() < 1; //todo test

  if(time || position || moving)
  {
    if(theBallModelBH.estimate.position.abs() > 200)
    {
      phase = 4;
      leftEndPhaseFour = leftEndPhaseOne;
      rightEndPhaseFour = rightEndPhaseOne;
    }
    phaseLeavingPossible = true;
  }
}


/*
 * move ball out 1
 */
void BallTaking::phaseThree(BallTakingOutputBH& output)
{
  int t = theFrameInfoBH.getTimeSince(phaseStart);
  float tp = sinUp((float)t / phaseDuration);

  targetLeftFootPositon = leftEndPhaseOne;
  targetRightFootPositon = rightEndPhaseOne;
  
  if(side > 0) //left swing
  {   
    targetLeftFootPositon.translation += Vector3BH<>(-tp * 20, -tp * 35, 0);
  }
  else //right swing
  {
    targetRightFootPositon.translation += Vector3BH<>(-tp * 20, tp * 35, 0);
  }
  
  if(!InverseKinematic::calcLegJoints(targetLeftFootPositon, targetRightFootPositon, output, theRobotDimensionsBH, 0.5f))
    OUTPUT_WARNING("not reachable 3");
  
  //leaving possible ?
  if(theFrameInfoBH.getTimeSince(phaseStart) >= phaseDuration)
  {
    leftEndPhaseThree = targetLeftFootPositon;
    rightEndPhaseThree = targetRightFootPositon;
    phaseLeavingPossible = true;
  }
}


/*
 * move ball out 2
 */
void BallTaking::phaseFour(BallTakingOutputBH& output)
{
  int t = theFrameInfoBH.getTimeSince(phaseStart);
  float tp = sinUp((float)t / phaseDuration);

  targetLeftFootPositon = leftEndPhaseThree;
  targetRightFootPositon = rightEndPhaseThree;
  
  if(side > 0) //left swing
  {    
    targetLeftFootPositon.translation += Vector3BH<>(tp * 100, 0, tp * 25);
    targetRightFootPositon.translation += Vector3BH<>(0, 0, tp * 20);
  }
  else //right swing
  {
    targetLeftFootPositon.translation += Vector3BH<>(0, 0, tp * 20);
    targetRightFootPositon.translation += Vector3BH<>(tp * 100, 0, tp * 25);
  }
  
  if(!InverseKinematic::calcLegJoints(targetLeftFootPositon, targetRightFootPositon, output, theRobotDimensionsBH, 0.5f))
    OUTPUT_WARNING("not reachable 4");
  
  //leaving possible ?
  if(theFrameInfoBH.getTimeSince(phaseStart) >= phaseDuration)
  {
    leftEndPhaseFour = targetLeftFootPositon;
    rightEndPhaseFour = targetRightFootPositon;
    phaseLeavingPossible = true;
  }
}


/*
 * get back to stand
 */
void BallTaking::phaseFive(BallTakingOutputBH& output)
{
  int t = theFrameInfoBH.getTimeSince(phaseStart);
  float tp = sinUp((float)t / phaseDuration);

  targetLeftFootPositon = leftEndPhaseFour;
  targetRightFootPositon = rightEndPhaseFour;

  targetLeftFootPositon.translation = leftEndPhaseFour.translation + (standLeftFoot.translation - leftEndPhaseFour.translation) * tp;
  targetRightFootPositon.translation = rightEndPhaseFour.translation + (standRightFoot.translation - rightEndPhaseFour.translation) * tp;

  if(side > 0) //left swing
  {
    targetLeftFootPositon.rotateZ(-rot * tp);
  }
  else //right swing
  {
    targetRightFootPositon.rotateZ(rot * tp);
  }
  
  if(!InverseKinematic::calcLegJoints(targetLeftFootPositon, targetRightFootPositon, output, theRobotDimensionsBH, 0.5f))
    OUTPUT_WARNING("not reachable 5");
  
  //leaving possible ?
  if(theFrameInfoBH.getTimeSince(phaseStart) >= phaseDuration)
  {
    phaseLeavingPossible = true;
  }
}

MAKE_MODULE(BallTaking, Motion Control)