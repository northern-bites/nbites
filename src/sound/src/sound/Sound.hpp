#ifndef _NB_SND_SOUND_H_
#define _NB_SND_SOUND_H_

#ifdef __APPLE__

#include "/Users/pkoch/Robotics/nonbuild-includes/alsa-lib-1.0.29/include/asoundlib.h"

#else
#include <alsa/asoundlib.h>
//#include <asoundlib.h>
#endif

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
#include <stdint.h>
#include <pthread.h>

#include <vector>

#include "utilities.hpp"

//#define USE_CORRELATION

namespace nbsound {
    /* left and right  */
    static const int DEFAULT_NUM_CAPTURE_CHANNELS = 2;
    /* just one channel */
    static const int DEFAULT_NUM_PLAYBACK_CHANNELS = 2;

    /* alsa device specified.  run 'arecord -l' to list */
    static const char * DEF_CAPTURE_DEVICE = "hw:0,0,0";
    static const char * DEF_PLAYBACK_DEVICE = "hw:0,0,0";

    //static const char * DEF_CAPTURE_DEVICE = "hw:0,1";

    /* for both capture and playback */
    static const snd_pcm_access_t DEFAULT_ACCESS = SND_PCM_ACCESS_RW_INTERLEAVED;
    
    /* for snd_pcm_open calls, this means blocking non-asynchronous */
    static const int DEFAULT_MODE = 0;

    static const snd_pcm_format_t DEFAULT_FORMAT = SND_PCM_FORMAT_S16_LE;

    typedef int16_t nbs_sample_t;

    void alsa_print_info(snd_pcm_t * handle, snd_pcm_hw_params_t * hwp);

    class Config {
    public:
        int sample_rate = 48000;
        int window_size = 4096;

        const snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
        const int num_channels = 2;

        Config( int sr, int ws ) :
            sample_rate(sr),
            window_size(ws) {}
    };

    template<class T>
    class Buffer {
    private:

        void init_channels() {
            NBL_ASSERT_EQ(_channels.size(), 0)

            for (int i = 0; i < channels; ++i) {
                _channels.push_back(BufferChannel(i, *this));
            }
        }

        bool needs_delete;

        Buffer(Buffer& outer, int frame_start, int frame_end) :
            channels(outer.channels), frames(frame_end - frame_start)
        {
            NBL_ASSERT(frames > 0);
            NBL_ASSERT(frame_start >= 0);
            NBL_ASSERT(frame_end <= outer.frames);

            needs_delete = false;
            buffer = outer.buffer + (frame_start * outer.channels);

            init_channels();
        }

    public:
        T * buffer;
        int channels;
        int frames;

        class BufferChannel {
        public:
            int ch;
            Buffer& outer;

            BufferChannel(int c, Buffer& outer) :
            ch(c), outer(outer)
            { }

            T& operator[](int f) {
                return outer.buffer[f * outer.channels + ch];
            }
        };

        std::vector<BufferChannel> _channels;

        Buffer(int channels, int frames) :
            channels(channels), frames(frames)
        {
            buffer = new T[channels * frames];
            needs_delete = true;

            init_channels();
        }

        ~Buffer() {
            if (needs_delete && buffer) delete[] buffer;
        }

        BufferChannel& operator[](int c) {
            return _channels[c];
        }

        bool is_in_bounds(int chan, int frm) {
            return (chan < channels) && (frm < frames);
        }

        Buffer window(int frame_start, int frame_end) {
            return Buffer(*this, frame_start, frame_end);
        }

        size_t sample_bytes() {
            return sizeof(T);
        }

        size_t sample_max() {
            return std::numeric_limits<T>::max();
        }

        size_t size_bytes() {
            return sizeof(T) * frames * channels;
        }

        size_t size_samples() {
            return frames * channels;
        }
    };

    typedef Buffer<nbs_sample_t> SampleBuffer;
    typedef Buffer<double> TransformBuffer;
    
    class Handler;  //Same format for both capture and playback handlers.

    typedef void (*Callback)(Handler&, Config&, SampleBuffer&);
    
    class Handler {
    public:
        
        Handler(Callback cb, Config& conf) :
            callback(cb),
            config(conf),
            finished(false),
            should_run(false)
        { }
        
        //Don't delete until !is_active()
        bool is_active() const { return !finished; }
        bool is_running() const { return should_run; }
        const Config& get_config() const {return config;}
        Callback get_callback() const {return callback;}
        
        virtual bool stop() = 0;
        
        /* should handle io and callback until !should_run or error. */
        virtual bool main() = 0;
        /* set should_run and initiate any handles/buffers */
        virtual bool init() = 0;

        virtual ~Handler(){};
        
        void start_new_thread(pthread_t& thread, pthread_attr_t * attr);
        
    protected:
        Config config;
        Callback callback;

        //active is whether main is still looping
        volatile bool finished;
        //should_run is whether main should try to start another loop
        volatile bool should_run;
        
    };
}

#endif
