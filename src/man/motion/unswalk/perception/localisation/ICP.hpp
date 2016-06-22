
#pragma once

#include <vector>
#include <string>
#include <Eigen/Eigen>

#include "types/Point.hpp"
#include "types/AbsCoord.hpp"
#include "types/PostInfo.hpp"
#include "types/TeamBallInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/FieldEdgeInfo.hpp"

#define ICP_LOST -1
#define ICP_NO_OBS 0
#define ICP_LOCALISED 1

typedef unsigned long long timestamp_t;
enum TargetType { POINT, VERT_LINE, HOR_LINE };

// These values chosen to make sure they don't overlap with PostInfo::Type
enum FeatureType {   NO_TYPE = 0x10, 

                     // T Junctions: Whether a T junction has a post on the left or right of the upright 'T'
                     T_NO_POST = 0x20, T_POST_LEFT = 0x30, T_POST_RIGHT = 0x40, 

                     // Corners: GB means the goal box corner
                     C_GB_LEFT = 0x50, C_GB_RIGHT = 0x60, C_OUTSIDE = 0x70, C_UNKNOWN = 0x80,

                     // Parallel Lines: if orientation is known
                     PL_KNOWN = 0x100, PL_UNKNOWN = 0x200
                  };



// Iterative Closest Point
class ICP {
public:
   ICP();

   // Entry function from localisationAdapter, returns success or fail, if success the result indicates
   // how many points were used in the observation (greater number is more reliable, 2 points means only
   // a single field line or field feature was used. If isLost, it will not return an observation based
   // on single field line.
   static int localise(  const AbsCoord &robotPos,
                  const std::vector<FieldFeatureInfo> &fieldFeatures,
                  const std::vector<PostInfo> &posts, 
                  const float awayGoalProb,
                  const float headYaw,
                  const std::vector<FieldEdgeInfo> &fieldEdges,
                  const AbsCoord &ballRRC, bool isLost,
                  const AbsCoord &teamBall = AbsCoord(NAN,NAN,NAN) );

   static AbsCoord getCombinedObs(void);
   
   static std::vector<LineInfo> getAllFieldLines(void);

};

