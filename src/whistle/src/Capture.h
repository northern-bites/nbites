#ifndef _NB_SND_CAPTURE_H_
#define _NB_SND_CAPTURE_H_

#include "Sound.h"
#include <string>

namespace nbsound {
    
    class Capture : public Handler {
        
    public:
        Capture( Callback callback, parameter_t params );
        
        ~Capture();
        
        const void * buffer() const  {
            return snd_buffer;
        }
        
        bool stop();
        
        /* should handle io and callback until !should_run or error. */
        bool main();
        bool init();

        const std::string print() const;
        
    protected:
        //malloc'd, not new'd
        //size is always APP_BUFFER_SIZE(params)
        size_t snd_buffer_size;
        void * snd_buffer;
        
        snd_pcm_t * handle;
    };
}

#endif