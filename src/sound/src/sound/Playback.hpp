//
//  Playback.hpp
//  xcode-sound2
//
//  Created by Philip Koch on 4/8/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#ifndef Playback_hpp
#define Playback_hpp

#include <stdio.h>
#include <string>

#include "Sound.hpp"

namespace nbsound {
    class Playback : public Handler {

    public:
        Playback( Callback cb, Config& conf );

        ~Playback();

        bool stop();

        /* should handle io and playback until !should_run or error. */
        bool main();
        bool init();

        const std::string print() const;

    protected:
        SampleBuffer buffer;
        snd_pcm_t * handle;
    };
}

#endif /* Playback_hpp */
