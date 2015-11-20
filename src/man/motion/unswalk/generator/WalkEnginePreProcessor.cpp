#include "motion/generator/WalkEnginePreProcessor.hpp"

#define MAX_FORWARD_STEP 90

#define MAX_LEFT_STEP 50

#define MAX_TURN_STEP DEG2RAD(20)

#define FOOT_LENGTH 100
#define FORWARD_GAP 50
#define LEFT_GAP_DRIBBLE 40
#define LEFT_GAP_KICK 55

#define FORWARD_THRESHOLD 25
#define LEFT_THRESHOLD 20
//#define TURN_THRESHOLD DEG2RAD(20)

using namespace ActionCommand;
using namespace std;

int sign(float num) {
   int sign = 1;
   if (num < 0) {
      sign = -1;
   }
   return sign;
}

void toWalkRequest(ActionCommand::All* request) {
   request->body.actionType = Body::WALK;
   request->body.power = 0;
   request->body.bend = 1;
   request->body.speed = 1;
}

// LineUpEngine
WalkEnginePreProcessor::LineUpEngine::LineUpEngine(Walk2014Generator* walkEngine) {
   hasStarted = false;
   foot = Body::LEFT;
   this->walkEngine = walkEngine;
}

void WalkEnginePreProcessor::LineUpEngine::start(Body::Foot foot) {
   hasStarted = true;
   this->foot = foot;
}

void WalkEnginePreProcessor::LineUpEngine::reset() {
   hasStarted = false;
   walkEngine->exactStepsRequested = false;
}

bool WalkEnginePreProcessor::LineUpEngine::hasEnded(ActionCommand::All* request, float ballX, float ballY) {
   // Calculate required left gap (needs to be further out for kicks)
   int leftGap = LEFT_GAP_DRIBBLE;
   if (request->body.actionType == Body::KICK) {
      leftGap = LEFT_GAP_KICK;
   }
   int gapY = ballY - leftGap;
   if (foot == Body::RIGHT) {
      gapY = ballY + leftGap;
   }
   int gapX = ballX - FOOT_LENGTH - FORWARD_GAP - max(walkEngine->forwardL,walkEngine->forwardR)*1000;

//   if (abs(gapX) < 100 && abs(gapY) < 100 && fabs(request->body.turn) < 0.2) {
//      cout << "line up hasEnded: " << gapX << " " << gapY << " " << RAD2DEG(request->body.turn) << " " << walkEngine->t << endl;
//   }

   return (abs(gapX) < FORWARD_THRESHOLD && abs(gapY) < LEFT_THRESHOLD
         && fabs(request->body.turn) < request->body.speed);   //speed is overloaded for behaviour input turn threshold
}

void WalkEnginePreProcessor::LineUpEngine::preProcess(ActionCommand::All* request,
      float ballX,
      float ballY) {
   int forward = ballX - FOOT_LENGTH - FORWARD_GAP - max(walkEngine->forwardL,walkEngine->forwardR)*1000;
   // Calculate required left gap (needs to be further out for kicks)
   int leftGap = LEFT_GAP_DRIBBLE;
   if (request->body.actionType == Body::KICK) {
      leftGap = LEFT_GAP_KICK;
   }
   int left = ballY - leftGap;
   if (foot == Body::RIGHT) {
      left = ballY + leftGap;
   }

   request->body.forward = sign(forward) * min(MAX_FORWARD_STEP, abs(forward));
   request->body.left = sign(left) * min(MAX_LEFT_STEP, abs(left));

   // don't turn further than 30 degrees away from the ball heading
   float heading = atan2(ballY, ballX);
   if (NORMALISE(request->body.turn - heading) > DEG2RAD(30)) {
      request->body.turn = NORMALISE(DEG2RAD(30) + heading);
   } else if (NORMALISE(request->body.turn - heading) < -DEG2RAD(30)) {
      request->body.turn = NORMALISE(-DEG2RAD(30) + heading);
   }
   request->body.turn = sign(request->body.turn) * min(MAX_TURN_STEP, fabs(request->body.turn/2));

   toWalkRequest(request);
   walkEngine->exactStepsRequested = true;
}

// DribbleEngine
WalkEnginePreProcessor::DribbleEngine::DribbleEngine(Walk2014Generator* walkEngine) {
   this->walkEngine = walkEngine;
   foot = Body::LEFT;
   dribbleState = DribbleEngine::END;
}

void WalkEnginePreProcessor::DribbleEngine::reset() {
   dribbleState = DribbleEngine::END;
   walkEngine->exactStepsRequested = false;
}

bool WalkEnginePreProcessor::DribbleEngine::hasEnded() {
   return (dribbleState == DribbleEngine::END);
}

void WalkEnginePreProcessor::DribbleEngine::start(Body::Foot foot) {
   dribbleState = DribbleEngine::INIT;
   this->foot = foot;
}

