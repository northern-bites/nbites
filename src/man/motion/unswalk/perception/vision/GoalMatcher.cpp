#include "GoalMatcher.hpp"
#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "utils/basic_maths.hpp"


#define SEARCH_POSITIONS 8 // how many of the top responses to look at
#define MIN_CONSENSUS_DIFF 2 // eg. 4 against 2 if search positions == 6
#define MIN_SAVE_MATCHES 4 // How many hits you need to stop you saving the image again
#define MAP_MAX 40 // maximum number of images to map for each goal
#define WINDOW_SIZE 10 // how big our rolling window of observations is, not so big that it could possibly
// include both goal areas even at maximum robot turn and head turn combined

GoalMatcher::GoalMatcher(){


   gameType = MATCH;
   vocabLoaded = false;
   state = STATE_PLAYING; 
   secondaryState = STATE2_NORMAL;

   wasInitial = false;
   clearMap = true;

   awayMapSize = 0;
   homeMapSize = 0;

   while((int) obs.size() < WINDOW_SIZE){
      obs.push_back(PostInfo::pNone);
   }

}



// Load the visual dictionary for fast matching and starting map of features around the field 
void GoalMatcher::loadVocab(std::string vocabFile){

   tfidf.loadVocab(vocabFile);
   vocabLoaded = true;

}


void GoalMatcher::loadMap(std::string mapFile){

   if (vocabLoaded) tfidf.loadMap(mapFile);

}



int GoalMatcher::classifyGoalArea(VisionFrame &frame, unsigned int *seed, PostInfo::Type &type){

  if ((!vocabLoaded) || (!frame.wordMapped)) return 0; // Check valid data

  type = PostInfo::pNone;
  int num = 0;
  std::priority_queue<MapEntry> matches;

  Eigen::VectorXf query; 
  std::vector< std::vector<float> > query_pixLoc;

  // Augment landmarks with those from a previous matched frame if possible
  if(frame.validOdom){
    query = frame.landmark_tf_aug;
    query_pixLoc = frame.landmark_pixLoc_aug;
  } else {
    query = frame.landmark_tf; 
    query_pixLoc = frame.landmark_pixLoc;
  }

  tfidf.searchDocument(query, query_pixLoc, matches, seed, SEARCH_POSITIONS);

  num = (int) matches.size();
  if (num > MIN_CONSENSUS_DIFF){

    AbsCoord position;
    int i=0;
    int away_goal_votes = 0;
    while(!matches.empty() && i<SEARCH_POSITIONS){
      i++;
      MapEntry entry = matches.top();
      matches.pop();
      
        position = entry.position;      

        if(position.theta() < M_PI/2 && position.theta() > -M_PI/2){
          away_goal_votes++;
        } else {
          away_goal_votes--;
        }
    }

    // Now look for a consensus position 
    if (away_goal_votes >= MIN_CONSENSUS_DIFF) {
      type = PostInfo::pAway;      
      return num;
    } else if (away_goal_votes <= -MIN_CONSENSUS_DIFF) {
      type = PostInfo::pHome;
      return num;
    } 
  }
  return num;  
}




void GoalMatcher::process(VisionFrame &frame, unsigned int *seed){

   if (state == STATE_INITIAL) {
      wasInitial = true;
      clearMap = true;
   } else if (state != STATE_READY) {
      wasInitial = false;
   }

   if (!frame.wordMapped) {
      return; // Check valid data
   }
   if (secondaryState != STATE2_NORMAL){
      return; // system not running in a penalty shoot out
   }
   if ((state != STATE_READY) && (state != STATE_PLAYING) && (state != STATE_SET)){
      return; // only operates in playing, set, ready state
   }

   // Adjust robotPos for head yaw
   AbsCoord position = robotPos;
   if (isnan(position.x()) || isnan(position.y()) || isnan(position.theta())){
      return; // Need to already be localised to one side of the field or the other
   }
   position.theta() = position.theta() + headYaw;

   // Don't do anything if you are not really facing a field end
   if (position.theta() > UNSWDEG2RAD(30) && position.theta() < UNSWDEG2RAD(150)){
      return;
   }
   if (position.theta() < UNSWDEG2RAD(-30) && position.theta() > UNSWDEG2RAD(-150)){
      return;
   }

   if ((state == STATE_READY) && (wasInitial)){
      // saving landmarks mode
      if (clearMap) {
         tfidf.clearMap();
         clearMap = false;
         awayMapSize = 0;
         homeMapSize = 0;
      }
      
      // Don't save landmarks unless you can see the posts, need to be certain
      //if ( !frame.posts.empty() ){ 

         // Save landmarks if see a goal, and it doesn't get a big hit already. 
         PostInfo::Type type = PostInfo::pNone;
         int num_matches = classifyGoalArea(frame, seed, type);
         if (num_matches < MIN_SAVE_MATCHES ){
            // Check which goal we are mapping
            bool awayGoal = false;
            if(position.theta() < M_PI/2 && position.theta() > -M_PI/2){
               awayGoal = true;
            } 

            Eigen::VectorXf tf_doc; 
            std::vector< std::vector<float> > pixLoc;
            // Augment landmarks with those from a previous matched frame if possible
            if(frame.validOdom){
              tf_doc = frame.landmark_tf_aug;
              pixLoc = frame.landmark_pixLoc_aug;
            } else {
              tf_doc = frame.landmark_tf; 
              pixLoc = frame.landmark_pixLoc;
            }

            
            if (awayGoal && awayMapSize < MAP_MAX){
               MapEntry document = MapEntry(position);
               tfidf.addDocumentToCorpus(document, tf_doc, pixLoc);
               vocabLoaded = true;
               llog(DEBUG1) << "\t" << awayMapSize << "\tMapping away goal, position (" << position.x() << ", " << position.y() 
                  << ", " << RAD2DEG(position.theta()) << ")\n";
               awayMapSize++;
            } else if (!awayGoal && homeMapSize < MAP_MAX){
               MapEntry document = MapEntry(position);
               tfidf.addDocumentToCorpus(document, tf_doc, pixLoc);
               vocabLoaded = true;
               llog(DEBUG1) << homeMapSize << "\tMapping home goal, position (" << position.x() << ", " << position.y() 
                  << ", " << RAD2DEG(position.theta()) << ")\n";
               homeMapSize++;
            } else {
               llog(DEBUG1) << "I should map this view but the map is already full for awayGoal = " << awayGoal << "\n";
            }
         } 
      //} 

   } else { // landmark retrieval and goal classification mode

      llog(DEBUG1) << "Landmark retrieval mode\n";
      PostInfo::Type type = PostInfo::pNone;      
      classifyGoalArea(frame, seed, type); 
      frame.goalArea = type;

      obs.insert(obs.begin(), type);
      obs.pop_back();

      float away = 1.f;
      float home = 1.f;
      for(int i=0; i<(int)obs.size(); i++){
         if (obs[i] & PostInfo::pHome) home += 1.f;
         if (obs[i] & PostInfo::pAway) away += 1.f;            
      }
      frame.awayGoalProb = away / (home + away);
   }
}



