#ifndef SOUND_WHISTLE_H
#define SOUND_WHISTLE_H

#include "Sound.hpp"
#include "Log.hpp"

//#define DETECT_LOG_RESULTS

#ifdef DETECT_LOG_RESULTS

extern std::vector<nbl::Block> detect_results;

#endif

#define DETECT_WITH_SDEV 1
#define DETECT_WITH_IMPULSE 2

#define DETECT_METHOD DETECT_WITH_SDEV

namespace detect {

    //do any expensive init (such as fft)
    bool init();

    //reset any temporal whistle stats
    bool reset();

    bool detect(nbsound::SampleBuffer& buffer);

    //clean up any system resources (currently unused)
    bool cleanup();
}

#endif //SOUND_WHISTLE_H
