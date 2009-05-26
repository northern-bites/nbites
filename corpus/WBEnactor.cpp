
#include "WBEnactor.h"

using boost::shared_ptr;
using namespace std;

const int WBEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float WBEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / WBEnactor::MOTION_FRAME_RATE;
const float WBEnactor::MOTION_FRAME_LENGTH_S = 1.0f / WBEnactor::MOTION_FRAME_RATE;


WBEnactor::WBEnactor(shared_ptr<Sensors> _sensors,
                     shared_ptr<Transcriber> transcriber)
    :MotionEnactor(){
}

WBEnactor::~WBEnactor(){}


void WBEnactor::postSensors(){
}

void WBEnactor::sendCommands(){

}



