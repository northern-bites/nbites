#include "Capture.h"
#include <sstream>

namespace nbsound {
    
    Capture::Capture( Callback callback, parameter_t params ) : Handler(callback, params)
    {
        snd_buffer_size = APP_BUFFER_SIZE(params);
        snd_buffer = malloc(snd_buffer_size);

        handle = NULL;
        
    }
    
    Capture::~Capture() {
        free(snd_buffer);
    }
    
    bool Capture::init() {
        if (finished || should_run) {
            printf("Capture::init() called on %s which is IN INVALID STATE!\n", print().c_str());
            
            return false;
        }
        
        if (handle) {
            printf("Capture::init() called on %s which has ALREADY BEEN INITIALIZED!\n", print().c_str());
            
            return false;
        }
        
        int err;
        snd_pcm_hw_params_t * hw_params;
        
        //Open the device handle
        err = snd_pcm_open(&handle, DEF_CAPTURE_DEVICE, SND_PCM_STREAM_CAPTURE, DEFAULT_MODE);
        if (err < 0) {
            printf("Capture::init() could not open handle!: %s\n", snd_strerror(err));
            return false;
        }
        
        //Allocate hw_params on the stack.
        snd_pcm_hw_params_alloca(&hw_params);
        
        //Load default settings into our params structure
        err = snd_pcm_hw_params_any(handle, hw_params);
        if (err < 0) {
            printf("Capture::init() could not load default parameters: %s\n", snd_strerror(err));
            return false;
        }
        
        //Set access mode
        err = snd_pcm_hw_params_set_access(handle, hw_params, DEFAULT_ACCESS);
        if (err < 0) {
            printf("Capture::init() could not set access: %s\n", snd_strerror(err));
            return false;
        }
        
        //Set data format
        err = snd_pcm_hw_params_set_format(handle, hw_params, (snd_pcm_format_t) params.format);
        if (err < 0) {
            printf("Capture::init() could not set format: %s\n", snd_strerror(err));
            return false;
        }
        
        //Set number of channels to use
        err = snd_pcm_hw_params_set_channels(handle, hw_params, params.channels);
        if (err < 0) {
            printf("Capture::init() could not set %i channels: %s\n", params.channels, snd_strerror(err));
            return false;
        }
        
        //Try to set the requested rate, actual set rate is returned.
        int req_dir = 0;
        unsigned int req_rate = params.rate;
        
        err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &req_rate, &req_dir);
        if (err < 0) {
            printf("Capture::init() could not set rate near %u: %s\n", req_rate, snd_strerror(err));
            return false;
        }
        
        printf("Capture::init() rate set to %u (%i) requested %i (0)\n", req_rate, req_dir, params.rate);
        
        err = snd_pcm_hw_params(handle, hw_params);
        if (err < 0) {
            printf("Capture::init() COULD NOT SET HW_PARAMS TO DEVICE: %s\n", snd_strerror(err));
            return false;
        }
        
        printf("\n-------------------------\n");
        alsa_print_info(handle, hw_params);
        
        should_run = true;
        return true;
    }
    
    bool Capture::main() {
        if (finished || !should_run) {
            printf("Capture::run() invalid state for run()\n");
            return false;
        }
        
        while (should_run) {
            long err = snd_pcm_readi(handle, snd_buffer, params.frames);
            if (err != params.frames) {
                printf("read failed with: %s\n", snd_strerror( (int) err));
                return false;
            } else {
                callback(this, snd_buffer, &params);
            }
        }
   
        printf("Capture::main() closing handle.\n");
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
        return true;
    }
    
    bool Capture::stop() {
        if (should_run && !finished) {
            should_run = false;
            return true;
        } else {
            return false;
        }
    }
    
    const std::string Capture::print() const {
        std::ostringstream ss;
        char buffer[100];
        snprintf(buffer, 100, "finished=%i, should_run=%i, callback=%p, buffer=%p", finished, should_run, callback, snd_buffer);
        
        char buffer2[100];
        snprintf(buffer2, 100, "[rate=%i, chnls=%i, fpb=%i]", params.rate, params.channels, params.frames);
        
        ss << "Capture( " << buffer << ", " << buffer2 << " )";
        
        return ss.str();
    }
}