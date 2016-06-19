#pragma once

#include "types/RansacTypes.hpp"

/* 2010 code required two field edges to be written, one robot relative,
 * and one image relative for debugging. This is just a wrapper. Feel
 * free to change
 */
struct FieldEdgeInfo
{
   FieldEdgeInfo () {}
   FieldEdgeInfo (RANSACLine rrEdge, RANSACLine imageEdge) :
      rrEdge (rrEdge),
      imageEdge (imageEdge) {}

   virtual ~FieldEdgeInfo () {}

   RANSACLine rrEdge;
   RANSACLine imageEdge;

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & rrEdge;
      ar & imageEdge;
   }
};

inline std::ostream& operator<<(std::ostream& os, const FieldEdgeInfo& fieldEdge) {
   os << fieldEdge.rrEdge;
   os << fieldEdge.imageEdge;
   return os;
}

inline std::istream& operator>>(std::istream& is, FieldEdgeInfo& fieldEdge) {
   is >> fieldEdge.rrEdge;
   is >> fieldEdge.imageEdge;
   return is;
}
