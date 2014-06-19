/**
* @file BallPerceptBH.cpp
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include "BallPercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void BallPerceptBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:BallPerceptBH:ImageBH", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("representation:BallPerceptBH:Field", "drawingOnField");
  DECLARE_DEBUG_DRAWING3D("representation:BallPerceptBH", "origin");
  TRANSLATE3D("representation:BallPerceptBH", 0, 0, -230);
  if(ballWasSeen)
  {
    CIRCLE("representation:BallPerceptBH:ImageBH",
           positionInImage.x,
           positionInImage.y,
           radiusInImage,
           1, // pen width
           Drawings::ps_solid,
           ColorClasses::black,
           Drawings::bs_solid,
           ColorRGBA(255, 128, 64, 100));
    CIRCLE("representation:BallPerceptBH:Field",
           relativePositionOnField.x,
           relativePositionOnField.y,
           35,
           0, // pen width
           Drawings::ps_solid,
           ColorClasses::orange,
           Drawings::bs_null,
           ColorClasses::orange);
    // Sorry, no access to field dimensions here, so ball radius is hard coded
    SPHERE3D("representation:BallPerceptBH", relativePositionOnField.x, relativePositionOnField.y, 35, 35, ColorClasses::orange);
  }
}
