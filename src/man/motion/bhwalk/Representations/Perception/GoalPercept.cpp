/**
* @file GoalPercept.h
*
* Representation of a seen goal
*
* @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
*/

#include "GoalPercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"

void GoalPerceptBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:GoalPerceptBH:ImageBH", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("representation:GoalPerceptBH:Field", "drawingOnField");
  DECLARE_DEBUG_DRAWING3D("representation:GoalPerceptBH", "origin");
  TRANSLATE3D("representation:GoalPerceptBH", 0, 0, -230);
  ColorRGBA color = ColorClasses::yellow;
  for(unsigned int i = 0; i < goalPosts.size(); ++i)
  {
    const GoalPost& p = goalPosts.at(i);
    if(p.position != GoalPost::IS_UNKNOWN)
    {
      CIRCLE("representation:GoalPerceptBH:Field", p.positionOnField.x, p.positionOnField.y,
             50, 0, Drawings::ps_solid, ColorClasses::white, Drawings::bs_solid, color);
      LINE("representation:GoalPerceptBH:Field", p.positionOnField.x, p.positionOnField.y,
           p.positionOnField.x, p.positionOnField.y +
           (p.position == GoalPost::IS_LEFT ? -700 : 700),
           60, Drawings::ps_solid, color);
      MID_DOT("representation:GoalPerceptBH:ImageBH", p.positionInImage.x, p.positionInImage.y,
              ColorClasses::white, color);
      LINE("representation:GoalPerceptBH:ImageBH", p.positionInImage.x, p.positionInImage.y, p.positionInImage.x, 0,
           5, Drawings::ps_solid, color);
      // Sorry, no access to field dimensions here, so the dimensions are hard coded
      CYLINDER3D("representation:GoalPerceptBH", p.positionOnField.x, p.positionOnField.y, 400, 0, 0, 0, 50, 800, color);
    }
    else
    {
      CIRCLE("representation:GoalPerceptBH:Field", p.positionOnField.x, p.positionOnField.y,
             50, 0, Drawings::ps_solid, ColorRGBA(255, 0, 0), Drawings::bs_solid, color);
      MID_DOT("representation:GoalPerceptBH:ImageBH", p.positionInImage.x, p.positionInImage.y,
              ColorRGBA(255, 0, 0), color);
      LINE("representation:GoalPerceptBH:ImageBH", p.positionInImage.x, p.positionInImage.y, p.positionInImage.x, 0,
           5, Drawings::ps_dot, color);
      // Sorry, no access to field dimensions here, so the dimensions are hard coded
      CYLINDER3D("representation:GoalPerceptBH", p.positionOnField.x, p.positionOnField.y, 400, 0, 0, 0, 50, 800, color);
    }
  }
}
