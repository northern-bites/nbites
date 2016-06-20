
#pragma once

#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <Eigen/Eigen>

#include "VisionConstants.hpp"
#include "utils/Cluster.hpp"
#include "types/Ipoint.hpp"



class Vocab {

public:

  //! Destructor
  ~Vocab() {};

  //! Constructor
  Vocab() {
    vec_length = 0;
  };

	int getSize(){
		return vec_length;
	}

  //! Learns a set of visual words from larger set of interest points
  void learn(std::vector<Ipoint> ipts, int num_words);

	//! Words that have non-zero index coefficients coefficients got on stop list
	void setStopWords(Eigen::VectorXf indices);

  //! Loads a set of visual words for use
  void loadVocabFile(std::string filename);

  //! Saves a set of visual words
  void saveVocabFile(std::string filename);

  //! Map a set of interest points to a sparse term frequency vector
  Eigen::VectorXf mapToVec(std::vector<Ipoint> &ipts);

	//! Map a set of interest points to a sparse term frequency vector while preserving 
  // the pixel locations of the interest points
  Eigen::VectorXf mapToVec(std::vector<Ipoint> &ipts, std::vector< std::vector<float> > &pixel_location);


private:
  
	int vec_length;

  std::vector<Ipoint> pos_words;
  std::vector<Ipoint> neg_words;
	
	std::vector<Ipoint> pos_stop_words;
	std::vector<Ipoint> neg_stop_words;
  

};

