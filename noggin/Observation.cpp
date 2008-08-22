/**
 * Observation.cpp - The landmark observation class. Here we house all those
 * things needed to describe a single landmark sighting.  Observations can be of
 * field objects, corners (line intersections), lines, and possibly other
 * things in the future (i.e. carpet edges)
 *
 * @author Tucker Hermans
 */

#include "Observation.h"

/**
 * @param fo FieldObject that was seen and reported.
 */
Observation::Observation(FieldObject fo)
{
}

/**
 * @param c Corner that was seen and reported.
 */
Observation::Observation(ConcreteCorner c)
{
}

/**
 * @param l Line that was seen and reported.
 */
Observation::Observation(ConcreteLine l)
{
}

/*
 * Determines if the observed object is a line.
 *
 * @return true if the observed object is a line
 */
bool Observation::isLine()
{
    return false;
}
