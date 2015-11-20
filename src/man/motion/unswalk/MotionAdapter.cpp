#include "motion/MotionAdapter.hpp"

#include <boost/bind.hpp>

#ifdef SIMULATION
   #include "motion/touch/SimTouch.hpp"
   #include "motion/effector/SimEffector.hpp"
#else
   #include "motion/touch/AgentTouch.hpp"
   #include "motion/effector/AgentEffector.hpp"
#endif
#include "motion/touch/NullTouch.hpp"
#include "motion/touch/FilteredTouch.hpp"
#include "motion/effector/NullEffector.hpp"
#include "motion/generator/ClippedGenerator.hpp"
#include "motion/generator/DistributedGenerator.hpp"
#include "blackboard/Blackboard.hpp"
#include "thread/Thread.hpp"
#include "utils/Logger.hpp"
#include "utils/body.hpp"
#include "utils/incapacitated.hpp"
#include "types/ActionCommand.hpp"
#include "types/JointValues.hpp"
#include "types/SensorValues.hpp"
#include "utils/Timer.hpp"

// Empirically determined lags to get everything to match vision
#define ODOMETRY_LAG 8 // buffer the odometry by 8 motion ticks and it synchronises well with vision
#define SENSOR_LAG 8 // buffer the sensors/pose for same reason, but less since it doesn't wait for command to execute


using namespace std;

void construct(Touch** touch, std::string name) {
  #ifdef SIMULATION
      if (name == "Sim") *touch = (Touch*) new SimTouch();
  #else
      if (name == "Agent") *touch = (Touch*) new AgentTouch();
  #endif
   if (name == "Null") *touch = (Touch*) new NullTouch();
   if (*touch == NULL)
      llog(FATAL) << "MotionAdapter: NULL " + name + "Touch" << endl;
}

void construct(Effector** effector, std::string name) {
  #ifdef SIMULATION
      if (name == "Sim") *effector = (Effector*) new SimEffector();
  #else
      if (name == "Agent") *effector = (Effector*) new AgentEffector();
  #endif
   if (name == "Null") *effector = (Effector*) new NullEffector();
   if (*effector == NULL)
      llog(FATAL) << "MotionAdapter: NULL " + name + "Effector" << endl;
}

/*-----------------------------------------------------------------------------
 * Motion thread constructor
 *---------------------------------------------------------------------------*/
