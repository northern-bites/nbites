#include "Vision.hpp"
#include <algorithm>
#include <strings.h>
#include <string.h>
#include <ctime>
#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "utils/NaoVersion.hpp"

#include "NaoCameraV4.hpp"


using namespace std;
extern bool offNao;

Vision::Vision(
        bool _dumpframes,
        int _dumprate,
        string _dumpfile, 
        string _calibrationFileTop, 
        string _calibrationFileBot,
        string _goalMap,
		  string _vocabFile,
        bool _visionEnabled,
        bool _seeBluePosts,
        bool _seeLandmarks)
   : topSaliency(BBox(Point(0,0), Point(TOP_SALIENCY_COLS, TOP_SALIENCY_ROWS)),
              TOP_SALIENCY_DENSITY, 0, true),
     botSaliency(BBox(Point(0,0), Point(BOT_SALIENCY_COLS, BOT_SALIENCY_ROWS)),
              BOT_SALIENCY_DENSITY, 0, false),
     seeBluePosts( _seeBluePosts),
     seeLandmarks( _seeLandmarks),       
     dumpframes(_dumpframes),
     dumprate(_dumprate),
     dumpfile(_dumpfile),
     calibrationFileTop(_calibrationFileTop), 
     calibrationFileBot(_calibrationFileBot),
     goalMap(_goalMap),         
     vocabFile(_vocabFile),
     visionEnabled(_visionEnabled)
     
{
   if (offNao) visionEnabled = false;
   if (visionEnabled) {
      nnmc_top.load(calibrationFileTop.c_str());
      nnmc_bot.load(calibrationFileBot.c_str());
      // make sure surfDetection and goalMatcher use the same vocab file!
      surfDetection.loadVocab(vocabFile);
		goalMatcher.loadVocab(vocabFile);
      //goalMatcher.loadMap(goalMap); 
      goalArea = PostInfo::pNone;   
      currentFrame = NULL;
      topFrame = NULL;
      botFrame = NULL;
      if (dumpframes) {
         camera->startRecording(dumpfile.c_str(), dumprate);
      }
   }
   

   frame = boost::shared_ptr<VisionFrame>();
   // srand needs a seed, we could make this constant for repeatability
   // seed = time(NULL);
   seed = 42;
   convRR.setCamera(camera);
   awayMapSize = 0;
   homeMapSize = 0; 
}

Vision::~Vision() {
   llog(INFO) << "Vision Destroyed" << endl;
   camera->stopRecording();
}

void Vision::getFrame() {
   //camera->setCamera(whichCamera);

//   if (naoVersion >= nao_v4) {
      /* Read from the inactive camera. We will probably want to modify
       * this when we read from both streams
       */
      //(camera == top_camera ? bot_camera : top_camera)->get();
   botFrame = bot_camera->get();
   topFrame = top_camera->get();
  // }
   //currentFrame = camera->get();
}