void WalkEnginePreProcessor::DribbleEngine::preProcess(ActionCommand::All* request,
      BodyModel &bodyModel) {
   int direction = 1;
   bool leftTurnPhase = true;
   if (foot == Body::RIGHT) {
      direction = -1;
      leftTurnPhase = false;
   }

   //do transition
   if (dribbleState == DribbleEngine::INIT && bodyModel.isLeftPhase == leftTurnPhase
         && walkEngine->t == 0) {
      dribbleState = DribbleEngine::TURN;
   } else if (dribbleState == DribbleEngine::TURN && bodyModel.isLeftPhase != leftTurnPhase
         && walkEngine->t == 0) {
      dribbleState = DribbleEngine::FORWARD;
   } else if (dribbleState == DribbleEngine::FORWARD && bodyModel.isLeftPhase == leftTurnPhase
         && walkEngine->t == 0) {
      dribbleState = DribbleEngine::END;
      dribbleTimer.restart();
   }

   // set request
   request->body.left = 0;
   if (dribbleState == DribbleEngine::TURN) {
      request->body.forward = 0;
      request->body.turn = direction * DEG2RAD(40);
   } else if (dribbleState == DribbleEngine::FORWARD) {
      request->body.forward = 140;
      request->body.turn = 0;
   } else if(dribbleState == DribbleEngine::END) {
      request->body.forward = 70;
      request->body.turn = 0;
   } else {
      request->body.forward = 1; // hack so walk doesn't stand
      request->body.turn = 0;
   }
   toWalkRequest(request);
   walkEngine->exactStepsRequested = true;

//   cout  << "dribbling: " << walkEngine->t << " " << dribbleState << endl;
}


WalkEnginePreProcessor::WalkEnginePreProcessor() {
   walkEngine = new Walk2014Generator();
   lineUpEngine = new LineUpEngine(walkEngine);
   dribbleEngine = new DribbleEngine(walkEngine);
   isKicking = false;
}

WalkEnginePreProcessor::~WalkEnginePreProcessor() {
   delete walkEngine;
   delete lineUpEngine;
   delete dribbleEngine;
}

bool isLineUpRequired(Body::ActionType actionType) {
   return (actionType == Body::DRIBBLE || actionType == Body::KICK);
}

JointValues WalkEnginePreProcessor::makeJoints(ActionCommand::All* request,
                                          Odometry* odometry,
                                          const SensorValues &sensors,
                                          BodyModel &bodyModel,
                                          float ballX,
                                          float ballY) {
   Body::ActionType active = request->body.actionType;

   // don't try to dribble again within timer
   if (active == Body::DRIBBLE && dribbleEngine->dribbleTimer.elapsed_ms() < 400) {
      toWalkRequest(request);
      active = Body::WALK;
      request->body.forward = 1;
      request->body.left = 0;
      request->body.turn = 0;
   }

   // persist until current state ends
   if (!isKicking) {
      if (!dribbleEngine->hasEnded()) {
         dribbleEngine->preProcess(request, bodyModel);
         active = Body::DRIBBLE;
      } else {
         dribbleEngine->reset();

         // line up is on demand, can be interrupted
         if (isLineUpRequired(request->body.actionType)) {
            // start line up
            if (!lineUpEngine->hasStarted) {
               lineUpEngine->start(request->body.foot);
            }

            if (!lineUpEngine->hasEnded(request, ballX, ballY)) {
               // do line up
               lineUpEngine->preProcess(request, ballX, ballY);
               active = Body::LINE_UP;
            } else {
               lineUpEngine->reset();

               if (request->body.actionType == Body::DRIBBLE) {
                  // start dribble
                  dribbleEngine->start(request->body.foot);
                  dribbleEngine->preProcess(request, bodyModel);
                  active = Body::DRIBBLE;
               } else if (request->body.actionType == Body::KICK) {
                  // walkEngine will set request back to walk after it's done kicking
                  // don't preProcess anything in the mean time
                  isKicking = true;
               }
            }
         } else {
            lineUpEngine->reset();
         }
      }
   }

   if (request->body.actionType == Body::KICK) {
      request->body.turn = 0;    // 0 the turn used for line up heading adjustments
      request->body.speed = 0;   // reverted overloaded param for turn threshold
   }

   JointValues joints = walkEngine->makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
   
//   cout << "walk option: " << walkEngine->walk2014Option << " " << active << " " << request->body.actionType << endl;
//   cout << "kick state " << kickState << endl;

   // walkEngine sets kick to walk2014 after kick finishes
   if (walkEngine->active.actionType == Body::KICK && request->body.actionType == Body::WALK) {
      isKicking = false;
   } else {
      // **HACK ALERT** Don't let ref pickup override kick. for mario in particular since foot sensors zero out during kick
      if (isKicking && active == Body::REF_PICKUP) {
         request->body.actionType = Body::KICK;
      } else {
         request->body.actionType = active;
      }
   }

   return joints;
}

bool WalkEnginePreProcessor::isActive() {
   return walkEngine->isActive() || !dribbleEngine->hasEnded();
}

void WalkEnginePreProcessor::readOptions(boost::program_options::variables_map& config) {
   walkEngine->readOptions(config);
}

void WalkEnginePreProcessor::reset() {
   walkEngine->reset();
   lineUpEngine->reset();
   dribbleEngine->reset();
   isKicking = false;
}

void WalkEnginePreProcessor::stop() {
   walkEngine->stop();
}

