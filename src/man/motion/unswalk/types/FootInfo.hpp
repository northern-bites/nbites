#pragma once

#include "types/Point.hpp"
#include "types/RRCoord.hpp"

struct FootInfo
{
   FootInfo () {}
   FootInfo (RRCoord rr, Point imageCoords) :
      rr(rr),
      imageCoords(imageCoords) {}

   virtual ~FootInfo () {}

   RRCoord rr;
   Point imageCoords;

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & rr;
      ar & imageCoords;
   }
};
