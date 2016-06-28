#ifndef SOUND_WHISTLE_H
#define SOUND_WHISTLE_H

#include "Sound.hpp"
#include "Log.hpp"
#include "Transform.hpp"

#include <stdarg.h>

//#define DETECT_LOG_RESULTS

#ifdef DETECT_LOG_RESULTS
extern std::vector<nbl::Block> detect_results;
#endif

#define DETECT_PRINT

namespace detect {

    //do any expensive init (such as fft)
    bool init();

    //reset any temporal whistle stats
    bool reset();

    bool detect( nbsound::SampleBuffer& buffer, bool initialized );

    //clean up any system resources (currently unused)
    bool cleanup();

    extern const int PEAK_RADIUS;

    static const int AMPLITUDE_LENGTH = 4096;
    static const int FREQUENCY_LENGTH = SPECTRUM_LENGTH(AMPLITUDE_LENGTH);

    class Channel {

    public:

        void print(int indent, const char * format, ...) {
#ifdef DETECT_PRINT
            std::string in(indent, '\t');
            std::string prefix = nbl::utilities::format("%s[%C][%zd] (on%d/off%d)", in.c_str(),
                            _left_ ? 'L' : 'R', _iteration_,
                                                        frames_on, frames_off);

            va_list vaargs;
            va_start(vaargs, format);

            printf("%s", prefix.c_str() );
            vprintf(format, vaargs);
            std::cout << std::endl;

            va_end(vaargs);
#endif
        }

        Channel(){
            _left_ = true;
            _iteration_ = 0;
        }

        struct Peak {
            int center;
            float value;

            Peak(int c, float v) : center(c), value(v) {}
            Peak() : Peak(0,0.0) {}

            bool update(int c, float v) {
                if (v > value) {
                    center = c;
                    value = v;
                    return true;
                } else return false;
            }

            bool valid() {
                return center > 0;
            }
        };

        struct Range {
            int left, right;

            Range(int l, int r) : left(l), right(r) {}
            Range() : Range(0,0) {}

            bool contains(int i) const {
                return i >= left && i < right;
            }

            bool contains(const Peak& p) const  {
                return contains( p.center );
            }

            bool contains(const Range& other) const {
                return contains(other.left) && contains(other.right);
            }

            int count() const {
                return right - left;
            }

            static Range around(int i, int radius = PEAK_RADIUS) {
                Range ret;
                ret.left = std::max(0, i - radius);
                ret.right = std::min(FREQUENCY_LENGTH, i + radius);
                return ret;
            }

            static Range around(Peak& pk, int radius = PEAK_RADIUS) {
                return around(pk.center, radius);
            }
        };

        static const Range FULL_RANGE, WHISTLE_RANGE;
        static const Peak NO_PEAK;

        double sum_over( float * spectrum, const Range& range = FULL_RANGE);

        double sum_over( const Range& range = FULL_RANGE );
        double sum_except( const Range& except, const Range& over = FULL_RANGE );

        double mean_over( const Range& range = FULL_RANGE );
        double mean_except( const Range& except, const Range& over = FULL_RANGE );

        double sdev_over( const Range& range = FULL_RANGE );
        double sdev_except( const Range& except, const Range& over = FULL_RANGE );

        /* for debugging printouts */
        bool _left_;
        static size_t _iteration_;

        float this_spectrum[FREQUENCY_LENGTH];
        float last_spectrum[FREQUENCY_LENGTH];

        int frames_on, frames_off;

        struct frame_attributes {
            float sum;
            float sdev;
            float sdratio;
        };

        struct frame_attributes this_attr, last_attr, peak_attr;

        Peak current;
        Peak last_peak, this_peak;

        void loop_start(float * newSpec) {
            memcpy(this_spectrum, newSpec, sizeof(float) * FREQUENCY_LENGTH);

            current = NO_PEAK;
            for (int i = 0; i < FREQUENCY_LENGTH; ++i) {
                current.update(i, this_spectrum[i]);
            }

            this_attr.sum = sum_over(Range::around(current));
            this_attr.sdev = sdev_over();
            this_attr.sdratio = sdev_except( FULL_RANGE, Range::around(current)) / this_attr.sdev;
        }

        void loop_end() {
            memcpy(last_spectrum, this_spectrum, sizeof(float) * FREQUENCY_LENGTH);
            last_attr = this_attr;
            last_peak = this_peak;
        }

        bool count(bool on, int min_frames_on, int max_frames_off) {
            if (on) {
                ++frames_on;
                frames_off = 0;
                return frames_on >= min_frames_on;
            } else {
                ++frames_off;

                if (frames_off >= max_frames_off) {
                    frames_off = 0;
                    frames_on = 0;
                }

                return false;
            }
        }

        void reset() {
            frames_on = frames_off = 0;
            this_peak = last_peak = NO_PEAK;
        }
        
        bool analyze(float * spectrum, bool initialized) {

#ifdef DETECT_PRINT
            printf("\n");
#endif

            ++_iteration_;
            loop_start(spectrum);

            if (!initialized) {
                NBL_WARN("Channel::analyze() not calling _analyze()")
            }

            bool ret = initialized ? _analyze() : false;

            loop_end();

            return ret;
        }
        
        bool _analyze();
    };
    
    
}

#endif //SOUND_WHISTLE_H
