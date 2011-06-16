#include "TTMan.h"

using boost::shared_ptr;
using namespace std;


TTMan::TTMan(shared_ptr<Profiler> _profiler,
             shared_ptr<Sensors> _sensors,
             shared_ptr<Transcriber> _transcriber,
             shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
             shared_ptr<ThreadedMotionEnactor> _enactor,
             shared_ptr<Synchro> synchro,
             shared_ptr<Lights> _lights,
             shared_ptr<Speech> _speech)
    :Man(_profiler,_sensors, _transcriber, _imageTranscriber,
         _enactor, synchro, _lights, _speech),
     threadedImageTranscriber(_imageTranscriber),
     threadedEnactor(_enactor){}

TTMan::~TTMan(){}


void TTMan::startSubThreads(){
    if(threadedEnactor->start()!=0)
        cout << "Failed to start enactor" <<endl;
    else
        threadedEnactor->getTrigger()->await_on();

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (threadedImageTranscriber->start() != 0) {
        cerr << "Image transcriber failed to start" << endl;
    }
    else
        threadedImageTranscriber->getTrigger()->await_on();
}
void TTMan::stopSubThreads(){
#ifdef DEBUG_MAN_THREADING
    cout << "  TTMan stoping:" << endl;
#endif

    threadedImageTranscriber->stop();
    threadedImageTranscriber->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Image Transcriber thread is stopped" << endl;
#endif

    Man::stopSubThreads();

    threadedEnactor->stop();
    threadedEnactor->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Enactor thread is stopped" << endl;
#endif
}

