/**
* @file LineSpots.h
* Declaration of a class that represents a spot that  indicates a line.
* @author jeff
*/

#pragma once

#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streams/AutoStreamable.h"
#include <algorithm>

/**
* @class LineSpotsBH
* This class contains all the linespots and nonlinesposts (=white regions which are no lines)
*/
STREAMABLE(LineSpotsBH,
{
public:
  /**
   * @class LineSpot
   * A class that represents a spot that's an indication of a line.
   */
  STREAMABLE(LineSpot,
  {
  public:
    bool operator<(const LineSpotsBH::LineSpot& ls) const
    {
      if(std::min(p1.x, p2.x) < std::min(ls.p1.x, ls.p2.x))
        return true;
      else if(std::min(p1.y, p2.y) < std::min(ls.p1.y, ls.p2.y))
        return true;
      else
        return false;
    }

    bool operator<(const LineSpotsBH::LineSpot* ls) const
    {
      return *this < *ls;
    },

    (float) alpha, /**< the direction/rotation of the region   | */
    (float) alphaLen, /**< "ausbreitung entlang alpha"         |-> Haupttraegheitsachsenbla */
    (float) alphaLen2, /**< "ausbreitung orthogonal zu alpha"  | */
    (int) xs, /**< center of mass x */
    (int) ys, /**< center of mass y */
    (Vector2BH<int>) p1,
    (Vector2BH<int>) p2, /**< The starting/end point of this linespot in image coordinates*/
  });

  /**
   * @class NonLineSpot
   * This class represents a white region which is no line
   */
  STREAMABLE(NonLineSpot,
  {,
    (Vector2BH<int>) p1, /**< start point of this spot in image coordinates */
    (Vector2BH<int>) p2, /**< end point of this spot in image coordinates */
    (int) size, /**< The size of the coresponding region in the image */
  });

  /**
  * The method draws all line spots.
  */
  void draw() const
  {
    DECLARE_DEBUG_DRAWING("representation:LineSpotsBH:nonLineSpots", "drawingOnImage");
    COMPLEX_DRAWING("representation:LineSpotsBH:nonLineSpots",
    {
      for(std::vector<LineSpotsBH::NonLineSpot>::const_iterator i = nonLineSpots.begin(); i != nonLineSpots.end(); ++i)
      {
        ARROW("representation:LineSpotsBH:nonLineSpots", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 2, Drawings::ps_solid, ColorClasses::blue);
      }
    });

    DECLARE_DEBUG_DRAWING("representation:LineSpotsBH:ImageBH", "drawingOnImage"); // Draws the LineSpotsBH to the image
    COMPLEX_DRAWING("representation:LineSpotsBH:ImageBH",
    {
      for(std::vector<LineSpotsBH::LineSpot>::const_iterator i = spots.begin(); i != spots.end(); ++i)
      {
        LINE("representation:LineSpotsBH:ImageBH", i->xs, i->ys, i->xs + (int)(cos(i->alpha + pi_2) * i->alphaLen2), i->ys + (int)(sin(i->alpha + pi_2)*i->alphaLen2), 0, Drawings::ps_solid, ColorClasses::black);
        ARROW("representation:LineSpotsBH:ImageBH", i->p1.x, i->p1.y, i->p2.x, i->p2.y, 0, Drawings::ps_solid, ColorClasses::black);
      }
    });
  },

  (std::vector<LineSpot>) spots, /**< All the line spots */
  (std::vector<NonLineSpot>) nonLineSpots, /**< All the non line spots (= white regions which are no lines)*/
});
