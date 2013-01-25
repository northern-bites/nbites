#pragma once

#include <Eigen/Dense>
#include "WorldConstants.h"

namespace tool{
namespace visionsim{

// This point represents the center of the base of the post.
typedef Eigen::Vector3f FieldPost;

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

static const FieldPost ALL_POSTS[NUM_POSTS] =
{
    LEFT_TOP_POST,
    RIGHT_TOP_POST,
    LEFT_BOTTOM_POST,
    RIGHT_BOTTOM_POST
};

}
}
