/* 
   Constants for saving .FRMs used in Cortex 1.0
 */

#ifndef CortexDef_h_DEFINED
#define CortexDef_h_DEFINED

#include "VisionDef.h"

// joints + accelorometers + psds + body_tilt
#if ROBOT(AIBO)
static const int NUM_RAW_DATA = 22;
static const int RAW_DATA_SIZE = (2*NUM_RAW_DATA)*sizeof(double); 
static const int RAW_HEADER_SIZE = 100;
static const int RAW_IMAGE_SIZE = IMAGE_BYTE_SIZE;
#else
static const int NUM_RAW_DATA = 22;
static const int RAW_DATA_SIZE = 150*1024;
static const int RAW_HEADER_SIZE = 0;
static const int RAW_IMAGE_SIZE = IMAGE_BYTE_SIZE;
#endif

static const int SAVED_FRAME_SIZE = RAW_IMAGE_SIZE+RAW_HEADER_SIZE+RAW_DATA_SIZE;
static const int SAVED_FRAME_BUFFER_FRAMES = 60;
static const int SAVED_FRAME_BUFFER_SIZE = SAVED_FRAME_SIZE*
  SAVED_FRAME_BUFFER_FRAMES; // roughly 10mb

#if ROBOT(AIBO)
#  define FRM_FILE_EXT ".FRM"
#elif ROBOT(NAO_RL)
#  define FRM_FILE_EXT ".NFRM"
#elif ROBOT(NAO_SIM)
#  define FRM_FILE_EXT ".NSFRM"
#else
#  error Undefined robot type
#endif


#endif // CortexDef_h_DEFINED

