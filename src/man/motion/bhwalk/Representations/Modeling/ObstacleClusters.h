/*
 * File:   ObstacleClusters.h
 * Author: Arne Bšckmann arneboe@tzi.de
 */

#pragma once

#include "Tools/Math/Random.h"
#include "Representations/Modeling/CombinedWorldModel.h" //for GaussianPositionDistribution

STREAMABLE(ObstacleClustersBH,
{
public:
  /** Function for drawing */
  void draw() const,

  (std::vector<GaussianPositionDistribution>) obstacles, /**< Obstacles in absolute world coordinates */
});

STREAMABLE(ObstacleClustersCompressed,
{
public:
  STREAMABLE(Obstacle,
  {
  public:
    Obstacle(const GaussianPositionDistribution& g);
    operator GaussianPositionDistribution() const,

    (Vector2BH<short>) position,
    (float) c00,
    (float) c01,
    (float) c11,
  });

  ObstacleClustersCompressed(const ObstacleClustersBH& obstacles, unsigned int maxNumOfObstaclesToSend);
  operator ObstacleClustersBH() const,

  (std::vector<Obstacle>) obstacles,
});
