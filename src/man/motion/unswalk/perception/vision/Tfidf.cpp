
#include <iostream>
#include <cmath>
#include <limits>

#include "Tfidf.hpp"
#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "utils/basic_maths.hpp"


# define VALID_COSINE_SCORE 0.40f // 0.42f
# define VALID_INLIERS 40 // 50

// Loads vocab ready for use
void Tfidf::loadVocab(std::string vocabFile){

  // clear everything
  clearData();

  // load the vocab file
  vocab.loadVocabFile(vocabFile);
  T = vocab.getSize();
  ni = Eigen::VectorXf::Zero(T);	
  llog(VERBOSE) << "Loaded vocab of " << T << " words\n"; 

}

void Tfidf::clearMap(){
   clearData();
   T = vocab.getSize();
   ni = Eigen::VectorXf::Zero(T);	
}

// Loads map ready for use (needs a vocab first)
void Tfidf::loadMap(std::string mapFile){

  if(T!=0){
    // clear previous map
    clearMap();
    
    // load the map file
    std::vector<MapEntry> tempMap;
    std::ifstream ifs(mapFile.c_str());
    if (ifs.is_open()){  
      boost::archive::text_iarchive ia(ifs);
      ia >> tempMap;
    } else {
      throw std::runtime_error("error opening map file in tfidf");
    }
    for (int i=0; i<(int)tempMap.size(); i++){
      addDocumentToCorpus(tempMap[i]);
    }
    llog(VERBOSE) << "Loaded map with " << map.size() << " entrys\n";

   }   
}

// Saves map, including any new entries
void Tfidf::saveMap(std::string mapFile){
  
  std::ofstream ofs(mapFile.c_str());
  {
  boost::archive::text_oarchive oa(ofs);
  oa << map;
  }

}


//! Adds to the searchable collection, return true if successful
bool Tfidf::addDocumentToCorpus(MapEntry document){

   if(vocab.getSize() != 0){
      std::vector< std::vector<float> > pixLoc; 
      Eigen::VectorXf tf_doc = vocab.mapToVec(document.ipoints, pixLoc);
      return addDocumentToCorpus(document, tf_doc, pixLoc);      
   } 
   return false;

}

//! Faster version if landmarks have already been mapped to words (with the same vocab file)
bool Tfidf::addDocumentToCorpus(MapEntry document, Eigen::VectorXf tf_doc, std::vector< std::vector<float> > pixLoc){

  bool result = false;

   if(vocab.getSize() != 0){

      if(tf_doc.sum() != 0){  // don't let an empty document be added
	      tf.push_back(tf_doc);
         pixels.push_back(pixLoc);
	      ni = ni + tf_doc;	
	      N++;
	      nd.push_back(tf_doc.sum());
	
	      map.push_back(document);
	      //! Recalculate the inverse document frequency (log(N/ni))
	      idf = (ni.cwise().inverse() * N ).cwise().log();

         // Remove Inf from idf (can occur with random initialised learned words)
         for(int i=0; i<T; i++){
            if(idf[i] == std::numeric_limits<float>::infinity()){
               idf[i] = 0.f;
            }
         }
         result = true;
      }
   } 
   return result;

}


int Tfidf::getSize(){
   return N;
}


//! Gets the best matches based on the cosine of tf-idf
void Tfidf::searchDocument(std::vector<Ipoint> query, std::priority_queue<MapEntry> &matches, unsigned int *seed, int n){

  Timer t;
  t.restart();

  if(vocab.getSize() != 0){
	 // First get the tf-idf vector for the query
    std::vector< std::vector<float> > query_pixLoc; // pixel locations of the words 
    Eigen::VectorXf tf_query = vocab.mapToVec(query, query_pixLoc);

    llog(VERBOSE) << "Mapping ipoints to tfidf query vector took " << t.elapsed_us() << " us" << std::endl;
    searchDocument(tf_query, query_pixLoc, matches, seed, n);
  }
  return;
}