void Vision::processFrame()
{
   const NNMC &topNnmc = nnmc_top;
   const NNMC &botNnmc = nnmc_bot;

   frame = boost::shared_ptr<VisionFrame>(
         new VisionFrame(topFrame, topNnmc, botFrame, botNnmc, convRR, frame));

   /* Now we are tracking three past frames */
   if (frame->last && frame->last->last && frame->last->last->last) {
      frame->last->last->last->last = boost::shared_ptr<VisionFrame>();
   }
   frame->whichCamera = whichCamera;
   
   /*******************************************************************
    * Fovea Construction                                              *
    *******************************************************************/
   timer.restart();

   botSaliency.actuate(*frame);
   topSaliency.actuate(*frame);

   botSaliency.xhistogram.applyWindowFilter
            (HIST_AVE_WINDOW_SIZE, hBall | hGoals);
   botSaliency.yhistogram.applyWindowFilter
            (HIST_AVE_WINDOW_SIZE, hBall | hGoals);

   topSaliency.xhistogram.applyWindowFilter
            (HIST_AVE_WINDOW_SIZE, hBall | hGoals);
   topSaliency.yhistogram.applyWindowFilter
            (HIST_AVE_WINDOW_SIZE, hBall | hGoals);

   llog(VERBOSE) << "Fovea Construction took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Fovea Construction took " << timer.elapsed_us() << " us" << std::endl;
   }

   /* Note: this shadows the templated definition */
   const Fovea &topSaliency = this->topSaliency.asFovea();
   const Fovea &botSaliency = this->botSaliency.asFovea();
   convRR.isRobotMoving();


   /*******************************************************************
    * Field Edge Detection                                            *
    *******************************************************************/
   timer.restart();
   fieldEdgeDetection.findFieldEdges(*frame,
                                     topSaliency,
                                     botSaliency,
                                     &convRR,
                                     &seed);
   frame->topStartScanCoords = fieldEdgeDetection.topStartScanCoords;
   frame->botStartScanCoords = fieldEdgeDetection.botStartScanCoords;
   llog(VERBOSE) << "Field Edge Detection took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Field Edge Detection took " << timer.elapsed_us() << " us" << std::endl;
   }


   /*******************************************************************
    * Goal Detection                                                  *
    *******************************************************************/
   timer.restart();
   goalDetection.findGoals(*frame, botSaliency, &seed);
   goalDetection.findGoals(*frame, topSaliency, &seed);
      
   llog(VERBOSE) << "Goal Detection took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Goal Detection took " << timer.elapsed_us() << " us" << std::endl;
   }


   /********************************************************************
    * Robot Detection - needs fieldEdgeDetection, goalDetection        *
    *******************************************************************/
   timer.restart();
   robotDetection.findRobotsWithBot(*frame, topSaliency, botSaliency);
   llog(VERBOSE) << "Robot Detection took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << endl;
   if (timer.elapsed_us() > 30000) {
       llog(ERROR) << "Robot Detection took " << timer.elapsed_us() << " us" << std::endl;
   }

   /********************************************************************
    * SURF Landmark Extraction - needs Robot Detection                *
    *******************************************************************/
   timer.restart();
   surfDetection.findLandmarks(*frame, robotDetection._robots);
   llog(VERBOSE) << "Surf Landmark Extraction & Mapping to Visual Words took ";
   llog(VERBOSE) << timer.elapsed_us() << " us" << endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Surf Landmark Extraction & Mapping to Visual Words took " << timer.elapsed_us() << " us" << std::endl;
   }
   /********************************************************************
    * Visual Odometry - needs SURF                                    *
    *******************************************************************/
    
   timer.restart();
   visualOdometry.findOdometry(*frame, &seed); 
   llog(VERBOSE) << "Surf Visual Odometry took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Surf Visual Odometry took " << timer.elapsed_us() << " us" << std::endl;
   }

  /********************************************************************
    * SURF Landmarks used to classify the goal area                   *
    *******************************************************************/
   timer.restart();
   if (seeLandmarks && false){
      goalMatcher.process(*frame, &seed);  
   }
   llog(VERBOSE) << "Goal Post Matching took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Goal Post Matching took " << timer.elapsed_us() << " us" << std::endl;
   }

   /*******************************************************************
    * Ball Detection                                                  *
    *******************************************************************/
   timer.restart();
   ballDetection.findBalls(*frame, topSaliency, botSaliency, &seed);
   llog(VERBOSE) << "Ball Detection took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Ball Detection took " << timer.elapsed_us() << " us" << std::endl;
   }

   /*******************************************************************
    * Field Feature Detection                                         *
    *******************************************************************/
   timer.restart();
   fieldLineDetection.findFieldFeatures(*frame, topSaliency,
                                        botSaliency, &seed);
   llog(VERBOSE) << "Field Feature Detection took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "Field Feature Detection took " << timer.elapsed_us() << " us" << std::endl;
   }

   /**** Hax for left / right goal ****/
   pickPost(frame->posts, fieldLineDetection.fieldFeatures);


   /*******************************************************************
    * Copy Detected Objects To Blackboard                             *
    *******************************************************************/
   timer.restart(); 
   balls          = frame->balls;
   landmarks      = frame->landmarks;
   ballHint       = frame->ballHint;
   posts          = frame->posts;
   robots         = robotDetection._robots;
   fieldEdges     = fieldEdgeDetection.fieldEdges;
   fieldFeatures  = fieldLineDetection.fieldFeatures;
   vOdometry      = frame->odometry;
   dualOdometry   = visualOdometry.dualOdometry;
   caughtLeft     = visualOdometry.caughtLeft;
   caughtRight    = visualOdometry.caughtRight;
   missedFrames   = frame->missedFrames;
   dxdy           = frame->dxdy;
   goalArea       = frame->goalArea;
   awayGoalProb   = frame->awayGoalProb;
   homeMapSize    = goalMatcher.homeMapSize;
   awayMapSize    = goalMatcher.awayMapSize;

   llog(VERBOSE) << "Copy To Blackboard took " << timer.elapsed_us();
   llog(VERBOSE) << " us" << std::endl;
}

UNSWCamera *Vision::camera = NULL;
UNSWCamera *Vision::top_camera = NULL;
UNSWCamera *Vision::bot_camera = NULL;

void Vision::pickPost(std::vector<PostInfo>& posts,
                      std::vector<FieldFeatureInfo>& features) {

   // Needs 1 post
   if (posts.size() == 1 && posts[0].type == PostInfo::pNone) {

      PostInfo p = posts[0];
      std::vector<FieldFeatureInfo>::const_iterator it;
      for (it = features.begin(); it != features.end(); ++it) {

         // And a T-Junction
         if (it->type ==  FieldFeatureInfo::fTJunction) {

            // T-Junction has to be the right distance and orientation
            if (fabs(it->rr.orientation()) > UNSWDEG2RAD(90)) return;

            Point pxy = p.rr.toCartesian();
            Point t = it->rr.toCartesian();
            float d = (((pxy.x() - t.x()) * (pxy.x() - t.x())) +
                 ((pxy.y() - t.y()) * (pxy.y() - t.y())));
            if ((((pxy.x() - t.x()) * (pxy.x() - t.x())) +
                 ((pxy.y() - t.y()) * (pxy.y() - t.y()))) > 500000) {
               return;
            }

            if (p.rr.heading() > it->rr.heading()) {

               // Update post type
               posts[0].type = PostInfo::pRight;
            }

            else if (p.rr.heading() < it->rr.heading()) {
               // Update post type
               posts[0].type = PostInfo::pLeft;
            }
         }
      }
   }
}
