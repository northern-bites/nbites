#pragma once

#include "types/AbsCoord.hpp"

struct TeamBallInfo
{
   enum Status
   {
      tNeutral,
      tAgree,
      tDisagree
   };

   /* Names of corresponding enums from above */
   static const char *const StatusName[];

   TeamBallInfo () : pos(NAN, NAN, NAN), status(tNeutral), contributors(0) {}
   TeamBallInfo (AbsCoord pos, Status status, unsigned int contributors) :
      pos(pos),
      status(status),
      contributors(contributors) {}

   virtual ~TeamBallInfo () {}

   AbsCoord pos;
   Status status;
   unsigned int contributors;

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & pos;
      ar & status;
      ar & contributors;
   }
};
