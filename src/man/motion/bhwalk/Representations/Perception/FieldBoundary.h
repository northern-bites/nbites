/**
* @author Alexis Tsogias
*/

#pragma once

#include <vector>
#include "Tools/Math/Vector2.h"
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(FieldBoundaryBH,
{
public:
  typedef std::vector<Vector2BH<int> > InImage;   ///< Type for the boundary in image coordinates.
  typedef std::vector<Vector2BH<float> > InField; ///< Type for the boundary in field coordinates.

  std::vector<InImage> convexBoundaryCandidates; ///< Possible bondary candidates.

  /**
   * Draws some DebugDrawings
   *
   * Modifiable:
   *   representation:FieldBoundaryBH:SelectedCandidate - Set the candidate which shall be drawn
   *
   * DebugDrawings:
   *   representation:FieldBoundaryBH:BoundarySpots
   *   representation:FieldBoundaryBH:ConvexBoundary
   *   representation:FieldBoundaryBH:BoundaryCandidates
   *   representation:FieldBoundaryBH:ImageBH - The field final boundary in the image.
   *   representation:FieldBoundaryBH:Field - The field final boundary on the field.
   *   representation:FieldBoundaryBH:HighestPoint
   */
  void draw() const;

  /**
   * Returns the y coordinate of the field boundary at the specified x coordiante in the current image.
   */
  int getBoundaryY(int x) const,

  (InImage) boundarySpots,     ///< Spots on the boundary.
  (InImage) convexBoundary,    ///< A convex upper hull arround the spots that schould fit best the actual boundary.
  (InField) boundaryOnField,   ///< The boundary projectet to the Field in relative coordinates.
  (InImage) boundaryInImage,   ///< The boundary in image coordinates.
  (Vector2BH<int>) highestPoint, ///< The highest pont of the boundary.
  (bool)(false) isValid,	   ///< True if a boundary could be detected.
  (int)(16) scanlineDistance,  ///< The distance between the scanlines used to find the boundarySpots.
  (int)(0) width,              ///< The width of the current image; used for some debug drawings.
});
