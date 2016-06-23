#pragma once

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <utility>
#include <string>
#include "utils/Timer.hpp"

#include "types/FootInfo.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/UNSWRobotInfo.hpp"
#include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/Ipoint.hpp"
#include "types/Odometry.hpp"

#include "NNMC.hpp"
#include "UNSWCamera.hpp"
#include "VisionDefs.hpp"
#include "CameraToRR.hpp"
#include "yuv.hpp"
#include "BallDetection.hpp"
#include "FieldLineDetection.hpp"
#include "GoalDetection.hpp"
#include "OldRobotDetection.hpp"
#include "robotdetection/RobotDetection.hpp"
#include "FieldEdgeDetection.hpp"
#include "Fovea.hpp"
#include "SurfDetection.hpp"
#include "VisualOdometry.hpp"
#include "GoalMatcher.hpp"

class VisionFrame;

class Vision {
   friend class VisionAdapter;
   friend class CalibrationTab;
   friend class VisionTab;
   friend class OverviewTab;
   friend class Tab;
   friend class CameraPoseTab;
   friend class ControlTab;
   friend class SurfTab;
   friend class ICPTab;

   friend class GoalDetection;
   friend class BallDetection;
   friend class RobotDetection;
	friend class SurfDetection;
   friend class VisualOdometry;

   public:
      /**
       * Constructor for vision module. Initialises the camera
       * Only displays an error if camera not initated
       **/
      Vision(bool dumpframes,
             int dumprate,
             std::string dumpfile,
             std::string calibrationFileTop,
             std::string calibrationFileBot,
             std::string goalMap,
				 std::string vocabFile,
             bool visionEnabled,
             bool seeBluePosts,
             bool seeLandmarks);

      /* Destructor */
      ~Vision();

      /* Pointer to current vision frame */
      boost::shared_ptr<VisionFrame> frame;

      /**
       * Camera object, talks to v4l or NaoQi
       * Initilised by main
       **/
      static UNSWCamera *camera;
      static UNSWCamera *top_camera;
      static UNSWCamera *bot_camera;

      bool                          caughtLeft;
      bool                          caughtRight;
      std::vector<Ipoint>           landmarks;
      std::vector<FootInfo>         feet;
      std::vector<BallInfo>         balls;
      BallHint                      ballHint;
      std::vector<PostInfo>         posts;
      std::vector<UNSWRobotInfo>        robots;
      std::vector<FieldEdgeInfo>    fieldEdges;
      std::vector<FieldFeatureInfo> fieldFeatures;
      Odometry                      vOdometry;
      Odometry                      dualOdometry;
      unsigned int                  missedFrames;
      std::pair<int, int>           dxdy;
      PostInfo::Type                goalArea;
      float                         awayGoalProb;
      int                           awayMapSize;
      int                           homeMapSize; 

      FoveaT<hGoals, eGrey> topSaliency;
      FoveaT<hGoals, eGrey> botSaliency;

      /**
       * C-Space lookup table
       **/
      NNMC nnmc_top, nnmc_bot;

   private:

      /**
       * Variables for turning parts of vision on and off
       **/
      bool seeBluePosts;
      bool seeLandmarks;

      /**
       * Pointer to the frame currently being processed
       **/
      uint8_t const* currentFrame;
      uint8_t const* topFrame;
      uint8_t const* botFrame;
      

      /**
       * Seed value for rand_r
       **/
      unsigned int seed;

      /**
       * which camera is in use
       **/
      WhichCamera whichCamera;

      /**
       * A nice wall clock
       **/
      Timer timer;

      /**
       * Settings related to recording frames to disk
       **/
      bool dumpframes;
      int dumprate;
      std::string dumpfile;
      std::string calibrationFileTop, calibrationFileBot;
      std::string robotMap;
      std::string goalMap;
      std::string vocabFile;
      bool visionEnabled;

      /**
       * The major sections of vision processing used by process frame
       **/
          
      CameraToRR convRR;
		SurfDetection surfDetection;
      RobotDetection robotDetection;
      OldRobotDetection oldRobotDetection;
      VisualOdometry visualOdometry;
      FieldEdgeDetection fieldEdgeDetection;
      FieldLineDetection fieldLineDetection;
      BallDetection ballDetection;
      GoalDetection goalDetection;
      GoalMatcher goalMatcher;  

      /**
       * Get a frame from the camera and put a pointer to it
       * in currentFrame
       **/
      void getFrame();

      /**
       * Processes a single frame, reading it from the camera
       * finding all interesting objects, and running sanity checks
       **/
      void processFrame();

      /**
       * Subsample the image, forming a colour histogram
       * in the x-axis and y-axis, for each colour in
       * the C-PLANE.
       **/
      void saliencyScan();
      void edgeSaliencyScan();

      void pickPost(std::vector<PostInfo>& posts,
                    std::vector<FieldFeatureInfo>& features);
};

