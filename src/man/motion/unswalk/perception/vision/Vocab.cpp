
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>

#include "Vocab.hpp"
#include "utils/Logger.hpp"


using namespace Clustering;


// Learn visual words
void Vocab::learn(std::vector<Ipoint> ipts, int num_words){

   vec_length = num_words;
	pos_words.clear();
   neg_words.clear();
	pos_stop_words.clear();
   neg_stop_words.clear();

   Points pos, neg;
   for (unsigned int j=0; j<ipts.size(); j++){
      PointND point;
      for (unsigned int k=0; k<SURF_DESCRIPTOR_LENGTH; k++) point.push_back(ipts[j].descriptor[k]);
      if(ipts[j].laplacian == 1) {
      pos.push_back(point);
    } else {
      neg.push_back(point);
    }
  }
  PointsSpace pos_ps(pos);
  Clusters pos_clusters(vec_length/2, pos_ps);
  pos_clusters.k_means();

  PointsSpace neg_ps(neg);
  Clusters neg_clusters(vec_length/2, neg_ps);
  neg_clusters.k_means();

  // Save the clusters to pos words
  
  Centroids pos_cen = pos_clusters.getCentroids();
  for(int i=0; i<(int)pos_cen.size(); i++){
    Ipoint ipt;    
	 ipt.laplacian = 1;
    PointND point = pos_cen[i];
    for(int j=0; j<SURF_DESCRIPTOR_LENGTH; j++){
      ipt.descriptor[j] = point[j]; 
    }
    pos_words.push_back(ipt);
  }

  // Save the clusters to neg words
  Centroids neg_cen = neg_clusters.getCentroids();
  for(int i=0; i<(int)neg_cen.size(); i++){
    Ipoint ipt;   
	 ipt.laplacian = 0; 
    PointND point = neg_cen[i];
    for(int j=0; j<SURF_DESCRIPTOR_LENGTH; j++){
      ipt.descriptor[j] = point[j]; 
    }
    neg_words.push_back(ipt);
  }

}

//! Words that have non-zero index coefficients coefficients got on stop list
void Vocab::setStopWords(Eigen::VectorXf indices){

	int index = 0;
	for(int i=0; i<(int)pos_words.size(); i++){
		if (indices[index++]!=0){
			pos_stop_words.push_back(pos_words.at(i));			
			pos_words.erase(pos_words.begin() + i--);
		}
	}

	for(int i=0; i<(int)neg_words.size(); i++){
		if (indices[index++]!=0){
			neg_stop_words.push_back(neg_words.at(i));
			neg_words.erase(neg_words.begin() + i--);
		}
	}

  vec_length = pos_words.size() + neg_words.size();

}


void Vocab::loadVocabFile(std::string filename){
  
   pos_words.clear();
   neg_words.clear();
	pos_stop_words.clear();
	neg_stop_words.clear();

   std::ifstream ifs(filename.c_str());
   if (ifs.is_open()){  
      boost::archive::text_iarchive ia(ifs);
      ia >> pos_words;
	   ia >> neg_words;
	   ia >> pos_stop_words;
	   ia >> neg_stop_words;
   } else {
      throw std::runtime_error("error opening vocab file");
   }

   vec_length = pos_words.size() + neg_words.size();
  
}

void Vocab::saveVocabFile(std::string filename){

   std::ofstream ofs(filename.c_str());
   {
   boost::archive::text_oarchive oa(ofs);
   oa << pos_words << neg_words << pos_stop_words << neg_stop_words;
   }

}

//! Map a set of interest points to a sparse term frequency vector while preserving 
// the pixel locations of the interest points
Eigen::VectorXf Vocab::mapToVec(std::vector<Ipoint> &ipts, std::vector< std::vector<float> > &pixel_location){

   pixel_location.clear();
   pixel_location.resize(vec_length); // will call default constructor of std::vector<float>
   Eigen::VectorXf vec;
   vec = Eigen::VectorXf::Zero(vec_length);
	
   for(unsigned int i=0; i<ipts.size(); i++){
	   bool stop = false;
      Ipoint ipt = ipts[i];
      float best = std::numeric_limits<float>::max();
      float dist;
      int word = 0;

		// Search either pos or neg laplacians, not both
      if(ipt.laplacian == 1){
         for(int j=0; j<(int)pos_words.size(); j++){
            dist = pos_words[j] - ipt;
            if(dist < best){
               best = dist;
               word = j;
            }
         }   
			// now search stop list
			for(int j=0; j<(int)pos_stop_words.size(); j++){
            dist = pos_stop_words[j] - ipt;
            if(dist < best){
               stop = true;
					break;
            }
         }            
      } else {
         for(int j=0; j<(int)neg_words.size(); j++){
            dist = neg_words[j] - ipt;
            if(dist < best){
               best = dist;
               word = j+pos_words.size();
            } 
         }    
			// now search stop list
			for(int j=0; j<(int)neg_stop_words.size(); j++){
            dist = neg_stop_words[j] - ipt;
            if(dist < best){
               stop = true;
					break;
            } 
         }           
      }
		if (!stop) {
      vec[word]=vec[word]+1.f;
      pixel_location[word].push_back(ipt.x); 
    }  
  }
  return vec;
}


// maps a vector of interest points to a sparse term frequency vector
Eigen::VectorXf Vocab::mapToVec(std::vector<Ipoint> &ipts){

  std::vector< std::vector<float> > discard;
  return mapToVec(ipts, discard);
  
}


