#include "WBImageTranscriber.h"

using boost::shared_ptr;
using namespace std;

WBImageTranscriber::WBImageTranscriber(shared_ptr<Sensors> s,
                                       shared_ptr<Synchro> synchro)
    :ThreadedImageTranscriber(s,synchro,"WBImageTranscriber")
{
}


WBImageTranscriber::~WBImageTranscriber(){}


void WBImageTranscriber::releaseImage(){}

void WBImageTranscriber::waitForImage(){}

void WBImageTranscriber::run(){
    Thread::running = true;
    Thread::trigger->on();

    while (Thread::running) {
        //start timer
        const long long startTime = micro_time();
        waitForImage();

        subscriber->notifyNextVisionImage();

        //stop timer
        const long long processTime = micro_time() - startTime;
        //sleep until next frame
        if (processTime > VISION_FRAME_LENGTH_uS){
            cout << "Time spent in ALImageTranscriber loop longer than"
                 << " frame length: " << processTime <<endl;
            //Don't sleep at all
        } else{
            //cout << "Sleeping for " << VISION_FRAME_LENGTH_uS
            //    -processTime << endl;

            //usleep(10000000);
            usleep(static_cast<useconds_t>(VISION_FRAME_LENGTH_uS
                                           -processTime));
        }
    }

    Thread::trigger->off();
}
