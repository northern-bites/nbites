#include "NaoImageTranscriber.h"
#include "Profiler.h"

#include <vector>

namespace man {
namespace corpus {

using namespace memory;

NaoImageTranscriber::NaoImageTranscriber(boost::shared_ptr<Sensors> sensors,
                                         std::string name, memory::MRawImages::ptr rawImages)
		: ThreadedImageTranscriber(sensors, name),
          topImageTranscriber(sensors, Camera::TOP, rawImages),
          bottomImageTranscriber(sensors, Camera::BOTTOM, rawImages),
          memoryProvider(&NaoImageTranscriber::updateMRawImages, this, rawImages),
          rawImages(rawImages)
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

        //usually the locking and releasing of mobjects is automatic, but in this
        //case we actually update them directly in waitForImage (to avoid copying of the
        //image, slow)
        rawImages->lock();
        topImageTranscriber.waitForImage();
        bottomImageTranscriber.waitForImage();
        rawImages->release();

        sensors->updateVisionAngles();
        memoryProvider.updateMemory();

        PROF_EXIT(P_GETIMAGE);

        subscriber->notifyNextVisionImage();

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

void NaoImageTranscriber::updateMRawImages(memory::MRawImages::ptr rawImages) const {
    //we don't need to copy image information - we do that in each of the transcribers

    //TODO: this is a hack so we get the vision body angles for pose in image processing
    rawImages->get()->clear_vision_body_angles();
    std::vector<float> bodyAngles = this->sensors->getVisionBodyAngles();
    for (std::vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        rawImages->get()->add_vision_body_angles(*i);
    }
}

}
}
