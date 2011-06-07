#include "TMan.h"

using namespace std;

TMan::TMan(boost::shared_ptr<Profiler> _profiler,
        boost::shared_ptr<Sensors> _sensors,
             boost::shared_ptr<Transcriber> _transcriber,
             boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
             boost::shared_ptr<MotionEnactor> _enactor,
           boost::shared_ptr<Synchro> synchro,
           boost::shared_ptr<Lights> _lights)
    :Man(_profiler, _sensors,_transcriber,_imageTranscriber,_enactor,synchro,_lights),
     threadedImageTranscriber(_imageTranscriber)
{

}

TMan::~TMan(){

}



void TMan::startSubThreads(){

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (threadedImageTranscriber->start() != 0) {
        cerr << "Image transcriber failed to start" << endl;
    }
    else
        threadedImageTranscriber->getTrigger()->await_on();

}

void TMan::stopSubThreads(){
#ifdef DEBUG_MAN_THREADING
    cout << "  TMan stoping:" << endl;
#endif

    threadedImageTranscriber->stop();
    threadedImageTranscriber->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Image Transcriber thread is stopped" << endl;
#endif

    Man::stopSubThreads();
}
