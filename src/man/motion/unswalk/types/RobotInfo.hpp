#pragma once

#include "types/Point.hpp"
#include "types/RRCoord.hpp"
#include "types/BBox.hpp"

struct RobotInfo
{

   enum Type
   {
      rUnknown       = 0x00,
      rBlue          = 0x01,
      rRed           = 0x02,
   };

   enum Cameras
   {
       TOP_CAMERA,
       BOT_CAMERA,
       BOTH_CAMERAS,
       OLD_DETECTION
   };

   /* Names of corresponding enums from above */
   static const char *const TypeName[];

   RobotInfo () :
   rr(),
   type(rUnknown),
   cameras(BOTH_CAMERAS),
   topImageCoords(),
   botImageCoords()
   {}

   RobotInfo (RRCoord rr, Type type, BBox imageCoords) :
         rr(rr),
         type(type),
         cameras(OLD_DETECTION),
         imageCoords(imageCoords),
         topImageCoords(),
         botImageCoords() {}

   RobotInfo (RRCoord rr, Type type, BBox topImageCoords, BBox botImageCoords) :
       rr(rr),
       type(type),
       cameras(BOTH_CAMERAS),
       topImageCoords(topImageCoords),
       botImageCoords(botImageCoords){}

   RobotInfo (RRCoord rr, Type type, BBox imageCoords, Cameras cameras) :
      rr(rr),
      type(type),
      cameras(cameras),
      topImageCoords(),
      botImageCoords() {
      if (cameras == TOP_CAMERA) {
          topImageCoords = imageCoords;
      } else if (cameras == BOT_CAMERA) {
          botImageCoords = imageCoords;
      } else if (cameras == BOTH_CAMERAS) {
          //should use function above for BOTH
      }

   }

   virtual ~RobotInfo () {}

   RRCoord rr;
   Type type;
   Cameras cameras;

   BBox imageCoords; //BACKWARD COMPAT ONLY

   BBox topImageCoords;
   BBox botImageCoords;

   
   bool operator== (const RobotInfo &other) const {
      return rr == other.rr;
   }

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version) {
      ar & rr;
      ar & type;
      ar & cameras;
      ar & imageCoords;
      ar & topImageCoords;
      ar & botImageCoords;
   }
};
