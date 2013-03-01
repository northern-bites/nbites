#ifndef ENCODER_H
#define ENCODER_H

class Encoder; //forward reference
#include "../newVision.h"

class Encoder {

 public:
  Encoder(Blackboard *_memory);
  ~Encoder();


  void make_runs(); // this should be the only point of entry to RLE




 private:
  Blackboard *memory;


};
#endif
