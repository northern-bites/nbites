/*
 * FootContactModelProvider.cpp
 *
 *  Created on: Mar 14, 2012
 *      Author: arne
 *              simont@tzi.de
 */

#include "FootContactModelProvider.h"
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(FootContactModelProvider, Sensing);


FootContactModelProvider::FootContactModelProvider():
  contactDurationLeft(0), contactDurationRight(0), leftFootLeftDuration(0), leftFootRightDuration(0), rightFootLeftDuration(0), rightFootRightDuration(0)
{
}


void FootContactModelProvider::update(FootContactModelBH& model)
{
  // Check, if any bumper is pressed
  bool leftFootLeft = !theDamageConfigurationBH.leftFootBumperDefect && checkContact(KeyStatesBH::leftFootLeft, leftFootLeftDuration);
  bool leftFootRight = !theDamageConfigurationBH.leftFootBumperDefect && checkContact(KeyStatesBH::leftFootRight, leftFootRightDuration);
  bool rightFootLeft = !theDamageConfigurationBH.rightFootBumperDefect &&  checkContact(KeyStatesBH::rightFootLeft, rightFootLeftDuration);
  bool rightFootRight = !theDamageConfigurationBH.rightFootBumperDefect && checkContact(KeyStatesBH::rightFootRight, rightFootRightDuration);

  // Update statistics
  if (leftFootLeft || leftFootRight)
  {
    contactBufferLeft.add(1);
    contactDurationLeft++;
  }
  else
  {
    contactBufferLeft.add(0);
    contactDurationLeft = 0;
  }
  if (rightFootLeft || rightFootRight)
  {
    contactBufferRight.add(1);
    contactDurationRight++;
  }
  else
  {
    contactBufferRight.add(0);
    contactDurationRight = 0;
  }

  // Generate model
  if ((theMotionInfoBH.motion == MotionInfoBH::stand || theMotionInfoBH.motion == MotionInfoBH::walk) &&
      (theGameInfoBH.state == STATE_READY || theGameInfoBH.state == STATE_SET || theGameInfoBH.state == STATE_PLAYING) && //The bumper is used for configuration in initial
      (theFallDownStateBH.state == FallDownStateBH::upright))
  {
    if(contactBufferLeft.getSum() > contactThreshold)
    {
      model.contactLeft = true;
      model.contactDurationLeft = contactDurationLeft;
      model.lastContactLeft = theFrameInfoBH.time;
    }
    else
    {
      model.contactLeft = false;
      model.contactDurationLeft = 0;
    }
    if(contactBufferRight.getSum() > contactThreshold)
    {
      model.contactRight = true;
      model.contactDurationRight = contactDurationRight;
      model.lastContactRight = theFrameInfoBH.time;
    }
    else
    {
      model.contactRight = false;
      model.contactDurationRight = 0;
    }
  }
  else
  {
    model.contactLeft = false;
    model.contactRight = false;
    model.contactDurationLeft = 0;
    model.contactDurationRight = 0;
  }

  DECLARE_PLOT("module:FootContactModelProvider:sumLeft");
  DECLARE_PLOT("module:FootContactModelProvider:durationLeft");
  DECLARE_PLOT("module:FootContactModelProvider:contactLeft");
  DECLARE_PLOT("module:FootContactModelProvider:sumRight");
  DECLARE_PLOT("module:FootContactModelProvider:durationRight");
  DECLARE_PLOT("module:FootContactModelProvider:contactRight");
  DECLARE_PLOT("module:FootContactModelProvider:leftFootLeft");
  DECLARE_PLOT("module:FootContactModelProvider:leftFootRight");
  DECLARE_PLOT("module:FootContactModelProvider:rightFootLeft");
  DECLARE_PLOT("module:FootContactModelProvider:rightFootRight");
  PLOT("module:FootContactModelProvider:sumLeft", contactBufferLeft.getSum());
  PLOT("module:FootContactModelProvider:durationLeft", contactDurationLeft);
  PLOT("module:FootContactModelProvider:sumRight", contactBufferRight.getSum());
  PLOT("module:FootContactModelProvider:durationRight", contactDurationRight);
  PLOT("module:FootContactModelProvider:contactLeft", model.contactLeft ? 10 : 0);
  PLOT("module:FootContactModelProvider:contactRight", model.contactRight ? 10 : 0);
  PLOT("module:FootContactModelProvider:leftFootLeft", leftFootLeft ? 10 : 0);
  PLOT("module:FootContactModelProvider:leftFootRight", leftFootRight ? 10 : 0);
  PLOT("module:FootContactModelProvider:rightFootLeft", rightFootLeft ? 10 : 0);
  PLOT("module:FootContactModelProvider:rightFootRight", rightFootRight ? 10 : 0);
}


bool FootContactModelProvider::checkContact(KeyStatesBH::Key key, int& duration)
{
  bool pressed = theKeyStatesBH.pressed[key];
  duration = pressed ? duration + 1 : 0;
  // if key is pressed longer than the malfunction threshold, it is ignored
  return pressed && duration < malfunctionThreshold;
}
