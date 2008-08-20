/**
 * Landmark.cpp - The landmark class.  Here we house all those things needed
 * to describe a single landmark.  Landmarks can be field objects, corners (
 * line intersections), lines, and possibly other things in the future...
 *
 * @author Tucker Hermans
 */

#include "Landmark.h"

/**
 *
 * @param fo FieldObject that was seen to be passed into the filter
 */
Landmark::Landmark(FieldObject fo)
{
}

/**
 * @param c Corner that was seen to be passed into the filter
 */
Landmark::Landmark(VisualCorner c)
{
}

/**
 * @param l Line that was seen to be passed into the filter.
 */
Landmark::Landmark(VisualLine l)
{
}
