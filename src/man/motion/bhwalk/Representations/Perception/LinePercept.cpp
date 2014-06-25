/**
* @file LinePercept.h
* Implementation of a class that represents the fieldline percepts
* @author jeff
*/

#include "LinePercept.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/Math/Geometry.h"

using namespace std;

Vector2BH<> LinePerceptBH::Line::calculateClosestPointOnLine(const Vector2BH<>& p) const
{
  const Vector2BH<> normale = Vector2BH<>(cos(alpha + pi), sin(alpha + pi));
  return p + normale * calculateDistToLine(p);
}

float LinePerceptBH::getClosestLine(Vector2BH<> point, Line& retLine) const
{
  vector<LinePerceptBH::Line>::const_iterator closestLine = lines.end();
  float minDist = -1.f;
  for(vector<LinePerceptBH::Line>::const_iterator l1 = lines.begin(); l1 != lines.end(); l1++)
  {
    const float dist = abs(l1->calculateDistToLine(point));
    if(dist < minDist || minDist == -1)
    {
      closestLine = l1;
      minDist = dist;
    }
  }

  if(minDist != -1.f)
    retLine = *closestLine;
  return minDist;
}

void LinePerceptBH::drawOnField(const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified) const
{
  DECLARE_DEBUG_DRAWING("representation:LinePerceptBH:Field", "drawingOnField");
  COMPLEX_DRAWING("representation:LinePerceptBH:Field",
  {
    for(vector<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
    {
      const Drawings::PenStyle pen = line->midLine ? Drawings::ps_dash : Drawings::ps_solid;
      LINE("representation:LinePerceptBH:Field", line->first.x, line->first.y, line->last.x, line->last.y, 15, pen, ColorClasses::red);
      ARROW("representation:LinePerceptBH:Field", line->first.x, line->first.y, line->first.x + cos(line->alpha - pi_2) * 100, line->first.y + sin(line->alpha - pi_2) * 100, 15, pen, ColorClasses::blue);
      CROSS("representation:LinePerceptBH:Field", line->first.x, line->first.y, 10, 5, pen, ColorClasses::red);
    }
    for(vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
    {
      switch(inter->type)
      {
      case Intersection::X:
        LINE("representation:LinePerceptBH:Field", inter->pos.x - inter->dir1.x * 100, inter->pos.y - inter->dir1.y * 100,
             inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        LINE("representation:LinePerceptBH:Field", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100,
             inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        break;
      case Intersection::T:
        LINE("representation:LinePerceptBH:Field", inter->pos.x, inter->pos.y,
             inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        LINE("representation:LinePerceptBH:Field", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100,
             inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        break;
      case Intersection::L:
        LINE("representation:LinePerceptBH:Field", inter->pos.x, inter->pos.y,
             inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        LINE("representation:LinePerceptBH:Field", inter->pos.x , inter->pos.y,
             inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100,
             15, Drawings::ps_solid, ColorClasses::blue);
        break;
      }
    }
    if(circle.found)
    {
      CROSS("representation:LinePerceptBH:Field", circle.pos.x, circle.pos.y, 40, 40, Drawings::ps_solid, ColorClasses::blue);
      CIRCLE("representation:LinePerceptBH:Field", circle.pos.x, circle.pos.y, theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified, 30, Drawings::ps_solid, ColorClasses::blue, Drawings::bs_null, ColorClasses::blue);
    }
    for(vector<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
      ARROW("representation:LinePerceptBH:Field", seg->p1.x, seg->p1.y, seg->p2.x, seg->p2.y, 10, Drawings::ps_solid, ColorClasses::orange);
  });
}

void LinePerceptBH::drawOnImage(const CameraMatrixBH& theCameraMatrixBH, const CameraInfoBH& theCameraInfoBH, const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified, const ImageCoordinateSystemBH& theImageCoordinateSystemBH) const
{
  DECLARE_DEBUG_DRAWING("representation:LinePerceptBH:ImageBH", "drawingOnImage");
  COMPLEX_DRAWING("representation:LinePerceptBH:ImageBH",
  {
    for(vector<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
    {
      Vector2BH<int> p1;
      Vector2BH<int> p2;
      if(Geometry::calculatePointInImage(line->first, theCameraMatrixBH, theCameraInfoBH, p1) &&
      Geometry::calculatePointInImage(line->last, theCameraMatrixBH, theCameraInfoBH, p2))
      {
        const Drawings::PenStyle pen = line->midLine ? Drawings::ps_dash : Drawings::ps_solid;
        Vector2BH<> uncor1 = theImageCoordinateSystemBH.fromCorrectedLinearized(p1);
        Vector2BH<> uncor2 = theImageCoordinateSystemBH.fromCorrectedLinearized(p2);
        LINE("representation:LinePerceptBH:ImageBH", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 3, pen, ColorClasses::red);
      }
    }

    if(circle.found)
    {
      Vector2BH<int> p1;
      if(Geometry::calculatePointInImage(circle.pos, theCameraMatrixBH, theCameraInfoBH, p1))
      {
        Vector2BH<> uncor = theImageCoordinateSystemBH.fromCorrectedLinearized(p1);
        CROSS("representation:LinePerceptBH:ImageBH", uncor.x, uncor.y, 5, 3, Drawings::ps_solid, ColorClasses::blue);
      }
      const float stepSize = 0.2f;
      for(float i = 0; i < pi2; i += stepSize)
      {
        Vector2BH<int> p1;
        Vector2BH<int> p2;
        if(Geometry::calculatePointInImage(circle.pos + Vector2BH<>(theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified, 0).rotate(i), theCameraMatrixBH, theCameraInfoBH, p1) &&
           Geometry::calculatePointInImage(circle.pos + Vector2BH<>(theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified, 0).rotate(i + stepSize), theCameraMatrixBH, theCameraInfoBH, p2))
        {
          Vector2BH<> uncor1 = theImageCoordinateSystemBH.fromCorrectedLinearized(p1);
          Vector2BH<> uncor2 = theImageCoordinateSystemBH.fromCorrectedLinearized(p2);
          LINE("representation:LinePerceptBH:ImageBH", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 3, Drawings::ps_solid, ColorClasses::blue);
        }
      }

    }

    for(vector<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
    {
      Vector2BH<int> p1;
      Vector2BH<int> p2;
      if(Geometry::calculatePointInImage(seg->p1, theCameraMatrixBH, theCameraInfoBH, p1) &&
         Geometry::calculatePointInImage(seg->p2, theCameraMatrixBH, theCameraInfoBH, p2))
      {
        Vector2BH<> uncor1 = theImageCoordinateSystemBH.fromCorrectedApprox(p1);
        Vector2BH<> uncor2 = theImageCoordinateSystemBH.fromCorrectedApprox(p2);
        LINE("representation:LinePerceptBH:ImageBH", uncor1.x, uncor1.y, uncor2.x, uncor2.y, 2, Drawings::ps_solid, ColorClasses::orange);
      }
    }

    for(vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
    {
      Vector2BH<int> p1;
      Vector2BH<int> p2;
      Vector2BH<int> p3;
      Vector2BH<int> p4;
      const Vector2BH<> dir1 = inter->dir1 * 100.f;
      const Vector2BH<> dir2 = inter->dir2 * 100.f;
      switch(inter->type)
      {
      case Intersection::X:
        Geometry::calculatePointInImage(inter->pos - dir1, theCameraMatrixBH, theCameraInfoBH, p1);
        Geometry::calculatePointInImage(inter->pos + dir1, theCameraMatrixBH, theCameraInfoBH, p2);
        Geometry::calculatePointInImage(inter->pos - dir2, theCameraMatrixBH, theCameraInfoBH, p3);
        Geometry::calculatePointInImage(inter->pos + dir2, theCameraMatrixBH, theCameraInfoBH, p4);
        break;
      case Intersection::T:
        Geometry::calculatePointInImage(inter->pos, theCameraMatrixBH, theCameraInfoBH, p1);
        Geometry::calculatePointInImage(inter->pos + dir1, theCameraMatrixBH, theCameraInfoBH, p2);
        Geometry::calculatePointInImage(inter->pos - dir2, theCameraMatrixBH, theCameraInfoBH, p3);
        Geometry::calculatePointInImage(inter->pos + dir2, theCameraMatrixBH, theCameraInfoBH, p4);
        break;
      case Intersection::L:
        Geometry::calculatePointInImage(inter->pos, theCameraMatrixBH, theCameraInfoBH, p1);
        Geometry::calculatePointInImage(inter->pos + dir1, theCameraMatrixBH, theCameraInfoBH, p2);
        Geometry::calculatePointInImage(inter->pos, theCameraMatrixBH, theCameraInfoBH, p3);
        Geometry::calculatePointInImage(inter->pos + dir2, theCameraMatrixBH, theCameraInfoBH, p4);
        break;
      }

      Vector2BH<> uncor1 = theImageCoordinateSystemBH.fromCorrectedLinearized(p1);
      Vector2BH<> uncor2 = theImageCoordinateSystemBH.fromCorrectedLinearized(p2);
      Vector2BH<> uncor3 = theImageCoordinateSystemBH.fromCorrectedLinearized(p3);
      Vector2BH<> uncor4 = theImageCoordinateSystemBH.fromCorrectedLinearized(p4);
      ARROW("representation:LinePerceptBH:ImageBH", uncor1.x, uncor1.y, uncor2.x, uncor2.y,
            3, Drawings::ps_solid, ColorClasses::blue);
      ARROW("representation:LinePerceptBH:ImageBH", uncor3.x, uncor3.y, uncor4.x, uncor4.y,
            3, Drawings::ps_solid, ColorClasses::blue);
    }
  });
}

void LinePerceptBH::drawIn3D(const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified) const
{
  DECLARE_DEBUG_DRAWING3D("representation:LinePerceptBH", "origin");
  TRANSLATE3D("representation:LinePerceptBH", 0, 0, -210);
  COMPLEX_DRAWING3D("representation:LinePerceptBH",
  {
    for(vector<Line>::const_iterator line = lines.begin(); line != lines.end(); line++)
      LINE3D("representation:LinePerceptBH", line->first.x, line->first.y, 0, line->last.x, line->last.y, 0, 2, ColorClasses::red);
    for(vector<Intersection>::const_iterator inter = intersections.begin(); inter != intersections.end(); inter++)
    {
      switch(inter->type)
      {
      case Intersection::X:
        LINE3D("representation:LinePerceptBH", inter->pos.x - inter->dir1.x * 100, inter->pos.y - inter->dir1.y * 100, 0,
               inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
               2, ColorClasses::blue);
        LINE3D("representation:LinePerceptBH", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100, 0,
               inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
               2, ColorClasses::blue);
        break;
      case Intersection::T:
        LINE3D("representation:LinePerceptBH", inter->pos.x, inter->pos.y, 0,
               inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
               2, ColorClasses::blue);
        LINE3D("representation:LinePerceptBH", inter->pos.x - inter->dir2.x * 100, inter->pos.y - inter->dir2.y * 100, 0,
               inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
               2, ColorClasses::blue);
        break;
      case Intersection::L:
        LINE3D("representation:LinePerceptBH", inter->pos.x, inter->pos.y, 0,
               inter->pos.x + inter->dir1.x * 100, inter->pos.y + inter->dir1.y * 100, 0,
               2, ColorClasses::blue);
        LINE3D("representation:LinePerceptBH", inter->pos.x , inter->pos.y, 0,
               inter->pos.x + inter->dir2.x * 100, inter->pos.y + inter->dir2.y * 100, 0,
               2, ColorClasses::blue);
        break;
      }
    }
    if(circle.found)
    {
      Vector2BH<> v1(circle.pos.x + theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified, circle.pos.y);
      for(int i = 1; i < 33; ++i)
      {
        const float angle(i * pi2 / 32);
        Vector2BH<> v2(circle.pos.x + cos(angle) * (theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified),
                     circle.pos.y + sin(angle) * (theFieldDimensionsBH.centerCircleRadius + circleBiggerThanSpecified));
        LINE3D("representation:LinePerceptBH", v1.x, v1.y, 0, v2.x, v2.y, 0, 2, ColorClasses::blue);
        v1 = v2;
      }
    }
    for(vector<LineSegment>::const_iterator seg = singleSegs.begin(); seg != singleSegs.end(); seg++)
      LINE3D("representation:LinePerceptBH", seg->p1.x, seg->p1.y, 0, seg->p2.x, seg->p2.y, 0, 2, ColorClasses::orange);
  });
}
