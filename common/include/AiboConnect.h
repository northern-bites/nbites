/* 

   AIBOCONNECT CONSTANTS.
   AiboConnect mostly lives in: AiboConnect.cc and EchoServer.cc, as well
   as some initialization stuff in Vision.cc

 */

#ifndef AiboConnect_h_DEFINED
#define AiboConnect_h_DEFINED

/* 
#
#   VISION CONSTANTS 
#
*/

// These are constants for connectString, which keeps track of the current
// type of request from the AiboConnect client
#define AC_NONE 0
#define AC_STRING 1
#define AC_RAW 2
#define AC_CORRECTED 3
#define AC_THRESH 4
#define AC_RLE 5
#define AC_LOG_IMAGE 6
#define AC_LOG_RFRAME 7
#define AC_LOG_VFRAME 8
#define AC_LOG_MFRAME 9

/* for the deflate() function, we compress each byte to BIT_SHIFT_SIZE number of bits to save space when we send over the image array */
#define BIT_SHIFT_SIZE 4 

/* With Image Sizes defined in VisionDef.h included above, these are the sizes
   for the two types of image arrays we send over the wireless, one for FULL, 
   UNSEGMENTED, UNPROCESSED images (minus chromatic distortion correction) and 
   one for the THRESHOLDED images.
 */
#define THRESH_IMAGE_BYTE_SIZE 160*208*BIT_SHIFT_SIZE/8

// Total number of passed values within the string command.
#define NUM_ECHO_VALUES 20

/* 
#
#  TCP INTERFACE 
#
*/

/* number of connections allowed per robot */
#define AC_CONNECTION_MAX  4

/* TCP ports, we will take up ports from 
   AIBOCONNECT_PORT to AIBOCONNECT_PORT+AIBOCONNECT_CONNECTION_MAX*/ 
#define AC_PORT 23000  

/* size of our TCP buffers for recv, send, and listen */
#define AC_LISTEN_BUFFER 200000
#define AC_RECV_BUFFER 100000
#define AC_SEND_BUFFER 100000

#endif // AiboConnect_h_DEFINED
