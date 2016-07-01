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

        Buffer& operator=(Buffer& other);   //no assignment operator

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

        Buffer(const Buffer& other) :
            channels(other.channels), frames(other.frames)
        {
            buffer = new T[channels * frames];
            needs_delete = true;

            init_channels();
            memcpy(buffer, other.buffer, other.size_bytes());
        }

        void take(const Buffer& other) {
            NBL_ASSERT_EQ(other.frames, frames)
            NBL_ASSERT_EQ(other.channels, channels)

            memcpy(buffer, other.buffer, other.size_bytes());
        }

        BufferChannel& operator[](int c) {
            return _channels[c];
        }

        bool is_in_bounds(int chan, int frm) const {
            return (chan < channels) && (frm < frames);
        }

        Buffer window(int frame_start, int frame_end) {
            return Buffer(*this, frame_start, frame_end);
        }

        size_t sample_bytes() const {
            return sizeof(T);
        }

        size_t sample_max() const {
            return std::numeric_limits<T>::max();
        }

        size_t size_bytes() const {
            return sizeof(T) * frames * channels;
        }

        size_t size_samples() const {
            return frames * channels;
        }

        std::string toString() const {
            return std::string((const char *) buffer, size_bytes());
        }
    };

    typedef Buffer<nbs_sample_t> SampleBuffer;
    typedef Buffer<float> TransformBuffer;

    class SampleRingBuffer {
        int position;
        std::vector<SampleBuffer> buckets;
    public:
        
        SampleRingBuffer(int nbuck, int nchan, int nframe)
        {
            NBL_ASSERT(nbuck > 0);

            for (int i = 0; i < nbuck; ++i) {
                buckets.push_back(SampleBuffer{nchan, nframe});
            }

            position = 0;
        }

        void push(SampleBuffer& fb) {
            NBL_ASSERT(fb.size_bytes() == buckets[position].size_bytes());
            memcpy(buckets[position].buffer, fb.buffer, fb.size_bytes());

            position = (position + 1) % buckets.size();
        }

        std::string toString() {
            std::string ret;
            ret.reserve( buckets.size() * buckets[0].size_bytes() );

            for (int i = 0; i < buckets.size(); ++i) {
                const int index = (position + i) % buckets.size();
                ret.append( (const char *) buckets[index].buffer, buckets[index].size_bytes() );
            }

            return ret;
        }

        NBL_DISALLOW_COPY(SampleRingBuffer)
    };

//    class SampleBufferArray {
//        std::vector<SampleBuffer *> buffers;
//
//    public:
//
//        SampleBufferArray() { }
//
//        void add(SampleBuffer& buffer) {
//            buffers.push_back(&buffer);
//        }
//
//        nbs_sample_t get(size_t channel, size_t offset) {
//            size_t ioff = offset;
//            size_t bi = 0;
//
//            for (; ioff >= buffers[bi]->frames; ++bi ) {
//                ioff -= buffers[bi]->frames;
//            }
//
//            NBL_ASSERT(ioff >= 0);
//            NBL_ASSERT(ioff < buffers[bi]->frames)
//
//            SampleBuffer::BufferChannel& sbc = buffers[bi]->operator[](channel);
//            return sbc[ioff];
//        }
//    };

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
