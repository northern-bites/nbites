#pragma once

#include <vector>
#include <queue>
#include <math.h>
#include <limits>
#include <string>
#include <stdio.h>
#include <fstream>

#include "gamecontroller/RoboCupGameControlData.hpp"
#include "VisionFrame.hpp"
#include "types/Ipoint.hpp"
#include "types/Odometry.hpp"
#include "types/AbsCoord.hpp"
#include "types/MapEntry.hpp"
#include "utils/angles.hpp"
#include "Ransac.hpp"
#include "VisionConstants.hpp"
#include "VisionDefs.hpp"
#include "Tfidf.hpp"


class GoalMatcher {
public:


   GoalMatcher();

   // Load a saved visual dictionary for fast matching and starting map of features around the field 
   void loadVocab(std::string vocabFile);
   void loadMap(std::string mapFile);

   // Tries to classify which end the visible goals are at, based on background landmarks, or learns 
   // landmarks if in first ready state for the half
   void process(VisionFrame &frame, unsigned int *seed); 

   // input signals from visionAdapter
   uint8 state; 
   uint8 secondaryState;
   AbsCoord robotPos;
   float headYaw;
   GameType gameType;
   int awayMapSize;
   int homeMapSize;   

private:

   // Tries to classify which field end we are facing, based on background landmarks, returns the number of matches
   int classifyGoalArea(VisionFrame &frame, unsigned int *seed, PostInfo::Type &type);

   // Change to the mirror image position on other side of field
   AbsCoord flip(AbsCoord pos);

   Tfidf tfidf;
   
   bool vocabLoaded;
   //int frame_count; // for open challenge, only saves landmarks for 45 seconds

   bool wasInitial;
   bool clearMap;
   //bool firstReadyState;
   //uint8 lastHalf;
   //uint8 lastState;

   std::vector <PostInfo::Type> obs;

};

