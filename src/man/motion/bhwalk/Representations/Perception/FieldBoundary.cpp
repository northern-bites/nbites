/**
* @author Alexis Tsogias
*/

#include "FieldBoundary.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/Modify.h"
#include "Platform/BHAssert.h"

void FieldBoundaryBH::draw() const
{
  if(width == 0)
    return;

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:BoundarySpots", "drawingOnImage");
  for(const Vector2BH<int>& p : boundarySpots)
  {
    DOT("representation:FieldBoundaryBH:BoundarySpots", p.x, p.y, ColorClasses::blue, ColorClasses::blue);
  }

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:ConvexBoundary", "drawingOnImage");
  const Vector2BH<int>* previ = nullptr;
  for(const Vector2BH<int>& p : convexBoundary)
  {
    DOT_AS_VECTOR("representation:FieldBoundaryBH:ConvexBoundary", p, ColorClasses::red, ColorClasses::red);
    if(previ != nullptr)
    {
      LINE("representation:FieldBoundaryBH:ConvexBoundary", p.x, p.y, previ->x, previ->y, 1, Drawings::ps_solid, ColorClasses::red);
    }
    previ = &p;
  }

  int selected = -1;
  MODIFY("representation:FieldBoundaryBH:SelectedCandidate", selected);

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:BoundaryCandidates", "drawingOnImage");
  int num = convexBoundaryCandidates.size();
  float step = 255.0f / (num - 1);
  int pos = 0;
  for(const InImage& tmpBoundary : convexBoundaryCandidates)
  {
    previ = nullptr;
    unsigned char colorMod = static_cast<unsigned char>(step * pos);
    ColorRGBA col = ColorRGBA(colorMod, colorMod, 255 - colorMod);
    if(pos == selected || selected < 0 || selected >= num)
    {
      for(const Vector2BH<int>& p : tmpBoundary)
      {
        DOT_AS_VECTOR("representation:FieldBoundaryBH:BoundaryCandidates", p, col, col);
        if(previ != nullptr)
        {
          LINE("representation:FieldBoundaryBH:BoundaryCandidates", p.x, p.y, previ->x, previ->y, 1, Drawings::ps_solid, col);
        }
        previ = &p;
      }
    }
    pos++;
  }

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:ImageBH", "drawingOnImage");
  previ = nullptr;
  for(const Vector2BH<int>& p : boundaryInImage)
  {
    DOT_AS_VECTOR("representation:FieldBoundaryBH:ImageBH", p, ColorClasses::orange, ColorClasses::orange);
    if(previ != nullptr)
    {
      LINE("representation:FieldBoundaryBH:ImageBH", p.x, p.y, previ->x, previ->y, 1, Drawings::ps_solid, ColorClasses::orange);
    }
    previ = &p;
  }

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:Field", "drawingOnField");
  const Vector2BH<float>* prevf = nullptr;
  for(const Vector2BH<float>& p : boundaryOnField)
  {
    DOT("representation:FieldBoundaryBH:Field", p.x, p.y, ColorClasses::black, ColorClasses::black);
    if(prevf != nullptr)
    {
      LINE("representation:FieldBoundaryBH:Field", p.x, p.y, prevf->x, prevf->y, 20, Drawings::ps_solid, ColorClasses::black);
    }
    prevf = &p;
  }

  DECLARE_DEBUG_DRAWING("representation:FieldBoundaryBH:HighestPoint", "drawingOnImage");
  LINE("representation:FieldBoundaryBH:HighestPoint", highestPoint.x, highestPoint.y, highestPoint.x + 20, highestPoint.y, 2, Drawings::ps_solid, ColorClasses::black);
  LINE("representation:FieldBoundaryBH:HighestPoint", highestPoint.x, highestPoint.y, highestPoint.x, highestPoint.y + 20, 2, Drawings::ps_solid, ColorClasses::black);
}

int FieldBoundaryBH::getBoundaryY(int x) const
{
  ASSERT(boundaryInImage.size() >= 2);

  const Vector2BH<int>* left = &boundaryInImage.front();
  const Vector2BH<int>* right = &boundaryInImage.back();

  if(x < left->x)
    right = &(*(boundaryInImage.begin() + 1));
  else if(x > right->x)
    left = &(*(boundaryInImage.end() - 2));
  else
  {
    for(const Vector2BH<int>& point : boundaryInImage)
    {
      if(point.x == x)
        return point.y;
      else if(point.x < x && point.x > left->x)
        left = &point;
      else if(point.x > x && point.x < right->x)
        right = &point;
    }
  }

  double m = 1.0 * (right->y - left->y) / (right->x - left->x);

  return static_cast<int>((x * m) + right->y - (right->x * m));
}
