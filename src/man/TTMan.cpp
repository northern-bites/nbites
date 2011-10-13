#include "TTMan.h"

using boost::shared_ptr;
using namespace std;


TTMan::TTMan(shared_ptr<Profiler> _profiler,
             shared_ptr<Sensors> _sensors,
             boost::shared_ptr<RoboGuardian> guardian,
             shared_ptr<Transcriber> _transcriber,
             shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
             shared_ptr<ThreadedMotionEnactor> _enactor,
             shared_ptr<Lights> _lights,
             shared_ptr<Speech> _speech)
    :Man(_profiler,_sensors, guardian, _transcriber, _imageTranscriber,
         _enactor, _lights, _speech),
     threadedImageTranscriber(_imageTranscriber),
     threadedEnactor(_enactor){}

TTMan::~TTMan(){}


void TTMan::startSubThreads(){
    if(threadedEnactor->start()!=0)
        cout << "Failed to start enactor" <<endl;

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (threadedImageTranscriber->start() != 0)
        cerr << "Image transcriber failed to start" << endl;

}
void TTMan::stopSubThreads(){

#ifdef DEBUG_MAN_THREADING
    cout << "  TTMan stopping:" << endl;
#endif

    threadedImageTranscriber->stop();
    threadedImageTranscriber->waitForThreadToFinish();

    Man::stopSubThreads();

    threadedEnactor->stop();
    threadedEnactor->waitForThreadToFinish();
}

