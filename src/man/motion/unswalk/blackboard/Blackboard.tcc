#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

template<class T>
const T& Blackboard::read(const T *component) {
   return *component;
}

template<class T>
void Blackboard::write(T *component, const T& value) {
   *component = value;
}

BOOST_CLASS_VERSION(Blackboard, 16);

template<class Archive>
void Blackboard::shallowSerialize(Archive & ar,
                                  const unsigned int version) {

   if (version < 15) {
      throw std::runtime_error("Depricated 2011 dump file detected");
   }

   ar & gameController.team_red;
   ar & gameController.player_number;

   ar & motion.sensors;
   ar & motion.pose;
   ar & motion.com;
   ar & motion.odometry;
   ar & motion.active;

   ar & perception.behaviour;
   ar & perception.kinematics;
   ar & perception.localisation;
   ar & perception.total;
   ar & perception.vision;

   ar & behaviour.request;

   ar & kinematics.sonarFiltered;
   ar & kinematics.parameters;
   ar & kinematics.sensorsLagged;

   if (this->mask & ROBOT_FILTER_MASK) {
      ar & localisation.robotObstacles;
   } else {
      std::vector<RobotInfo> empty;
      ar & empty;
   }

   if (this->mask & LANDMARKS_MASK) {
      ar & vision.landmarks;
   } else {
      std::vector<Ipoint> empty;
      ar & empty;
   }

   ar & vision.timestamp;
   ar & vision.caughtLeft;
   ar & vision.caughtRight;
   ar & vision.goalArea;
   ar & vision.awayGoalProb;
   ar & vision.homeMapSize;
   ar & vision.awayMapSize;
   ar & vision.feet;
   ar & vision.balls;
   ar & vision.ballHint;
   ar & vision.posts;
   ar & vision.robots;
   ar & vision.fieldEdges;
   ar & vision.fieldFeatures;
   ar & vision.vOdometry;
   ar & vision.dualOdometry;
   ar & vision.missedFrames;
   ar & vision.dxdy;

   ar & receiver.data;
   ar & receiver.incapacitated;

   ar & motion.pendulumModel;

   ar & localisation.robotPos;
   ar & localisation.ballLostCount;
   ar & localisation.ballPosRR;
   ar & localisation.ballPosRRC;
   ar & localisation.ballVelRRC;
   if (version >= 16) {
      ar & localisation.ballVel;
      ar & localisation.ballPosUncertainty;
      ar & localisation.ballVelEigenvalue;
      ar & localisation.robotPosUncertainty;
      ar & localisation.robotHeadingUncertainty;
   }
   ar & localisation.ballNeckRelative;
   ar & localisation.ballPos;
   ar & localisation.teamBall;
   ar & localisation.sharedLocalisationBundle;
   ar & localisation.havePendingOutgoingSharedBundle;
   ar & localisation.havePendingIncomingSharedBundle;
}