MotionAdapter::MotionAdapter(Blackboard *bb)
   : Adapter(bb), uptime(0) {
   llog(INFO) << "Constructing MotionAdapter" << endl;

   // Initialise the buffers
   odometryBuffer.clear();
   sensorBuffer.clear();
   while(odometryBuffer.size() < ODOMETRY_LAG){
      odometryBuffer.push_back(Odometry());
   }

   // We only construct the NullTouch/Generators, the rest are done on demand
   touches["Null"] = (Touch*)(new NullTouch());
   if (touches["Null"] == NULL) {
      llog(FATAL) << "MotionAdapter: NULL NullTouch" << endl;
   }

#ifdef SIMULATION
   touches["Sim"] = (Touch*)(NULL);
#else
   touches["Agent"] = (Touch*)(NULL);
#endif

   nakedTouch = touches["Null"];
   touch = (Touch*) new FilteredTouch(touches["Null"]);

   generator = (Generator*) new ClippedGenerator(
      (Generator*) new DistributedGenerator());
   if (generator == NULL) {
      llog(FATAL) << "MotionAdapter: NULL Generator" << endl;
   }

   effectors["Null"] = (Effector*)(new NullEffector());
   if (effectors["Null"] == NULL) {
      llog(FATAL) << "MotionAdapter: NULL NullEffector" << endl;
   }
#ifdef SIMULATION
   effectors["Sim"] = (Effector*)(NULL);
#else
   effectors["Agent"] = (Effector*)(NULL);
#endif
   effector = effectors["Null"];

   readOptions(bb->config);
   touch->readOptions(bb->config); // XXX: Do NOT put this in readOptions or robot will fall

   //writeTo(thread, configCallbacks[Thread::name],
   //        boost::function<void(const boost::program_options::variables_map &)>
   //        (boost::bind(&MotionAdapter::readOptions, this, _1)));

   llog(INFO) << "MotionAdapater constructed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * Motion thread destructor
 *---------------------------------------------------------------------------*/
MotionAdapter::~MotionAdapter() {
   llog(INFO) << "Destroying MotionAdapter" << endl;

   writeTo(thread, configCallbacks[Thread::name], boost::function<void(const boost::program_options::variables_map &)>());

   for (std::map<std::string, Touch*>::iterator it = touches.begin();
        it != touches.end(); it++) {
      delete it->second;
   }

   delete generator;

   for (std::map<std::string, Effector*>::iterator it = effectors.begin();
        it != effectors.end(); it++) {
      delete it->second;
   }
}

/*-----------------------------------------------------------------------------
 * read motion options
 *---------------------------------------------------------------------------*/
void MotionAdapter::readOptions(const boost::program_options::variables_map& config) {
   std::string e = config["motion.effector"].as<string>();
   std::string t = config["motion.touch"].as<string>();
   llog(INFO) << "MotionAdapter using effector " << e << " and touch " << t;

   // Look through the list of touches for the one requested,
   // initialize it if it exists.
   if (touches.count(t)) {
      if (touches[t] == NULL) {
         construct(&touches[t], t);
      }
      nakedTouch = touches[t];
      touch = (Touch*) new FilteredTouch(touches[t]);
   }

   // Look through the list of effectors for the one requested,
   // initialize it if it exists.
   if (effectors.count(e)) {
      if (effectors[e] == NULL) { 
         construct(&effectors[e], e);
      }
      effector = effectors[e];
   }

   // Read the generator options
   generator->readOptions(config);
}

/*-----------------------------------------------------------------------------
 * Motion thread tick function
 *---------------------------------------------------------------------------*/
void MotionAdapter::tick() {
   Timer t;

   // Get the motion request from behaviours
   int behaviourReadBuf = readFrom(behaviour, readBuf);
   ActionCommand::All request = readFrom(behaviour, request[behaviourReadBuf]).actions;

   // Get sensor information from kinematics
   SensorValues sensors;
   if(request.body.actionType == Body::MOTION_CALIBRATE){
       // raw sensor values are sent to offnao for calibration
       // these values are straight forward copy paste into pos files
       sensors = nakedTouch->getSensors(kinematics);
       sensors.sensors[Sensors::InertialSensor_AngleX] = -RAD2DEG(sensors.sensors[Sensors::InertialSensor_AngleX]);
       sensors.sensors[Sensors::InertialSensor_AngleY] = -RAD2DEG(sensors.sensors[Sensors::InertialSensor_AngleY]);
       sensors.sensors[Sensors::InertialSensor_GyrX] = -sensors.sensors[Sensors::InertialSensor_GyrX];
       sensors.sensors[Sensors::InertialSensor_GyrY] = -sensors.sensors[Sensors::InertialSensor_GyrY];
   } else {
       sensors = touch->getSensors(kinematics);
   }

   // For kinematics, give it the lagged sensorValues with the most recent lean angles (because they already
   // have a lag in them) unless it's the very first one otherwise it will propagate nans everywhere
   SensorValues sensorsLagged;
   if (sensorBuffer.size() > 0) {
      sensorsLagged = sensorBuffer.back(); 
   } else {
      sensorsLagged = sensors; 
   }
   sensorsLagged.sensors[Sensors::InertialSensor_AngleX] = sensors.sensors[Sensors::InertialSensor_AngleX];
   sensorsLagged.sensors[Sensors::InertialSensor_AngleY] = sensors.sensors[Sensors::InertialSensor_AngleY];
   kinematics.setSensorValues(sensorsLagged);
   kinematics.parameters = readFrom(kinematics, parameters);
   // Calculate the Denavit-Hartenberg chain
   kinematics.updateDHChain();
   writeTo(motion, pose, kinematics.getPose());

   bool standing = touch->getStanding();
   ButtonPresses buttons = touch->getButtons();
   llog(VERBOSE) << "touch->getSensors took "
                 << t.elapsed_ms() << "ms" << std::endl;
   t.restart();

   // Keep a running time for standing
   if (standing) {
      uptime = 0.0f;
   } else {
      uptime += 0.01f;
   }
   writeTo(motion, uptime, uptime);

   // Sensors are lagged so it correctly synchronises with vision
   sensorBuffer.insert(sensorBuffer.begin(), sensors);
   writeTo(motion, sensors, sensors); //Lagged);
   writeTo(kinematics, sensorsLagged, sensorsLagged);
//   std::cout << "live = " << sensors.joints.angles[Joints::HeadYaw]
//             << " delayed = " << sensorsLagged.joints.angles[Joints::HeadYaw]
//             << std::endl;

   if (sensorBuffer.size() > SENSOR_LAG) {
      sensorBuffer.pop_back();
   }

   // sonar recorder gets and update and returns the next sonar request
   request.sonar = sonarRecorder.update(sensors.sonar);
   writeTo(motion, sonarWindow, sonarRecorder.sonarWindow);

   if (isIncapacitated(request.body.actionType)) {
      uptime = 0.0f;
   }
   buttons |= readFrom(motion, buttons);
   writeTo(motion, buttons, buttons);
   Odometry odo = Odometry(odometryBuffer.front());

   llog(VERBOSE) << "writeTo / readFrom took "
                 << t.elapsed_ms() << "ms" << std::endl;
   t.restart();

   if (standing) {
      generator->reset();
      request.body = ActionCommand::Body::INITIAL;
      odo.clear();
   }

   // When the robot sees another near its side, it lets its arms go limp, so as not
   // to "push" them.
   bool caughtLR = (readFrom(vision, caughtLeft) || readFrom(vision, caughtRight));
   request.body.caughtLeft = caughtLR;
   request.body.caughtRight = caughtLR;

   // Get the position of the ball in robot relative cartesian coordinates
   
   AbsCoord robotPose = readFrom(localisation, robotPos);
   AbsCoord ballAbs = readFrom(localisation, ballPos);
   AbsCoord ball = ballAbs.convertToRobotRelativeCartesian(robotPose);

   // Update the body model
   bodyModel.kinematics = &kinematics;
   bodyModel.update(&odo, sensors);
   // Get the joints requested by whichever generator we're using
   JointValues joints = generator->makeJoints(&request, &odo, sensors, bodyModel, ball.x(), ball.y());

   // Save the body model Center of Mass
   writeTo(motion, com, bodyModel.getCoM());

   writeTo(motion, active, request);

   // Odometry is lagged so it correctly synchronises with vision
   odometryBuffer.insert(odometryBuffer.begin(), Odometry(odo));
   writeTo(motion, odometry, Odometry(odometryBuffer.back()));
   odometryBuffer.pop_back();

   // Save the pendulum model
   writeTo(motion, pendulumModel, bodyModel.getPendulumModel());
   llog(VERBOSE) << "generator->makeJoints took "
                 << t.elapsed_ms() << "ms" << std::endl;
   t.restart();

   // Actuate joints as requested.
   effector->actuate(joints, request.leds, request.sonar);
   llog(VERBOSE) << "effector->actuate took "
                 << t.elapsed_ms() << "ms" << std::endl;
}

