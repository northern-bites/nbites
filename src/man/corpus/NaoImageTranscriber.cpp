#include "NaoImageTranscriber.h"
#include "Profiler.h"

namespace man {
namespace corpus {

NaoImageTranscriber::NaoImageTranscriber(boost::shared_ptr<Sensors> sensors,
                                         std::string name)
		: ThreadedImageTranscriber(sensors, name),
          topImageTranscriber(sensors, Camera::TOP),
          bottomImageTranscriber(sensors, Camera::BOTTOM)
{
}

void NaoImageTranscriber::run()
{
    Thread::running = true;
    Thread::trigger->on();

    long long lastProcessTimeAvg = VISION_FRAME_LENGTH_uS;

    struct timespec interval, remainder;
    while (Thread::running) {
        PROF_ENTER(P_MAIN);
        PROF_ENTER(P_GETIMAGE);
        //start timer
        const long long startTime = monotonic_micro_time();

        topImageTranscriber.waitForImage();
        bottomImageTranscriber.waitForImage();

        subscriber->notifyNextVisionImage();

        PROF_EXIT(P_GETIMAGE);

        //stop timer
        const long long processTime = monotonic_micro_time() - startTime;
        //sleep until next frame

        lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

        if (processTime > VISION_FRAME_LENGTH_uS) {
            if (processTime > VISION_FRAME_LENGTH_PRINT_THRESH_uS) {
#ifdef DEBUG_ALIMAGE_LOOP
                cout << "Time spent in ALImageTranscriber loop longer than"
                          << " frame length: " << processTime <<endl;
#endif
            }
            //Don't sleep at all
        } else{
            const long int microSleepTime =
                static_cast<long int>(VISION_FRAME_LENGTH_uS - processTime);
            const long int nanoSleepTime =
                static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

            const long int secSleepTime =
                static_cast<long int>(microSleepTime / (1000*1000));

            // cout << "Sleeping for nano: " << nanoSleepTime
            //      << " and sec:" << secSleepTime << endl;

            interval.tv_sec = static_cast<time_t>(secSleepTime);
            interval.tv_nsec = nanoSleepTime;

            nanosleep(&interval, &remainder);
        }
        PROF_EXIT(P_MAIN);
        PROF_NFRAME();
    }
    Thread::trigger->off();
}

}
}
