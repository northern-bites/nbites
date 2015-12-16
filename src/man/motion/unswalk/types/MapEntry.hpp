
#pragma once

#include <vector>
#include "Ipoint.hpp"
#include "AbsCoord.hpp"
#include "RRCoord.hpp"
#include "RansacTypes.hpp"


struct MapEntry {

  MapEntry(){
    score = 0.f;
  };

  MapEntry(AbsCoord _position){
    position = _position;
    score = 0.f;
  }

  MapEntry(RRCoord _object){
    object = _object;
    score = 0.f;
  }

  MapEntry(std::vector<Ipoint> _ipoints, AbsCoord _position){
    ipoints = _ipoints;
    position = _position;
    score = 0.f;
  }

  MapEntry(std::vector<Ipoint> _ipoints, RRCoord _object){
    ipoints = _ipoints;
    object = _object;
    score = 0.f;
  }

	//! Enables MapEntrys to be sorted by score
  bool operator<(const MapEntry &rhs) const
  {
		if (this->score < rhs.score) return true;
		else return false;
  };

  
  // The position of the MapEntry
  AbsCoord position;

  // A RR coordinate if a MapEntry represents a view of another robot
  RRCoord object;

  // Landmarks visible from that position
  std::vector<Ipoint> ipoints;

  // Matching score against a query using cosine tfidf or other method
  float score;

  // Pixel ransac line between the two images
  RANSACLine ransacLine;


  template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version)
   {
      ar & position;
      ar & ipoints;      
      ar & score;
      ar & object;
      ar & ransacLine;
   }

};
