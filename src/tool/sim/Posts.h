/*
 * Defines all of the concrete posts on the soccer field--there are 4.
 * Specifies where each one is located to distinguish them.
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include <Eigen/Dense>
#include "WorldConstants.h"

namespace tool{
namespace visionsim{

/* A post is actually represented by a single point, which is stored
 * as a 3D world point to work with the transformation math.
 * @see Image.h
 * This point represents the center of the base of the post.
*/
typedef Eigen::Vector3f FieldPost;

// Total number of posts on the field
static const int NUM_POSTS = 4;

static const FieldPost LEFT_TOP_POST(FIELD_LEFT_EDGE_X,
                                     TOP_GOALPOST_Y,
                                     0);

static const FieldPost RIGHT_TOP_POST(FIELD_RIGHT_EDGE_X,
                                      TOP_GOALPOST_Y,
                                      0);


static const FieldPost LEFT_BOTTOM_POST(FIELD_LEFT_EDGE_X,
                                        BOTTOM_GOALPOST_Y,
                                        0);

static const FieldPost RIGHT_BOTTOM_POST(FIELD_RIGHT_EDGE_X,
                                         BOTTOM_GOALPOST_Y,
                                         0);

// All of the different posts, used for constructing visual posts
// @see Image constructor
static const FieldPost ALL_POSTS[NUM_POSTS] =
{
    LEFT_TOP_POST,
    RIGHT_TOP_POST,
    LEFT_BOTTOM_POST,
    RIGHT_BOTTOM_POST
};

}
}
