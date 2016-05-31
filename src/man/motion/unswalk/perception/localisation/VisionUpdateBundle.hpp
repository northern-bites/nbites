/*
Copyright 2013 The University of New South Wales (UNSW).

This file is part of the 2010 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#pragma once

#include "types/ActionCommand.hpp"
#include "types/BallInfo.hpp"
#include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/UNSWRobotInfo.hpp"

#include <vector>

/**
 * A container for all of the input data to the localisation system during a vision update.
 */
struct VisionUpdateBundle {
   
   VisionUpdateBundle() :
      amIOnRedTeam(false),
      awayGoalProb(0.0f),
      headYaw(0.0f),
      isHeadingReliable(false),
      isDistanceReliable(false) {}

   VisionUpdateBundle(
         const std::vector<FieldEdgeInfo> &fieldEdges,
         const std::vector<FieldFeatureInfo> &fieldFeatures,
         const std::vector<PostInfo> &posts,
         const std::vector<UNSWRobotInfo> &robots,
         const std::vector<AbsCoord> &teammatePositions,
         const bool amIOnRedTeam,
         const float awayGoalProb,
         const float headYaw,
         const std::vector<BallInfo> &visibleBalls,
         const bool isHeadingReliable,
         const bool isDistanceReliable) :
            fieldEdges(fieldEdges),
            fieldFeatures(fieldFeatures),
            posts(posts),
            robots(robots),
            teammatePositions(teammatePositions),
            amIOnRedTeam(amIOnRedTeam),
            awayGoalProb(awayGoalProb),
            headYaw(headYaw),
            visibleBalls(visibleBalls),
            isHeadingReliable(isHeadingReliable),
            isDistanceReliable(isDistanceReliable) {}
   
   // Visible field lines in the latest frame.
   std::vector<FieldEdgeInfo> fieldEdges;

   // Visible field line features in the latest frame.
   std::vector<FieldFeatureInfo> fieldFeatures;

   // Visible goal posts in the latest frame.
   std::vector<PostInfo> posts;
   
   // Visible robots in the latest frame.
   std::vector<UNSWRobotInfo> robots;
   
   // This is kind of a remnant from a quick hack we did to use teammates as landmarks.
   // This should be removed/or done properly.
   std::vector<AbsCoord> teammatePositions; 
   
   bool amIOnRedTeam;

   // The probability that the visible goal posts are the opponent goals. Value range [0.0, 1.0]
   float awayGoalProb;

   float headYaw;

   // Currently visible balls. We allow multiple visible balls to account for false-positives.
   std::vector<BallInfo> visibleBalls;
   
   bool isHeadingReliable;
   bool isDistanceReliable;
};

inline std::ostream& operator<<(std::ostream& os, const VisionUpdateBundle& bundle) {
   unsigned numFieldEdges = bundle.fieldEdges.size();
   os.write((char*) &numFieldEdges, sizeof(unsigned));
   for (unsigned i = 0; i < bundle.fieldEdges.size(); i++) {
      os << bundle.fieldEdges[i];
   }
   
   unsigned numFieldFeatures = bundle.fieldFeatures.size();
   os.write((char*) &numFieldFeatures, sizeof(unsigned));
   for (unsigned i = 0; i < bundle.fieldFeatures.size(); i++) {
      os << bundle.fieldFeatures[i];
   }

   unsigned numPosts = bundle.posts.size();
   os.write((char*) &numPosts, sizeof(unsigned));
   for (unsigned i = 0; i < bundle.posts.size(); i++) {
      os << bundle.posts[i];
   }

   os.write((char*) &(bundle.awayGoalProb), sizeof(float));
   os.write((char*) &(bundle.headYaw), sizeof(float));
   os.write((char*) &(bundle.isHeadingReliable), sizeof(bool));
   os.write((char*) &(bundle.isDistanceReliable), sizeof(bool));

   unsigned numVisibleBalls = bundle.visibleBalls.size();
   os.write((char*) &numVisibleBalls, sizeof(unsigned));
   for (unsigned i = 0; i < bundle.visibleBalls.size(); i++) {
      os << bundle.visibleBalls[i];
   }

   return os;
}

inline std::istream& operator>>(std::istream& is, VisionUpdateBundle& bundle) {
   unsigned numFieldEdges;
   is.read((char*) &numFieldEdges, sizeof(unsigned));
   bundle.fieldEdges.clear();
   for (unsigned i = 0; i < numFieldEdges; i++) {
      FieldEdgeInfo newFieldEdge;
      is >> newFieldEdge;
      bundle.fieldEdges.push_back(newFieldEdge);
   }
   
   unsigned numFieldFeatures;
   is.read((char*) &numFieldFeatures, sizeof(unsigned));
   bundle.fieldFeatures.clear();
   for (unsigned i = 0; i < numFieldFeatures; i++) {
      FieldFeatureInfo newFieldFeature;
      is >> newFieldFeature;
      bundle.fieldFeatures.push_back(newFieldFeature);
   }
   
   unsigned numPosts;
   is.read((char*) &numPosts, sizeof(unsigned));
   bundle.posts.clear();
   for (unsigned i = 0; i < numPosts; i++) {
      PostInfo newPost;
      is >> newPost;
      bundle.posts.push_back(newPost);
   }

   is.read((char*) &(bundle.awayGoalProb), sizeof(float));
   is.read((char*) &(bundle.headYaw), sizeof(float));
   is.read((char*) &(bundle.isHeadingReliable), sizeof(bool));
   is.read((char*) &(bundle.isDistanceReliable), sizeof(bool));

   unsigned numVisibleBalls;
   is.read((char*) &numVisibleBalls, sizeof(unsigned));
   bundle.visibleBalls.clear();
   for (unsigned i = 0; i < numVisibleBalls; i++) {
      BallInfo newBall;
      is >> newBall;
      bundle.visibleBalls.push_back(newBall);
   }

   return is;
}
