#include "HoughSpace.h"

HoughSpace::HoughSpace() : acceptThreshold(DEFAULT_ACCEPT_THRESH),
                           angleSpread(DEFAULT_ANGLE_SPREAD)
{

}

/**
 * Pass through the given Gradient and mark all potential edges
 * in the accumulator.
 */
void HoughSpace::markEdges(boost::shared_ptr<Gradient> g)
{

}

/**
 * Smooth out irregularities in the Hough accumulator to reduce noisy peaks.
 */
void HoughSpace::smooth()
{

}

/**
 * Find the peaks of the accumulator and create the list of lines in the space.
 */
void HoughSpace::peaks()
{

}

/**
 * Combine/remove duplicate lines and lines which are not right.
 */
void HoughSpace::suppress()
{

}

/**
 * Reset the accumulator and peak arrays to their initial values.
 */
void HoughSpace::reset()
{

}
