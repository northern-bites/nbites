#include "ImageTranscriberModule.h"
#include <iostream>

using namespace portals;

namespace man {
namespace image {

ImageTranscriberModule::ImageTranscriberModule()
    : Module(),
      topImageOut(base()),
      bottomImageOut(base()),
      topImageTranscriber(Camera::TOP, &topImageOut),
      bottomImageTranscriber(Camera::BOTTOM, &bottomImageOut),
      lastProcessTimeAvg(VISION_FRAME_LENGTH_uS)
{
    topImageTranscriber.initTable("/home/nao/nbites/lib/table/top_table.mtb");
    bottomImageTranscriber.initTable("/home/nao/nbites/lib/table/bottom_table.mtb");
}

void ImageTranscriberModule::run_()
{
    // start timer
    const long long startTime = monotonic_micro_time();

    topImageTranscriber.acquireImage();
    bottomImageTranscriber.acquireImage();

    //stop timer
    const long long processTime = monotonic_micro_time() - startTime;

    //sleep until next frame
    lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

    if (processTime > VISION_FRAME_LENGTH_uS)
    {
        if (processTime > VISION_FRAME_LENGTH_PRINT_THRESH_uS) {
            std::cerr << "Time spent in image loop longer than"
                      << " frame length: " << processTime << std::endl;
        }
            //Don't sleep at all
    }
    else
    {
        const long int microSleepTime =
            static_cast<long int>(VISION_FRAME_LENGTH_uS - processTime);
        const long int nanoSleepTime =
            static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

        const long int secSleepTime =
            static_cast<long int>(microSleepTime / (1000*1000));

        //std::cerr << "Sleeping for nano: " << nanoSleepTime
        //<< " and sec:" << secSleepTime << std::endl;

        interval.tv_sec = static_cast<time_t>(secSleepTime);
        interval.tv_nsec = nanoSleepTime;

        nanosleep(&interval, &remainder);
    }

}

}
}
