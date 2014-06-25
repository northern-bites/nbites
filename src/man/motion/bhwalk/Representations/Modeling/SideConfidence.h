/**
* @file SideConfidence.h
*
* Declaration of class SideConfidenceBH.
* @author Michel Bartsch, Thomas Münder
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Enum.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Settings.h"

/**
* @class SideConfidenceBH
*/
STREAMABLE(SideConfidenceBH,
{
public:
  ENUM(ConfidenceState,
    CONFIDENT,
    ALMOST_CONFIDENT,
    UNSURE,
    CONFUSED
  );                               /**< Discrete states of confidence, mapped by provider */

  /** Draw representation. */
  void draw() const
  {
  DECLARE_DEBUG_DRAWING3D("representation:SideConfidenceBH", "robot",
    {
      static const ColorRGBA colors[numOfConfidenceStates] =
      {
        ColorRGBA(0, 255, 0),
        ColorRGBA(0, 128, 0),
        ColorRGBA(255, 255, 0),
        ColorRGBA(255, 0, 0)
      };
      int pNumber = Global::getSettings().playerNumber;
      float centerDigit = (pNumber > 1) ? 50.f : 0;
      ROTATE3D("representation:SideConfidenceBH", 0, 0, pi_2);
      DRAWDIGIT3D("representation:SideConfidenceBH", pNumber, Vector3BH<>(centerDigit, 0.f, 500.f), 80, 5, colors[confidenceState]);
    });

    DECLARE_DEBUG_DRAWING("representation:SideConfidenceBH","drawingOnField",
    {
      DRAWTEXT("representation:SideConfidenceBH", -5000, -3600, 140, ColorClasses::red, "Sideconfidence: " << sideConfidence);
    });
  },

  (float)(1) sideConfidence,  /**< Am I mirrored because of two yellow goals (0 = no idea, 1 = absolute sure I am right). */
  (bool)(false) mirror,       /**< Indicates whether ball model of others is mirrored to own ball model. */
  (ConfidenceState)(CONFIDENT) confidenceState, /**< The state of confidence */
});
