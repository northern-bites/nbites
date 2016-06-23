#pragma once

#include <stdint.h>
#include "utils/angles.hpp"

#include "types/PostInfo.hpp"

#define TOP_IMAGE_ROWS 960 //480
#define TOP_IMAGE_COLS 1280 //640
#define BOT_IMAGE_ROWS 480
#define BOT_IMAGE_COLS 640
#define IMAGE_ROWS 960 //480
#define IMAGE_COLS 1280 //640

#define IMAGE_HFOV UNSWDEG2RAD(60.9) // horizontal field of view
#define IMAGE_VFOV UNSWDEG2RAD(47.64)

#define IMAGE_DEPTH 2
#define IMAGE_ROW_STEP (IMAGE_COLS * IMAGE_DEPTH)

#define MAXY 128
#define MAXY_POW 7
#define MAXU 128
#define MAXU_POW 7
#define MAXV 128
#define MAXV_POW 7

#define MAX_FEET 2
#define MAX_POSTS 2
#define MAX_BALLS 2
#define MAX_FIELD_EDGES 2
//#define MAX_FIELD_LINE_POINTS 40
#define MAX_ROBOTS 9
#define MAX_LANDMARKS 100

/* Number of field lines could be potentially abitary,
 * so choose a large number
 *
 * Also includes corners, T-juncts, penalty spots, and centre circle
 */
#define MAX_FIELD_FEATURES 100
#define MAX_FIELD_LINE_POINTS 250

/* Scan every SALIENCY_DENSITYth pixel */
// #define SALIENCY_DENSITY 4
// #define SALIENCY_DENSITY_POW 2
#define TOP_SALIENCY_DENSITY 8
#define BOT_SALIENCY_DENSITY 8

#define TOP_SALIENCY_ROWS (TOP_IMAGE_ROWS / TOP_SALIENCY_DENSITY)
#define TOP_SALIENCY_COLS (TOP_IMAGE_COLS / TOP_SALIENCY_DENSITY)
#define BOT_SALIENCY_ROWS (BOT_IMAGE_ROWS / BOT_SALIENCY_DENSITY)
#define BOT_SALIENCY_COLS (BOT_IMAGE_COLS / BOT_SALIENCY_DENSITY)

#define MIN_ANGLE_BETWEEN_EDGE_LINES UNSWDEG2RAD(30)
//#define MIN_ANGLE_BETWEEN_FIELD_LINES UNSWDEG2RAD(50)
#define MAX_EDGE_DEVIATION_FROM_PERPENDICULAR UNSWDEG2RAD(15)
#define MIN_ANGLE_BETWEEN_CORNERS UNSWDEG2RAD(10)

/* Values of the various colours that are present on the field.
 * Anything not categorized as a 'colour' is named unclassified */
typedef enum {
   cBALL = 0,
   cGOAL_BLUE = 1,
   cGOAL_YELLOW = 2,
   cROBOT_BLUE = 1,
   cROBOT_RED = 4,
   cFIELD_GREEN = 5,
   cWHITE = 6,
   cBLACK = 7,
   cBACKGROUND = 8,
   cUNCLASSIFIED = 9,
   cBODY_PART = 10,
   cNUM_COLOURS = 11
}


#ifndef SWIG
__attribute__((packed))
#endif
Colour;

enum feature_type
{
   fNONE = -1,
   fBALL = 0,
   fGOAL_POST_BLUE,
   fGOAL_BAR_BLUE,
   fGOAL_POST_YELLOW,
   fGOAL_BAR_YELLOW,
   fROBOT_BLUE,
   fROBOT_RED,
   fFIELD_LINE,
   fFIELD_LINE_THIN,
   fFIELD_CORNER,
   fFIELD_T_JUNCTION,
   fFIELD_PENALTY_SPOT,
   fFIELD_CENTRE_CIRCLE,
   fFIELD_EDGE,
   fNUM_FEATURES
};

/**
 * LEGACY CODE - DO NOT USE
 * Which posts are seen by vision
 * They have been defined such that individual features
 * can be extracted using `and' masks
 */
enum WhichPosts
{
   p_legacy_NONE = 0,
   p_legacy_BLUE_LEFT = 1,
   p_legacy_BLUE_RIGHT = 2,
   p_legacy_BLUE_BOTH = 3,
   p_legacy_BLUE_EITHER = 4,
   p_legacy_YELLOW_LEFT = 5,
   p_legacy_YELLOW_RIGHT = 6,
   p_legacy_YELLOW_BOTH = 7,
   p_legacy_YELLOW_EITHER = 8
}
#ifndef SWIG
__attribute__((packed))
#endif
;

/**
 * DO NOT USE - THESE FUNCTION WILL BE DEPRECATED
 */
