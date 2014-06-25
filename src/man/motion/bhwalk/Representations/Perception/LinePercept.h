/**
* @file LinePercept.h
* Declaration of a class that represents the fieldline percepts
* @author jeff
*/

#pragma once

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Configuration/FieldDimensions.h"
#include <list>

/**
* @class LinePerceptBH
* A class that represents the found fieldlines, center circle and intersections.
*/
STREAMABLE(LinePerceptBH,
{
public:
  /**
   * @class Linesegment
   *
   * This class represents a linesegment generated from a lienspot.
   */
  STREAMABLE(LineSegment,
  {,
    (float) alpha, /**< direction of representation in Hesse norm form of this linesegment */
    (float) d, /**< distance of representation in Hesse norm form of this linesegment */
    (Vector2BH<>) p1, /**< start point of this linesegment in field coordinates */
    (Vector2BH<>) p2, /**< end point of this linesegment in field coordinates */
    (Vector2BH<int>) p1Img, /**< start point of this linesegment in image coordinates */
    (Vector2BH<int>) p2Img, /**< end point of this linesegment in image coordinates */
  });

  /**
   * @class Line
   *
   * This class represents a found fieldline.
   */
  STREAMABLE(Line,
  {
  public:
    /**
     * Calculates the distance of a point p the this line
     * @param p a point
     * @return the distance
     */
    float calculateDistToLine(const Vector2BH<>& p) const
    {
      return p.x * std::cos(alpha) + p.y * std::sin(alpha) - d;
    }

    /**
     * Calculates the closest point to a point on this line
     * @param p a point
     * @return the closest point on this line
     */
    Vector2BH<> calculateClosestPointOnLine(const Vector2BH<>& p) const,

    (float) alpha, /**< direction of this line in Hesse norm form */
    (float) d, /**< distance of this line in Hesse norm form */
    (bool) dead, /**< This is needed for merging lines */
    (bool) midLine, /**< Whether this is the line throught the center circle */
    (std::vector<LineSegment>) segments, /**< The linesegments forming this line */
    (Vector2BH<>) first, /**< The starting point of this line in field coordinates */
    (Vector2BH<>) last, /**< The end point of this line in field coordinates */
    (Vector2BH<>) startInImage, /**< The start point of this line in image coordinates */
    (Vector2BH<>) endInImage, /**< The end point of this line in image coordinates */
  });

  /**
   * @class CircleSpot
   *
   * This class represents circle spots. A circle spot
   * is a point calculated from a linesegment where the
   * center circle would be if the linesegment is part
   * of the center circle.
   * This is also used for the found circle.
   */
  STREAMABLE(CircleSpot,
  {
    friend class LinePerceptor; // Access to iterator
    std::list<LineSegment>::iterator iterator, /**< An temporary iterator pointing to the according segment
                                                    in the singleSegs list */
    (Vector2BH<float>) pos, /**< The position of the center of the center circle in field coordinates */
    (bool)(false) found, /**< Whether the center circle was found in this frame */
    (unsigned)(0) lastSeen, /**< The last time the center circle was seen */
  });

  /**
   * @class Intersection
   * A class representing a intersection of two fieldlines
   */
  STREAMABLE(Intersection,
  {
  public:
    ENUM(IntersectionType,
      L,
      T,
      X
    ),

    (IntersectionType) type,
    (Vector2BH<>) pos, /**< The fieldcoordinates of the intersection */
    (Vector2BH<>) dir1, /**< The first direction of the lines intersected. */
    (Vector2BH<>) dir2, /**< The second direction of the lines intersected. */
  });

  /** Determines the closest line to a given point
  * @param point the given point
  * @param retLine the closest line
  * @return the distance from point to retLine
  * */
  float getClosestLine(Vector2BH<> point, Line& retLine) const;

  /**
  * The method draws the line percepts on the field.
  */
  void drawOnField(const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified) const;

  /**
  * The method draws the line percepts on the image.
  */
  void drawOnImage(const CameraMatrixBH& theCameraMatrixBH, const CameraInfoBH& theCameraInfoBH, const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified, const ImageCoordinateSystemBH& theImageCoordinateSystemBH) const;

  /**
  * The method draws the line percepts in the 3D View.
  */
  void drawIn3D(const FieldDimensionsBH& theFieldDimensionsBH, float circleBiggerThanSpecified) const,

  (std::vector<Line>) lines, /**< The found fieldlines */
  (std::vector<Intersection>) intersections, /**< The intersections of the lines */
  (std::vector<LineSegment>) singleSegs, /**< Line segments which could not be clustered to a line */
  (std::vector<LineSegment>) rawSegs, /**< All line segments before clustering */
  (CircleSpot) circle, /**< The position of the center circle if found */
});
