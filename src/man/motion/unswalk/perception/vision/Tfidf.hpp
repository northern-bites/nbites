
#pragma once

#include <vector>
#include <queue>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <Eigen/Eigen>

#include "types/MapEntry.hpp"
#include "Vocab.hpp"
#include "VisionConstants.hpp"
#include "VisionDefs.hpp"
#include "Ransac.hpp"


// Term frequency - Inverse document frequency calculator (for fast searching over a lot of images)
class Tfidf {

public:

  //! Constructor
  Tfidf() {
    clearData();  
  };

  //! Destructor
  ~Tfidf() {};

  //! Loads vocab and map ready for use
  void loadVocab(std::string vocabFile); 
  void loadMap(std::string mapFile);

  //! Clears the map but not the vocab
  void clearMap();

  //! Saves the map, including any new entries
  void saveMap(std::string mapFile);

  //! Adds to the searchable collection, returns true if successful (won't work without a vocab loaded)
  bool addDocumentToCorpus(MapEntry document);

  //! Faster version if landmarks have already been mapped to words (with the same vocab file)
  bool addDocumentToCorpus(MapEntry document, Eigen::VectorXf tf_doc, std::vector< std::vector<float> > pixLoc);

  //! find out how many entries in map
  int getSize();

  //! Gets the n best matches based on the cosine of tf-idf
  void searchDocument(std::vector<Ipoint> query, std::priority_queue<MapEntry> &matches, unsigned int *seed, int n);

  //! Faster version if landmarks have already been mapped to words (with the same vocab file)
  void searchDocument(Eigen::VectorXf tf_query, 
                      std::vector< std::vector<float> > query_pixLoc, // pixel locations of the words 
                      std::priority_queue<MapEntry> &matches, 
                      unsigned int *seed, 
                      int n);

  // return the term count by word
  Eigen::VectorXf getni(){
    return ni;
  };


private:

   void clearData(){
      T = 0;
		N = 0;
		tf.clear();
		nd.clear();
      map.clear();
      pixels.clear();
   };
  
   Vocab                                        vocab;

	int														N; 					// number of documents
	int														T; 					// number of terms
   std::vector<Eigen::VectorXf> 	               tf;   			   // term frequency for each document
   std::vector< std::vector< std::vector<float> > > pixels;       // the pixel locations associated with each word
	std::vector<int> 							         nd; 				   // term count by document
	Eigen::VectorXf								      ni;					// term count by word;

	std::vector<MapEntry> 				            map;

	Eigen::VectorXf 							         idf;				// corpus inverse document frequency
	
};


