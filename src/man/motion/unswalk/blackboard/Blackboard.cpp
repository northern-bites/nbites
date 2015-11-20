#include "blackboard/Blackboard.hpp"
#include "thread/Thread.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <limits>
#include "motion/SonarRecorder.hpp"
#include "utils/angles.hpp"
#include "utils/Logger.hpp"
#include "utils/options.hpp"

using namespace std;
using namespace boost;

Blackboard::Blackboard(){
      boost::program_options::variables_map vm;
      std::vector<std::string> argv;
      store_and_notify(argv, vm, NULL);
      config = boost::program_options::variables_map(vm);

      mask = INITIAL_MASK;
      readOptions(vm);
      thread.configCallbacks["Blackboard"] = bind(&Blackboard::readOptions, this, _1);
      llog(INFO) << "Initialising the blackboard" << endl;
}

Blackboard::Blackboard(const program_options::variables_map &vm)
   : config(vm), mask(INITIAL_MASK) {
      readOptions(vm);
      thread.configCallbacks["Blackboard"] = bind(&Blackboard::readOptions, this, _1);
      thread.configCallbacks["Logger"] = &Logger::readOptions;
      llog(INFO) << "Initialising the blackboard" << endl;
   }

Blackboard::~Blackboard() {
   thread.configCallbacks["Blackboard"] =
      function<void(const program_options::variables_map &)>();
   llog(INFO) << "Blackboard destroyed" << endl;
}

void Blackboard::readOptions(const program_options::variables_map& config) {
   behaviour.readOptions(config);
   gameController.readOptions(config);
   receiver.readOptions(config);
   kinematics.readOptions(config);
}

BehaviourBlackboard::BehaviourBlackboard() {
   llog(INFO) << "Initialising blackboard: behaviour" << endl;
   readBuf = 0;
}

void BehaviourBlackboard::readOptions(const program_options::variables_map& config) {
   skill = config["behaviour.skill"].as<string>();
   string a = config["default.body"].as<string>();
   if (a == "NONE") request[readBuf].actions.body.actionType = ActionCommand::Body::NONE;
   if (a == "STAND") request[readBuf].actions.body.actionType = ActionCommand::Body::STAND;
   if (a == "WALK") request[readBuf].actions.body.actionType = ActionCommand::Body::WALK;
   if (a == "DRIBBLE") request[readBuf].actions.body.actionType = ActionCommand::Body::DRIBBLE;
   if (a == "INITIAL") request[readBuf].actions.body.actionType = ActionCommand::Body::INITIAL;
   if (a == "KICK") request[readBuf].actions.body.actionType = ActionCommand::Body::KICK;
   if (a == "GETUP_FRONT")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GETUP_FRONT;
   if (a == "GETUP_BACK")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GETUP_BACK;
   if (a == "DEAD") request[readBuf].actions.body.actionType = ActionCommand::Body::DEAD;
   if (a == "REF_PICKUP")
      request[readBuf].actions.body.actionType = ActionCommand::Body::REF_PICKUP;
   if (a == "THROW_IN")
      request[readBuf].actions.body.actionType = ActionCommand::Body::THROW_IN;
   if (a == "GOALIE_SIT")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_SIT;
   if (a == "GOALIE_DIVE_LEFT")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_DIVE_LEFT;
   if (a == "GOALIE_DIVE_RIGHT")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_DIVE_RIGHT;
   if (a == "GOALIE_CENTRE")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_CENTRE;
   if (a == "GOALIE_UNCENTRE")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_UNCENTRE;
   if (a == "GOALIE_STAND")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_STAND;
   if (a == "GOALIE_INITIAL")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_INITIAL;
   if (a == "GOALIE_AFTERSIT_INITIAL")
      request[readBuf].actions.body.actionType = ActionCommand::Body::GOALIE_AFTERSIT_INITIAL;
   request[readBuf].actions.body.forward = config["default.forward"].as<int>();
   request[readBuf].actions.body.left = config["default.left"].as<int>();
   request[readBuf].actions.body.turn = DEG2RAD(config["default.turn"].as<float>());
   request[readBuf].actions.body.speed = DEG2RAD(config["default.speed"].as<float>());
   request[readBuf].actions.body.power = config["default.power"].as<float>();
   request[readBuf].actions.body.kickDirection = DEG2RAD(config["default.kickDirection"].as<float>());

   string foot = config["default.foot"].as<string>();
   if (foot == "LEFT") {
      request[readBuf].actions.body.foot = ActionCommand::Body::LEFT;
   } else {
      request[readBuf].actions.body.foot = ActionCommand::Body::RIGHT;
   }
}

