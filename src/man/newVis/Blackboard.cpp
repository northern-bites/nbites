/*************************************************

This file serves as the the memory of the new vision
system. This memory blackboard will store all information
necessary for vision, and serves as the point of
interaction between all subsystems. Methods in this class
should only exist to write and read. NO PROCESSING
should happen here. 

@author: Benjamin Mende
@date: July 27, 2012


 ************************************************/


#include "Blackboard.h"


Blackboard::Blackboard()
{

  empty = true;
  init_runs();

}


void Blackboard::setImages_old(const uint16_t *top, const uint16_t *bot) {

  empty = false;

  yImg_bot = bot;
  uImg_bot = bot + AVERAGED_IMAGE_SIZE;
  vImg_bot = uImg_bot + AVERAGED_IMAGE_SIZE;

  yImg_top = top;
  uImg_top = top + AVERAGED_IMAGE_SIZE;
  vImg_top = uImg_top + AVERAGED_IMAGE_SIZE;


  thresholded_top = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(top) +
					 Y_IMAGE_BYTE_SIZE + U_IMAGE_BYTE_SIZE +
					 V_IMAGE_BYTE_SIZE);
  thresholded_bot = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(bot) +
					 Y_IMAGE_BYTE_SIZE + U_IMAGE_BYTE_SIZE +
					 V_IMAGE_BYTE_SIZE);
}

// the following section deals with runlength encoding storage
// RLE is still VERTICAL!!!
void Blackboard::init_runs() {

  const int RUNS_PER_SCANLINE = 15;
  const int RUNS_PER_LINE = 5;
  const int RUN_VALUES = 3;

  run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
  runsize = IMAGE_WIDTH * RUNS_PER_LINE;

  red_runs = (run*)malloc(sizeof(run) * run_num);
  navy_runs = (run*)malloc(sizeof(run) * run_num);
  yellow_runs = (run*)malloc(sizeof(run) * run_num);
  white_runs = (run*)malloc(sizeof(run) * run_num);
  green_runs = (run*)malloc(sizeof(run) * run_num);
  orange_runs = (run*)malloc(sizeof(run) * run_num);

}


// this is a complicated write
void Blackboard::newRun(int x, int y, int h, uint8_t color) {
  
  int NOISE_SKIPS = 0; //this will be different for each color;

  switch(color)
    {
    case RED_BIT:                     //this will help with red robots
      NOISE_SKIPS = 8;
      if (numRuns_red < runsize) {
	int last = numRuns_red - 1;
	//skip noise
	if (last > 0 && red_runs[last].x == x &&
	    (red_runs[last].y - (y + h) <= NOISE_SKIPS))
	  {
	    red_runs[last].h += red_runs[last].y - y; 
	    red_runs[last].y = y;
	    h = red_runs[last].h;
	    numRuns_red--; //dont count this merge as a new run
	  }
	else 
	  {
	    red_runs[numRuns_red].x = x;
	    red_runs[numRuns_red].y = y;
	    red_runs[numRuns_red].h = h;
	  }
	numRuns_red++;
      }
      break;
    case NAVY_BIT:                     //this will help with blue robots
      NOISE_SKIPS = 8;
      if (numRuns_navy < runsize) {
	int last = numRuns_navy - 1;
	//skip noise
	if (last > 0 && navy_runs[last].x == x &&
	    (navy_runs[last].y - (y + h) <= NOISE_SKIPS))
	  {
	    navy_runs[last].h += navy_runs[last].y - y; 
	    navy_runs[last].y = y;
	    h = navy_runs[last].h;
	    numRuns_navy--; //dont count this merge as a new run
	  }
	else 
	  {
	    navy_runs[numRuns_navy].x = x;
	    navy_runs[numRuns_navy].y = y;
	    navy_runs[numRuns_navy].h = h;
	  }
	numRuns_navy++;
      }
      break;
    }

}
