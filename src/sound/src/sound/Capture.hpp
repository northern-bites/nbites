#ifndef _NB_SND_CAPTURE_H_
#define _NB_SND_CAPTURE_H_

#include "Sound.hpp"
#include <string>

namespace nbsound {
    
    class Capture : public Handler {
        
    public:
        Capture( Callback callback, Config& conf );
        
        ~Capture();
        
        bool stop();
        
        /* should handle io and callback until !should_run or error. */
        bool main();
        bool init();

        const std::string print() const;
        
    protected:
        snd_pcm_t * handle;
        SampleBuffer buffer;
    };
}

#endif