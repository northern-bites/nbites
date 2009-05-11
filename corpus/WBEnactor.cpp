
#include "WBEnactor.h"

using boost::shared_ptr;
using namespace std;

const int WBEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float WBEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / WBEnactor::MOTION_FRAME_RATE;
const float WBEnactor::MOTION_FRAME_LENGTH_S = 1.0f / WBEnactor::MOTION_FRAME_RATE;


WBEnactor::WBEnactor(shared_ptr<Sensors> _sensors,
                     shared_ptr<Synchro> synchro,
                     shared_ptr<Transcriber> transcriber)
    :ThreadedMotionEnactor(synchro, "WBEnactor"){
}

WBEnactor::~WBEnactor(){}


void WBEnactor::postSensors(){
}

void WBEnactor::sendCommands(){

}

void WBEnactor::run(){
    Thread::running = true;
    Thread::trigger->on();

    long long currentTime;
    while (running) {
        currentTime = micro_time();
        sendCommands();
        //Once we've sent the most calculated joints
        postSensors();

        const long long zero = 0;
        const long long processTime = micro_time() - currentTime;

#if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        if (processTime > MOTION_FRAME_LENGTH_uS){
            cout << "Time spent in "<<Thread::name
                 << " longer than frame length: "
                 << processTime <<endl;
            //Don't sleep at all
        } else{
            usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS
                                           -processTime));
        }
#endif

    }
    Thread::running = true;
    Thread::trigger->off();

}