template<class Archive>
void Blackboard::save(Archive & ar, const unsigned int version) const {
   // note, version is always the latest when saving
   OffNaoMask_t mask = this->mask;
   if ((mask & SALIENCY_MASK) && (!vision.topSaliency || !vision.botSaliency))
      mask &= (~SALIENCY_MASK);
   if ((mask & RAW_IMAGE_MASK) && (!vision.topFrame || !vision.botFrame))
      mask &= (~RAW_IMAGE_MASK);
   ar & boost::serialization::make_nvp("Mask", mask);

   if ((mask & BLACKBOARD_MASK) && (mask & SALIENCY_MASK)) {
      locks.serialization->lock();
      ((Blackboard*)this)->shallowSerialize(ar, version);
      // TODO(jayen): RLE
      ar & boost::serialization::
      make_nvp("TopSaliency",
               boost::serialization::
               make_binary_object(vision.topSaliency,
                                  sizeof(Colour[IMAGE_COLS / TOP_SALIENCY_DENSITY][IMAGE_ROWS / TOP_SALIENCY_DENSITY])));
      ar & boost::serialization::
      make_nvp("BotSaliency",
               boost::serialization::
               make_binary_object(vision.botSaliency,
                                  sizeof(Colour[IMAGE_COLS / BOT_SALIENCY_DENSITY][IMAGE_ROWS / BOT_SALIENCY_DENSITY])));
      locks.serialization->unlock();
   } else if (mask & BLACKBOARD_MASK) {
      ((Blackboard*)this)->shallowSerialize(ar, version);
   } else if (mask & SALIENCY_MASK) {
      // TODO(jayen): RLE
      ar & boost::serialization::
      make_nvp("TopSaliency",
               boost::serialization::
               make_binary_object(vision.topSaliency,
                                  sizeof(Colour[IMAGE_COLS / TOP_SALIENCY_DENSITY][IMAGE_ROWS / TOP_SALIENCY_DENSITY])));
      ar & boost::serialization::
      make_nvp("BotSaliency",
               boost::serialization::
               make_binary_object(vision.botSaliency,
                                  sizeof(Colour[IMAGE_COLS / BOT_SALIENCY_DENSITY][IMAGE_ROWS / BOT_SALIENCY_DENSITY])));
   }
   if (mask & RAW_IMAGE_MASK) {
      // TODO(jayen): zlib
      ar & boost::serialization::make_nvp(
            "Top Raw Image",
            boost::serialization:: make_binary_object((void *)vision.topFrame,
            sizeof(uint8_t[IMAGE_ROWS * IMAGE_COLS * 2])));
      ar & boost::serialization::make_nvp(
           "Bot Raw Image",
            boost::serialization:: make_binary_object((void *)vision.botFrame,
            sizeof(uint8_t[IMAGE_ROWS * IMAGE_COLS * 2])));
   }

   ar & localisation.robotPos;
}

template<class Archive>
void Blackboard::load(Archive & ar, const unsigned int version) {
   ar & mask;
   if (mask & BLACKBOARD_MASK)
      shallowSerialize(ar, version);
   if (mask & SALIENCY_MASK) {
      vision.topSaliency = (Colour*) new
                  Colour[IMAGE_COLS / TOP_SALIENCY_DENSITY]
                        [IMAGE_ROWS / TOP_SALIENCY_DENSITY];
      // TODO(jayen): RLE
      ar & boost::serialization::make_binary_object(vision.topSaliency,
                                                    sizeof(Colour[IMAGE_COLS / TOP_SALIENCY_DENSITY][IMAGE_ROWS / TOP_SALIENCY_DENSITY]));

      vision.botSaliency = (Colour*) new
                  Colour[IMAGE_COLS / BOT_SALIENCY_DENSITY]
                        [IMAGE_ROWS / BOT_SALIENCY_DENSITY];
      // TODO(jayen): RLE
      ar & boost::serialization::make_binary_object(vision.botSaliency,
                                                    sizeof(Colour[IMAGE_COLS / BOT_SALIENCY_DENSITY][IMAGE_ROWS / BOT_SALIENCY_DENSITY]));
   }
   if (mask & RAW_IMAGE_MASK) {
      vision.topFrame = new uint8_t[IMAGE_ROWS * IMAGE_COLS * 2];
      ar & boost::serialization::
      make_binary_object((void *)vision.topFrame,
                         sizeof(uint8_t[IMAGE_ROWS * IMAGE_COLS * 2]));
      vision.botFrame = new uint8_t[IMAGE_ROWS * IMAGE_COLS * 2];
      ar & boost::serialization::
      make_binary_object((void *)vision.botFrame,
                         sizeof(uint8_t[IMAGE_ROWS * IMAGE_COLS * 2]));
   }

   ar & localisation.robotPos;
}

namespace boost {
   namespace serialization {
      /* // boost docs are broken
      template<class Archive>
      inline void save_construct_data(Archive &ar, const Blackboard *t,
                                      const unsigned int file_version) {
         // save data required to construct instance
         ar << t->config;
      }
      */
      template<class Archive>
      inline void load_construct_data(Archive &ar, Blackboard *t,
                                      const unsigned int file_version) {
         // retrieve data from archive required to construct new instance
         boost::program_options::variables_map config;
         ar >> config;
         // invoke inplace constructor to initialize instance of Blackboard
         ::new(t) Blackboard(config);
      }
   }
}  // namespace ...