LocalisationBlackboard::LocalisationBlackboard() {
   llog(INFO) << "Initialising blackboard: localisation" << endl;
   robotObstacles.reserve(MAX_ROBOT_OBSTACLES);
   ballLostCount = numeric_limits<uint32_t>::max();
   ballPosRR = RRCoord();
   ballPosRRC = AbsCoord();
   ballVelRRC = AbsCoord();
   ballVel = AbsCoord();
   ballNeckRelative = XYZ_Coord();
   ballPos = AbsCoord();
   teamBall = TeamBallInfo();
   sharedLocalisationBundle = SharedLocalisationUpdateBundle();
   
   // This flag is here so that localisation can know when a shared localisation bundle has
   // been sent so it can reset it.
   havePendingOutgoingSharedBundle = false;
   
   // one flag per teammate, include ourselves for simplicity.
   havePendingIncomingSharedBundle = std::vector<bool>(5, false);
}

VisionBlackboard::VisionBlackboard()
{
   llog(INFO) << "Initialising blackboard: vision" << endl;
   landmarks.reserve(MAX_LANDMARKS);
   feet.reserve(MAX_FEET);
   balls.reserve(MAX_BALLS);
   posts.reserve(MAX_POSTS);
   robots.reserve(MAX_ROBOTS);
   fieldEdges.reserve(MAX_FIELD_EDGES);
   fieldFeatures.reserve(MAX_FIELD_FEATURES);
   vOdometry = Odometry();
   dualOdometry = Odometry();
   missedFrames = 0;
   dxdy = std::make_pair(0,0);
   caughtLeft = false;
   caughtRight = false;
   goalArea = PostInfo::pNone;
   awayGoalProb = 0.5f;
   homeMapSize = 0;
   awayMapSize = 0;

   saliency = NULL;
   currentFrame = NULL;
   topFrame = NULL;
   botFrame = NULL;

   numFieldLinePoints = 0;
}

PerceptionBlackboard::PerceptionBlackboard() {
   kinematics = 0;
   localisation = 0;
   vision = 0;
   behaviour = 0;
   total = 33;
}

MotionBlackboard::MotionBlackboard() {
   llog(INFO) << "Initialising blackboard: motion" << endl;
   uptime = 0;
   std::vector<int> left;
   left.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   std::vector<int> middle;
   middle.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   std::vector<int> right;
   right.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   sonarWindow.reserve(3);
   sonarWindow.push_back(left);
   sonarWindow.push_back(middle);
   sonarWindow.push_back(right);
}

RemoteControlBlackboard::RemoteControlBlackboard() {
	llog(INFO) << "Initialising blackboard: remote control" << endl;
	time_received = 0;
}

KinematicsBlackboard::KinematicsBlackboard() {
   llog(INFO) << "Initialising blackboard: kinematics" << endl;
   std::vector<int> left;
   left.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   std::vector<int> middle;
   middle.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   std::vector<int> right;
   right.reserve(Sonar::NUMBER_OF_READINGS/2*WINDOW_SIZE);
   sonarFiltered.reserve(3);
   sonarFiltered.push_back(left);
   sonarFiltered.push_back(middle);
   sonarFiltered.push_back(right);
}

