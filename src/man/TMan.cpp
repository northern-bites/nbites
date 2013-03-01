#include "TMan.h"

using namespace std;
using namespace man::memory;

TMan::TMan(RobotMemory::ptr memory,
           boost::shared_ptr<Sensors> _sensors,
           boost::shared_ptr<Transcriber> _transcriber,
           boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
           boost::shared_ptr<MotionEnactor> _enactor,
           boost::shared_ptr<Lights> _lights,
           boost::shared_ptr<Speech> _speech)
    :Man(memory, _sensors, _transcriber, _imageTranscriber,
         _enactor, _lights, _speech),
     threadedImageTranscriber(_imageTranscriber)
{

}

TMan::~TMan(){

}

void TMan::startSubThreads(){

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (threadedImageTranscriber->start() != 0)
        cout << "Image transcriber failed to start" << endl;

}

void TMan::stopSubThreads(){
    cout << "  TMan stopping!" << endl;

    threadedImageTranscriber->stop();
    threadedImageTranscriber->waitForThreadToFinish();
    Man::stopSubThreads();
}
