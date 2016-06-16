//
//  Playback.cpp
//  xcode-sound2
//
//  Created by Philip Koch on 4/8/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "Playback.hpp"
#include "utilities.hpp"

#include <ostream>
#include <sstream>

namespace nbsound {

    Playback::Playback( Callback cb, Config& conf ) : Handler(cb, conf),
        buffer(conf.num_channels, conf.window_size)
    {
        handle = NULL;
    }

    Playback::~Playback() { NBL_ASSERT_EQ(handle, NULL) }

    const std::string Playback::print() const {
        std::ostringstream ss;
        char buffer[100];
        snprintf(buffer, 100, "finished=%i, should_run=%i, callback=%p, rate=%d, chnls=%d, wind=%d", finished, should_run, callback, config.sample_rate, config.num_channels, config.window_size);

        ss << "Playback( " << buffer << " )";

        return ss.str();
    }

    bool Playback::init() {
        if (finished || should_run) {
            NBL_ERROR("Playback::init() called on %s which is IN INVALID STATE!\n", print().c_str());

            return false;
        }

        if (handle) {
            NBL_ERROR("Playback::init() called on %s which has ALREADY BEEN INITIALIZED!\n", print().c_str());

            return false;
        }

        int err;
        snd_pcm_hw_params_t * hw_params;

        //Open the device handle
        err = snd_pcm_open(&handle, DEF_PLAYBACK_DEVICE, SND_PCM_STREAM_PLAYBACK, DEFAULT_MODE);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not open handle!: %s\n", snd_strerror(err));
            return false;
        }

        //Allocate hw_params on the stack.
        snd_pcm_hw_params_alloca(&hw_params);

        //Load default settings into our params structure
        err = snd_pcm_hw_params_any(handle, hw_params);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not load default parameters: %s\n", snd_strerror(err));
            return false;
        }

        //Set access mode
        err = snd_pcm_hw_params_set_access(handle, hw_params, DEFAULT_ACCESS);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not set access: %s\n", snd_strerror(err));
            return false;
        }

        //Set data format
        err = snd_pcm_hw_params_set_format(handle, hw_params, DEFAULT_FORMAT);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not set format: %s\n", snd_strerror(err));
            return false;
        }

        //Set number of channels to use
        err = snd_pcm_hw_params_set_channels(handle, hw_params, DEFAULT_NUM_PLAYBACK_CHANNELS);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not set %i channels: %s\n",DEFAULT_NUM_PLAYBACK_CHANNELS, snd_strerror(err));
            return false;
        }

        //Try to set the requested rate, actual set rate is returned.
        int req_dir = 0;
        unsigned int req_rate = config.sample_rate;

        err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &req_rate, &req_dir);
        if (err < 0) {
            NBL_ERROR("Playback::init() could not set rate near %u: %s", req_rate, snd_strerror(err));
            return false;
        }

        printf("Playback::init() rate set to %u (%i) requested %i (0)\n", req_rate, req_dir, config.sample_rate);
        NBL_ASSERT(req_rate == config.sample_rate);

        err = snd_pcm_hw_params(handle, hw_params);
        if (err < 0) {
            NBL_ERROR("Playback::init() COULD NOT SET HW_PARAMS TO DEVICE: %s\n", snd_strerror(err));
            return false;
        }

        printf("\n-------------------------\n");
        alsa_print_info(handle, hw_params);

        should_run = true;
        return true;
    }

    bool Playback::main() {

        if (finished || !should_run) {
            printf("Playback::main() invalid state for main()\n");
            return false;
        }

        printf("Playback::main() filling buffer...\n");
        this->callback(*this, config, buffer);

        while (should_run) {
//            printf("playback loop...\n");
            size_t nframes = buffer.frames;
            
            long err = snd_pcm_writei(handle, buffer.buffer, nframes);
            if (err != nframes) {
                NBL_ERROR("read failed with: %s\n", snd_strerror( (int) err));
                finished = true;
                handle = nullptr;
                return false;
            } else {
                callback(*this, config, buffer);
            }
        }

        printf("Playback::main() closing handle.\n");
        int r1 = snd_pcm_drain(handle);
        int r2 = snd_pcm_close(handle);

        if (r1 < 0) {
            printf("Error draining handle, %s\n",
                   snd_strerror(r1));
        }

        if (r2 < 0) {
            printf("Error closing handle, %s\n",
                   snd_strerror(r2));
        }

        finished = true;
        handle = nullptr;
        return true;
    }

    bool Playback::stop() {
        if (should_run && !finished) {
            should_run = false;
            return true;
        } else {
            return false;
        }
    }

}