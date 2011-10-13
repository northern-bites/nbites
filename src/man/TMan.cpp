#include "TMan.h"

using namespace std;

TMan::TMan(boost::shared_ptr<Profiler> _profiler,
           boost::shared_ptr<Sensors> _sensors,
           boost::shared_ptr<Transcriber> _transcriber,
           boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
           boost::shared_ptr<MotionEnactor> _enactor,
           boost::shared_ptr<Synchro> synchro,
           boost::shared_ptr<Lights> _lights,
           boost::shared_ptr<Speech> _speech)
    :Man(_profiler,_sensors, _transcriber, _imageTranscriber,
         _enactor, synchro, _lights, _speech),
     threadedImageTranscriber(_imageTranscriber)
{

}

TMan::~TMan(){

}

void TMan::startSubThreads(){

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (threadedImageTranscriber->start() != 0)
        cerr << "Image transcriber failed to start" << endl;

}

void TMan::stopSubThreads(){

#ifdef DEBUG_MAN_THREADING
    cout << "  TMan stopping:" << endl;
#endif

    threadedImageTranscriber->stop();
    threadedImageTranscriber->waitForThreadToFinish();
    Man::stopSubThreads();
}
