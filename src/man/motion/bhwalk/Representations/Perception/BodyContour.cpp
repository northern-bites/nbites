/**
* @file BodyContour.h
* The file implements a class that represents the contour of the robot's body in the image.
* The contour can be used to exclude the robot's body from image processing.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "BodyContour.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "ColorReference.h"

BodyContourBH::Line::Line(const Vector2BH<int>& p1, const Vector2BH<int>& p2)
: p1(p1.x < p2.x ? p1 : p2),
  p2(p1.x < p2.x ? p2 : p1) {}

void BodyContourBH::clipBottom(int x, int& y) const
{
  int yIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->yAt(x, yIntersection) && yIntersection < y)
      y = yIntersection;
}


void BodyContourBH::clipBottom(int x, int& y, int imageHeight) const
{
  clipBottom(x, y);

  //clippedY can be outside the image
  if(y < 0)
  {
    y = 0;
  }
  else if(y >= imageHeight)
  {
    y = imageHeight  - 1;
  }
}


void BodyContourBH::clipLeft(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y > i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection > x)
        x = xIntersection;
      else if(i->p2.y <= y && i->p2.x > x) // below a segment, clip anyway
        x = i->p2.x;
    }
}

void BodyContourBH::clipRight(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y < i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection < x)
        x = xIntersection;
      else if(i->p1.y <= y && i->p1.x < x) // below a segment, clip anyway
        x = i->p1.x;
    }
}

void BodyContourBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:BodyContourBH", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("representation:BodyContourBH:maxY", "drawingOnImage");
  COMPLEX_DRAWING("representation:BodyContourBH",
  {
    for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
      LINE("representation:BodyContourBH", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 1,
           Drawings::ps_solid, ColorRGBA(255, 0, 255));

    for(int x = 0; x < cameraResolution.x; x += 10)
    {
      int y = cameraResolution.y;
      clipBottom(x, y);
      LINE("representation:BodyContourBH", x, y, x, cameraResolution.y, 1,
           Drawings::ps_solid, ColorRGBA(255, 0, 255));
    }
  });
  int maxY = getMaxY();
  LINE("representation:BodyContourBH:maxY", 0, maxY, cameraResolution.x - 1, maxY,
       1, Drawings::ps_solid, ColorRGBA(255, 0, 255));
}

int BodyContourBH::getMaxY() const
{
  int y = cameraResolution.y - 1;
  clipBottom(0, y);
  clipBottom(cameraResolution.x - 1, y);

  if(y < 0)
  {//no need to continue
    return 0;
  }

  for(const Line& line : lines)
  {
    if(line.p1.y >= 0 && line.p1.x >= 0 && line.p1.x < cameraResolution.x && line.p1.y < y)
    {
      y = line.p1.y;
    }
    if(line.p2.y >= 0 && line.p2.x >= 0 && line.p2.x < cameraResolution.x && line.p2.y < y)
    {
      y = line.p2.y;
    }
  }
  return y;
}