inline std::pair<PostInfo::Type, PostInfo::Type> WhichPosts_to_PostType (WhichPosts posts)
{
   PostInfo::Type left, right; 

   switch (posts)
   {
      case p_legacy_NONE:
         left  = PostInfo::pNone;
         right = PostInfo::pNone;
         break;
      case p_legacy_BLUE_LEFT:
         left  = PostInfo::pHomeLeft;
         right = PostInfo::pNone;
         break;
      case p_legacy_BLUE_RIGHT:
         left  = PostInfo::pNone;
         right = PostInfo::pHomeRight;
         break;
      case p_legacy_BLUE_BOTH:
         left  = PostInfo::pHomeLeft;
         right = PostInfo::pHomeRight;
         break;
      case p_legacy_BLUE_EITHER:
         left  = PostInfo::pHome;
         right = PostInfo::pNone;
         break;
      case p_legacy_YELLOW_LEFT:
         left  = PostInfo::pAwayLeft;
         right = PostInfo::pNone;
         break;
      case p_legacy_YELLOW_RIGHT:
         left  = PostInfo::pNone;
         right = PostInfo::pAwayRight;
         break;
      case p_legacy_YELLOW_BOTH:
         left  = PostInfo::pAwayLeft;
         right = PostInfo::pAwayRight;
         break;
      case p_legacy_YELLOW_EITHER:
         left  = PostInfo::pAway;
         right = PostInfo::pNone;
         break;
   }

   return std::pair<PostInfo::Type, PostInfo::Type> (left, right);
}

inline WhichPosts PostType_to_WhichPosts (std::pair<PostInfo::Type, PostInfo::Type> posts)
{
   PostInfo::Type a = posts.first;
   PostInfo::Type b = posts.second;

   if (a == PostInfo::pNone && b != PostInfo::pNone) {
      a = b;
      b = PostInfo::pNone;
   }

   if (a == PostInfo::pNone) {
      return p_legacy_NONE;
   }

   if (a != PostInfo::pNone) {
      if (a & PostInfo::pHome) {
         return p_legacy_BLUE_BOTH;
      } else {
         return p_legacy_YELLOW_BOTH;
      }
   }

   if (a & PostInfo::pHome) {
      if (a & PostInfo::pLeft) {
         return p_legacy_BLUE_LEFT;
      } else if (a & PostInfo::pRight) {
         return p_legacy_BLUE_RIGHT;
      } else {
         return p_legacy_BLUE_EITHER;
      }
   } else {
      if (a & PostInfo::pLeft) {
         return p_legacy_YELLOW_LEFT;
      } else if (a & PostInfo::pRight) {
         return p_legacy_YELLOW_RIGHT;
      } else {
         return p_legacy_YELLOW_EITHER;
      }
   }
}

/* The number that has to be added to one of the blue post options to
   get the equivalent yellow post option. MAKE sure this is updated
   if the WhichPosts enum changes, otherwise the goal detection will
   break */
#define NUMBER_ADD_TO_BLUE 4

/**
 * for simplicity defined in the same direction as
 *    x,y(t) = cos(t), sin(t)
 * the order of this enum is important
 */
enum dir_t
{
   d_ee,
   d_nee,
   d_ne,
   d_nne,
   d_nn,
   d_nnw,
   d_nw,
   d_nww,
   d_ww,
   d_sww,
   d_sw,
   d_ssw,
   d_ss,
   d_sse,
   d_se,
   d_see,
   d_num,
   /* Beyond here, values are not used in numerical calculations */
   d_none
}
#ifndef SWIG
__attribute__((packed))
#endif
;

enum orientation_t
{
   horizontal,
   vertical,
   num_orientations
};

enum low_feature_t
{
   f_none,
   f_line_edge,
   f_line_centre,
   f_corner_convex,
   f_corner_concave,
   f_dogs_breakfast
}

#ifndef SWIG
__attribute__((packed))
#endif
;

struct low_feature
{
   low_feature_t type;
   dir_t         orientation;

   low_feature (low_feature_t type, dir_t orientation)
      : type (type), orientation (orientation) {}
   low_feature ()
      : type (f_none), orientation (d_none) {}
};

/* Components of a pixel */
typedef enum {
   ycomponent,
   ucomponent,
   vcomponent,
   NUM_COMPONENTS
}

#ifndef SWIG
__attribute__((packed))
#endif
Component;

typedef struct pixelValues PixelValues;
struct pixelValues {
   uint8_t y;
   uint8_t u;
   uint8_t v;
};

typedef struct twoParamLine TwoParamLine;
struct twoParamLine {
   float m;
   float b;
};

/* Name of the various colours that are present on the field.
 * Anything not categorized as a 'colour' is named unclassified */

extern const char* ColourNames[cNUM_COLOURS];

