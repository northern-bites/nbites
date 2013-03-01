#ifndef BLACKBOARD_H
#define BLACKBOARD_H

// This file is the header for the memory of the new vision system
// The blackboard will store all information that is shared across
// different parts of the vision system, including blobbing information,
// output data, and of course the image itself.
// @author: Benjamin Mende
// @date:   July 27, 2012




class Blackboard; //forward reference
#include "newVision.h"

struct run {
    int x;
    int y;
    int h;
};

class Blackboard {


 public:
  Blackboard();
  ~Blackboard();

  void setImages_old(const uint16_t *top, const uint16_t *bot);
  
  bool isEmpty() {return empty;}
  bool isTopCamera() {return usingTopCamera;}

  void setTopCamera(bool isTopCamera) {usingTopCamera = isTopCamera;}

  void init_runs();
  void newRun(int x, int y, int h, uint8_t color);

  const uint8_t* get_thresh_top() {return thresholded_top;}
  const uint8_t* get_thresh_bot() {return thresholded_bot;}

  inline uint8_t  getBitwise(int x, int y) {
    if (usingTopCamera)
      return thresholded_top[x + (y * IMAGE_HEIGHT)];
    else
      return thresholded_bot[x + (y * IMAGE_HEIGHT)];
  }
  
 private:
  
  // images, and their thresholded buddies.
  const uint16_t *yImg_top, *uImg_top, *vImg_top;
  const uint16_t *yImg_bot, *uImg_bot, *vImg_bot;
  uint8_t *thresholded_top, *thresholded_bot;
  bool empty, usingTopCamera;
  
  // run arrays and their auxillary data, for RLE only.
  run *red_runs, *navy_runs, *yellow_runs, *white_runs, *green_runs, *orange_runs;
  int numRuns_red, numRuns_navy, numRuns_yellow, numRuns_white, numRuns_green, numRuns_orange;
  int runsize, run_num;


};


#endif
