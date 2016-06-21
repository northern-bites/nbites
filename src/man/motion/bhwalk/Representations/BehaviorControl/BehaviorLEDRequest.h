/**
* @file Representations/BehaviorControl/BehaviorLEDRequest.h
* This file contains the BehaviorLEDRequestBH class.
* @author jeff
*/

#pragma once

#include "Representations/Infrastructure/LEDRequest.h"

STREAMABLE(BehaviorLEDRequestBH,
{
public:
  ENUM(BehaviorLED,
    leftEye,
    rightEye,
    leftEar,
    rightEar
  );

  ENUM(EyeColor,
    defaultColor,
    red,
    green,
    blue,
    white,
    magenta,
    yellow,
    cyan
  );

  bool operator==(const BehaviorLEDRequestBH& other) const
  {
    for(int i = 0; i < numOfBehaviorLEDs; i++)
      if(modifiers[i] != other.modifiers[i])
        return false;
    return true;
  }

  bool operator!=(const BehaviorLEDRequestBH& other) const
  {
    return !(*this == other);
  },

  (LEDRequestBH, LEDState[numOfBehaviorLEDs]) modifiers,
  (EyeColor)(defaultColor) leftEyeColor,
  (EyeColor)(defaultColor) rightEyeColor,

  // Initialization
  for(int i = 0; i < numOfBehaviorLEDs; ++i)
    modifiers[i] = LEDRequestBH::on;
});