void KinematicsBlackboard::readOptions(const program_options::variables_map& config) {
   isCalibrating = config["kinematics.isCalibrating"].as<bool>();
   if (!isCalibrating) {
      parameters.cameraYawBottom =
         config["kinematics.cameraYawBottom"].as<float>();
      parameters.cameraPitchBottom =
         config["kinematics.cameraPitchBottom"].as<float>();
      parameters.cameraRollBottom =
         config["kinematics.cameraRollBottom"].as<float>();
      parameters.cameraRollTop =
         config["kinematics.cameraRollTop"].as<float>();
      parameters.cameraYawTop = config["kinematics.cameraYawTop"].as<float>();
      parameters.cameraPitchTop =
         config["kinematics.cameraPitchTop"].as<float>();
      parameters.bodyPitch = config["kinematics.bodyPitch"].as<float>();
   }
}

GameControllerBlackboard::GameControllerBlackboard() {
   llog(INFO) << "Initialising blackboard: gameController" << endl;
   connected = false;
   game_type = MATCH;
   memset(&our_team, 0, sizeof our_team);
   memset(&data, 0, sizeof data);
}

void GameControllerBlackboard::readOptions(const program_options::variables_map& config) {
   connect = config["gamecontroller.connect"].as<bool>();
   player_number = config["player.number"].as<int>();
   string a = config["game.type"].as<string>();
   if (a == "MATCH") game_type = MATCH;
   if (a == "DRIBBLE") game_type = DRIBBLE;
   if (a == "OPEN") game_type = OPEN;
   if (a == "PASSING") game_type = PASSING;
   our_team.teamNumber = config["player.team"].as<int>();
   our_team.teamColour = (int)(config["gamecontroller.ourcolour"].
                               as<string>() == "red");
   team_red = our_team.teamColour;
   our_team.score = config["gamecontroller.ourscore"].as<int>();
   for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
      our_team.players[i].penalty = PENALTY_NONE;
      our_team.players[i].secsTillUnpenalised = 0;
   }
   TeamInfo their_team;
   their_team.teamNumber = config["gamecontroller.opponentteam"].as<int>();
   their_team.teamColour = (our_team.teamColour + 1) % 2;
   their_team.score = config["gamecontroller.opponentscore"].as<int>();
   for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
      their_team.players[i].penalty = PENALTY_NONE;
      their_team.players[i].secsTillUnpenalised = 0;
   }
   map<string, int> gcStateMap;
   gcStateMap["INITIAL"] = STATE_INITIAL;
   gcStateMap["READY"] = STATE_READY;
   gcStateMap["SET"] = STATE_SET;
   gcStateMap["PLAYING"] = STATE_PLAYING;
   gcStateMap["FINISHED"] = STATE_FINISHED;
   if (gcStateMap.count(config["gamecontroller.state"].as<string>())) {
      data.state = gcStateMap[config["gamecontroller.state"].as<string>()];
   } else {
      data.state = STATE_INVALID;
   }
   data.firstHalf = config["gamecontroller.firsthalf"].as<bool>();
   data.kickOffTeam = (int)(config["gamecontroller.kickoffteam"].as<string>()
                            == "red");
   map<string, int> gcSecStateMap;
   gcSecStateMap["NORMAL"] = STATE2_NORMAL;
   gcSecStateMap["PENALTYSHOOT"] = STATE2_PENALTYSHOOT;
   data.secondaryState = gcSecStateMap[
      config["gamecontroller.secondarystate"].as<string>()];
   data.secsRemaining = config["gamecontroller.secsremaining"].as<int>();
   data.teams[our_team.teamColour] = our_team;
   data.teams[their_team.teamColour] = their_team;
}

ReceiverBlackboard::ReceiverBlackboard() {
   BOOST_FOREACH(time_t & lr, lastReceived) {
      lr = 0;
   }
   BOOST_FOREACH(bool &i, incapacitated) {
      i = true;
   }
}

void ReceiverBlackboard::readOptions(const program_options::variables_map& config) {
   team = config["player.team"].as<int>();
}

ThreadBlackboard::ThreadBlackboard() {
   llog(INFO) << "Initialising blackboard: thread" << endl;
}

SynchronisationBlackboard::SynchronisationBlackboard() {
   buttons = new boost::mutex();
   serialization = new boost::mutex();
}
