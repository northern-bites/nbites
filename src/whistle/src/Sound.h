#ifndef _NB_SND_SOUND_H_
#define _NB_SND_SOUND_H_

#ifdef __APPLE__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/poll.h>
#include <errno.h>
#include <stdarg.h>

#include "asoundef.h"
#include "version.h"
#include "global.h"
#include "input.h"
#include "output.h"
#include "error.h"
#include "conf.h"
#include "pcm.h"
#include "rawmidi.h"
#include "timer.h"
#include "hwdep.h"
#include "control.h"
#include "mixer.h"
#include "seq_event.h"
#include "seq.h"
#include "seqmid.h"
#include "seq_midi_event.h"

#else
#include <alsa/asoundlib.h>
//#include <asoundlib.h>
#endif

#include <stdint.h>
#include <pthread.h>

namespace nbsound {
    /* left and right  */
    static const int DEF_NUM_CAPTURE_CHNL = 2;
    /* alsa device specified.  run 'arecord -l' to list */
    static const char * DEF_CAPTURE_DEVICE = "hw:0,0,0";
    static const char * DEF_PLAYBACK_DEVICE = "hw:0,0,0";
    //static const char * DEF_CAPTURE_DEVICE = "hw:0,1";
    
    /* for both capture and playback */
    static const snd_pcm_access_t DEFAULT_ACCESS = SND_PCM_ACCESS_RW_INTERLEAVED;
    
    /* for snd_pcm_open calls, this means blocking non-asynchronous */
    static const int DEFAULT_MODE = 0;
    
    /* sample format for both capture and playback */
    typedef enum {
        NBS_S16_LE = SND_PCM_FORMAT_S16_LE,
        NBS_S32_LE = SND_PCM_FORMAT_S32_LE
    } nbs_format;
    
    /* set of the significant parameters for an audio capture session */
    typedef struct {
        nbs_format  format;
        int         channels;
        int         frames;
        
        //Does not affect buffer size, only period and sample frequency
        int         rate;
    } parameter_t;
    
    static inline int SAMPLE_BYTES(const nbs_format& format) {
        switch (format) {
            case NBS_S16_LE:
                return sizeof(int16_t);
                break;
            case NBS_S32_LE:
                return sizeof(int32_t);
            default:
                return -1;
                break;
        }
    }
    
    static inline int FRAME_SIZE(const parameter_t& params) {
        return params.channels * SAMPLE_BYTES(params.format);
    }
    
    static inline int APP_BUFFER_SIZE(const parameter_t& params) {
        return params.frames * FRAME_SIZE(params);
    }
    
    static inline double FREQUENCY(const parameter_t& params) {
        return ((double) params.rate) / (FRAME_SIZE(params));
    }
    
    static inline double PERIOD(const parameter_t& params) {
        return ((double) APP_BUFFER_SIZE(params) / (params.rate) );
    }
    
    static inline void alsa_print_info(snd_pcm_t * handle, snd_pcm_hw_params_t * hwp) {
        printf("device [%s] opened with\n",
               snd_pcm_name(handle));
        printf("\tstate=%s\n",
               snd_pcm_state_name(snd_pcm_state(handle)));
        unsigned int val, val2;
        snd_pcm_hw_params_get_access(hwp,
                                     (snd_pcm_access_t *) &val);
        printf("\taccess_type=%s\n",
               snd_pcm_access_name((snd_pcm_access_t)val));
        
        snd_pcm_hw_params_get_format(hwp, (snd_pcm_format_t *) &val);
        printf("\tformat=%s\n",
               snd_pcm_format_name((snd_pcm_format_t) val)
               );
        
        snd_pcm_hw_params_get_channels(hwp, &val);
        printf("\tchannels=%d\n", val);
        
        snd_pcm_hw_params_get_rate(hwp, &val, (int *) &val2);
        printf("\trate=%d bps\n", val);
        
        snd_pcm_hw_params_get_period_time(hwp,
                                          &val, (int *) &val2);
        printf("\tperiod_time=%d us\n", val);
        
        snd_pcm_uframes_t frames;
        snd_pcm_hw_params_get_period_size(hwp,
                                          &frames, (int *) &val2);
        printf("\tperiod_size=%d frames\n", (int)frames);
        
        snd_pcm_hw_params_get_buffer_size(hwp,
                                          (snd_pcm_uframes_t *) &val);
        printf("\tbuffer_size=%d frames\n", val);
        
        snd_pcm_hw_params_get_periods(hwp, &val, (int *) &val2);
        printf("\tperiods_per_buffer=%d periods\n", val);
    }
    
    class Handler;  //Same format for both audio and output handlers.
    typedef void (*Callback)(Handler *, void *, parameter_t *);
    
    class Handler {
    public:
        
        Handler(Callback cb, parameter_t prms) :
            callback(cb),
            params(prms),
            finished(false),
            should_run(false)
        { }
        
        //Don't delete until !is_active()
        bool is_active() const { return !finished; }
        bool is_running() const { return should_run; }
        const parameter_t& get_params() const {return params;}
        Callback get_callback() const {return callback;}
        
        virtual bool stop() = 0;
        
        /* should handle io and callback until !should_run or error. */
        virtual bool main() = 0;
        /* set should_run and initiate any handles/buffers */
        virtual bool init() = 0;
        
        void start_new_thread(pthread_t& thread, pthread_attr_t * attr);
        
    protected:
        parameter_t params;
        
        //active is whether main is still looping
        volatile bool finished;
        //should_run is whether main should try to start another loop
        volatile bool should_run;
        
        Callback callback;
    };
}

#include "Capture.h"

#endif
