

#include "Encoder.h"

using namespace std;

Encoder::Encoder(Blackboard *_memory) {

  memory = _memory;

}


//in the future, we will want to use pose information to not look too high
// but we dont have it yet - August 6, 2012
void Encoder::make_runs() {

  cout << "we are using the top camera (t/f): " << memory->isTopCamera() << endl;

  unsigned char lastPixel = GREEN_BIT;
  int currentRun = 0;
  
  for (int x = 0; x < IMAGE_WIDTH; x++) {
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
      
      unsigned char curPixel = memory->getBitwise(x, y);
      if (lastPixel == curPixel) {
	currentRun++;
      }

      if (lastPixel != curPixel || y == IMAGE_HEIGHT - 1) {
	memory->newRun(x, y, currentRun, lastPixel);
	currentRun = 1; //starting a new run;
      }
      
      lastPixel = curPixel;
    }
  }


}
