#ifndef SOUND_WHISTLE_H
#define SOUND_WHISTLE_H

#include "Sound.hpp"
#include "Log.hpp"

//#define DETECT_LOG_RESULTS

#ifdef DETECT_LOG_RESULTS

extern std::vector<nbl::Block> detect_results;

#endif

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