//! Faster version if landmarks have already been mapped to words
void Tfidf::searchDocument(Eigen::VectorXf tf_query, 
                      std::vector< std::vector<float> > query_pixLoc, // pixel locations of the words 
                      std::priority_queue<MapEntry> &matches, 
                      unsigned int *seed,
                      int n){
   Timer t;
   t.restart();
  
   if(tf_query.sum() != 0 && N != 0){ // checked the document is not empty and corpus not empty

      std::priority_queue< std::pair<MapEntry, std::vector <std::vector<float> > > > queue;
      Eigen::VectorXf tfidf_query = (tf_query / tf_query.sum() ).cwise() * idf;

	   // Now compute the cosines against each document -- inverted index not used since our vectors are not sparse enough
	   Eigen::VectorXf tfidf_doc;
      llog(DEBUG1) << "Cosine scores:\n";
	   for (int i=0; i<N; i++){
		    tfidf_doc = (tf[i] / nd[i] ).cwise() * idf; // nd[i] can't be zero or it wouldn't have been added
		    map[i].score = tfidf_query.dot(tfidf_doc) / ( tfidf_query.norm()*tfidf_doc.norm() );
        if (map[i].score > VALID_COSINE_SCORE) {
          llog(DEBUG1) << "Cos: " << map[i].score << ", ";
          queue.push( std::make_pair(map[i], pixels[i]) );
        }
      }
      llog(DEBUG1) << "\n";

      // Now do geometric validation on the best until we have enough or the queue is empty
      while(!queue.empty() && matches.size() < (unsigned int)n){
          
          MapEntry mapEntry = queue.top().first;
          llog(DEBUG1) << "Validating Cos: " << mapEntry.score << ", ";
          std::vector< std::vector<float> > pixLoc = queue.top().second;
          queue.pop();

          // Do geometric validation - first build the points to run ransac
          std::vector<Point> matchpoints;
          for (int j=0; j<T; j++){
            for(uint32_t m=0; m<pixLoc[j].size(); m++){
              for(uint32_t n=0; n<query_pixLoc[j].size(); n++){
                matchpoints.push_back(Point(pixLoc[j][m], query_pixLoc[j][n]));
              }
            }
          }
    
          // ransac
          RANSACLine resultLine;
          uint16_t min_points = 2;
          std::vector<bool> *con, consBuf[2];
          consBuf[0].resize(matchpoints.size());
          consBuf[1].resize(matchpoints.size());
          float slopeConstraint = 2.f; // reject scale changes more than twice or half the distance
          bool ransacresult = RANSAC::findLineConstrained(matchpoints, &con, resultLine, MATCH_ITERATIONS, 
              PIXEL_ERROR_MARGIN, min_points, consBuf, seed, slopeConstraint);        
          
          if(ransacresult && (resultLine.t2 != 0.f)){  // check t2 but should be fixed by slope constraint anyway
            // count the inliers
            int inliers = 0;
        	   for (int v = 0; v < (int)matchpoints.size(); v++) {
              if ( (*con)[v]) {
					      inliers++;
              }
            }
            llog(DEBUG1) << "found " << inliers << " inliers, " << matchpoints.size() - inliers << " outliers, ";
            llog(DEBUG1) << "at (x,y,theta) loc: (" << mapEntry.position.x() << ", " << mapEntry.position.y() << ", " 
                  << mapEntry.position.theta() << ")\n";

            if(inliers >= VALID_INLIERS){
              llog(DEBUG1) << "Location is valid\n";
/*
              // Calculate rotation matrix from RR to ABS
              float sintheta = sin(mapEntry.position.theta());
              float costheta = cos(mapEntry.position.theta());      
              Eigen::Matrix<float, 3, 3> R;
              R << costheta, -sintheta, 0, sintheta, costheta, 0, 0, 0, 1;

              // Calculate a turn adjustment and make allowance for robot head movement
              float start = (IMAGE_COLS/SURF_SUBSAMPLE/2);
              float t1 = static_cast<float>(resultLine.t1);
              float t2 = static_cast<float>(resultLine.t2);
              float t3 = static_cast<float>(resultLine.t3);
		          float end = -t1 / t2 * start - t3 / t2;
              // this includes a turn fudge -- might need to tune this a bit more
		          float turn = 1.f*((end - start)/(IMAGE_COLS/SURF_SUBSAMPLE))*IMAGE_HFOV;
              turn  -=  headYaw; 

              // Calculate a forwards / backwards adjustment, assuming landmarks are 2m behind the goal
              float distanceToLandmarks = 0.f;
              if (mapEntry.position.theta() < M_PI/2.f && mapEntry.position.theta() > -M_PI/2.f  ){
                distanceToLandmarks = sqrt(SQUARE(5000-mapEntry.position.x()) + SQUARE(mapEntry.position.y()));
              } else {
                distanceToLandmarks = sqrt(SQUARE(5000+mapEntry.position.x()) + SQUARE(mapEntry.position.y()));
              }

              llog(DEBUG1) << "Match point at x:" << mapEntry.position.x() << ", y:" << mapEntry.position.y() << ", turn:" 
                  << mapEntry.position.theta() << ", score: " << mapEntry.score << std::endl;

              // Make adjustments to the mean
              Eigen::Vector3f adjustment;
              float forwardAdjustment = (-t1/t2-1.f)*distanceToLandmarks; 
              adjustment << forwardAdjustment, 0, turn;
              mapEntry.position.vec += R*adjustment;
              mapEntry.position.theta() = NORMALISE(mapEntry.position.theta());

              // Logging block
              llog(DEBUG1) << "Distance to landmarks: " << distanceToLandmarks << std::endl;     
              llog(DEBUG1) << "Scale -1  is: " << -t1/t2-1.f << std::endl;
              llog(DEBUG1) << "Forward adjustment: " << forwardAdjustment << std::endl;
              llog(DEBUG1) << "Adjusted (x,y): (" << mapEntry.position.x() << ", " << mapEntry.position.y() << ")\n";  

              // Calculate the observation variance in RR coordinates
              Eigen::Matrix<float, 3, 3> rr_var;

              float forwards_stddev = 250/SQUARE(mapEntry.score) + 1.0*std::abs(forwardAdjustment); // 1.0m at matching score = 0.5
              float sideways_stddev = 750/SQUARE(mapEntry.score); // 3m at matching score = 0.5

              rr_var(0,0) = SQUARE(forwards_stddev); 
              rr_var(1,1) = SQUARE(sideways_stddev); 
              rr_var(2,2) = SQUARE(0.2f); // constant 0.2 radians turn std dev
              
              // could account for strong covariance between theta error and sideways translation error,
              // but in practise this turns out to be a bad idea since the relationship is not linear over large distances
              //rr_var(1,2) = rr_var(2,1) = sideways_stddev * atan2(sideways_stddev , distanceToLandmarks);

              // Add observation variance to the variance of the original map landmark position
              mapEntry.position.var += R*rr_var*R.transpose();
*/
              matches.push(mapEntry);
              }
          }

      }  	        
    }
    llog(DEBUG1) << "\nCalculating cosines over " << map.size() << " images, RANSAC geo validation & position adjustments took ";
    llog(DEBUG1) << t.elapsed_us() << " us" << std::endl;
}

