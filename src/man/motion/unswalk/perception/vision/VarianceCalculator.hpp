#pragma once

#include <vector>

// #include "types/FootInfo.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
// #include "types/RobotInfo.hpp"
// #include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"

class VarianceCalculator {
public:
   //static void setVariance(std::vector<FootInfo>         &feet);
   static void setVariance(std::vector<BallInfo>         &balls);
   static void setVariance(std::vector<PostInfo>         &posts);
   //static void setVariance(std::vector<RobotInfo>        &robots);
   //static void setVariance(std::vector<FieldEdgeInfo>    &fieldEdges);
   static void setVariance(std::vector<FieldFeatureInfo> &fieldFeatures);

private:
   static void setDistanceHeadingVariance(RRCoord &rr);
   static void setDistanceHeadingOrientationVariance(RRCoord &rr);
};

