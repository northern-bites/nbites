#include "ImageTranscriberModule.h"
//#include "Profiler.h"

#include <iostream>

using namespace portals;

namespace man {
namespace image {

ImageTranscriberModule::ImageTranscriberModule()
    : Module(),
      topImageOut(base()),
      bottomImageOut(base()),
      topImageTranscriber(Camera::TOP, &topImageOut),
      bottomImageTranscriber(Camera::BOTTOM, &bottomImageOut)
{
    topImageTranscriber.initTable("/home/nao/nbites/lib/table/top_table.mtb");
    bottomImageTranscriber.initTable("/home/nao/nbites/lib/table/bottom_table.mtb");
}

void ImageTranscriberModule::run_()
{
    //long long lastProcessTimeAvg = VISION_FRAME_LENGTH_uS;

    //struct timespec interval, remainder;
    //PROF_ENTER(P_MAIN);
    //PROF_ENTER(P_GETIMAGE);
    //start timer
    //const long long startTime = monotonic_micro_time();

    topImageTranscriber.waitForImage();
    bottomImageTranscriber.waitForImage();

    //PROF_EXIT(P_GETIMAGE);

    //stop timer
    //const long long processTime = monotonic_micro_time() - startTime;
    //sleep until next frame

    //lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

    // if (processTime > VISION_FRAME_LENGTH_uS) {
    //     if (processTime > VISION_FRAME_LENGTH_PRINT_THRESH_uS) {
    //         std::cout << "Time spent in ImageTranscriber loop longer than"
    //                   << " frame length: " << processTime << std::endl;
    //     }
    //     //Don't sleep at all
    // } else{
    //     const long int microSleepTime =
    //         static_cast<long int>(VISION_FRAME_LENGTH_uS - processTime);
    //     const long int nanoSleepTime =
    //         static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

    //     const long int secSleepTime =
    //         static_cast<long int>(microSleepTime / (1000*1000));

    //     // cout << "Sleeping for nano: " << nanoSleepTime
    //     //      << " and sec:" << secSleepTime << endl;

    //     interval.tv_sec = static_cast<time_t>(secSleepTime);
    //     interval.tv_nsec = nanoSleepTime;

    //     nanosleep(&interval, &remainder);
    // }
    // PROF_EXIT(P_MAIN);
    // PROF_NFRAME();
}
}
}
