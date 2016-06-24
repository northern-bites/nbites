#include "Detect.hpp"
#include "Transform.hpp"
#include "utilities-pp.hpp"

#include <iostream>
#include <cmath>
#include <signal.h>
#include <cmath>

#if DETECT_METHOD == DETECT_WITH_IMPULSE

using namespace nbsound;

namespace detect {

bool inited = false;
Transform * transform = nullptr;

const int transform_input_length = 4096;

typedef std::pair<int,int> Range;
typedef std::pair<int,float> Peak;

NBL_OSTREAM_OVERLOAD(Range, range, "r{" << range.first << "," << range.second << "}")
NBL_OSTREAM_OVERLOAD(Peak, peak, "p{" << peak.first << "," << peak.second << "}")

const int frequency_output_length = SPECTRUM_LENGTH( transform_input_length );
const Range full_range = {0, frequency_output_length};
const Range whistle_peak_range = {70,160};

const int min_frames_on = 4;
const int max_frames_off = 2;

const int peak_radius = 15;

const float min_sdev_start_ratio = 3.0;
const float min_integral_start_ratio = 6.0;
//const float min_integral_start_ratio = 10.0;

const float min_sdev_cont_ratio = .6;
const float min_integral_cont_ratio = 0.5;

Range range_around(int i, int r) { return {i - r, i + r}; }

bool in_range(int i, Range range) {
    return i >= range.first && i <= range.second;
}

bool in_range(const Peak& p, Range r) { return in_range(p.first, r); }

Peak peak(float * spec, const Range range = full_range ) {
    Peak max = {-1,-1};
    for (int i = range.first; i < range.second; ++i) {
        if (spec[i] > max.second) {
            max.first = i;
            max.second = spec[i];
        }
    }
    
    return max;
}

double sum(float * spec, const Range range = full_range) {
    double total = 0;
    for (int i = range.first; i < range.second; ++i) {
        total += spec[i];
    }
    
    return total;
}

double mean(float * spectrum) {
    return sum(spectrum) / frequency_output_length;
}

double sdev(float * spectrum) {
    double _mean = mean(spectrum);
    double _sdev = 0;
    
    for (int i = 0; i < frequency_output_length; ++i) {
        double val = spectrum[i] - _mean;
        _sdev += (val*val);
    }
    
    return std::sqrt(_sdev);
}

class NullOut {
public:
    template<typename T>
    NullOut& operator<<(T thing) {return *this;}
};

NullOut _NULL_OUT_;

#if true

#define START(i) debug(i)
#define END << std::endl;

#else

#define START(i) _NULL_OUT_
#define END ;

#endif

struct Channel {
    size_t _frame_index_;
    int _channel_;
    
    std::ostream& debug(size_t i) {
        std::string prefix(i, '\t');
        
        std::cout << prefix << "[" << _channel_ << "]" << "[" << _frame_index_
            << "]" << "[" << frames_on << ":" << frames_off << "] ";
        return std::cout;
    }
    
    float last_frame_spectrum[frequency_output_length];
    float last_frame_sdev;

    void set_last_peak(Peak pk, float sd, float * spectrum) {
        last_peak = pk; last_peak_sdev = sd;
        last_peak_integral = sum(spectrum, range_around(pk.first, peak_radius));
    }
    Peak last_peak = {-1,0.0};
    float last_peak_sdev;
    float last_peak_integral;
    
    int frames_on;
    int frames_off;

    bool hearing() {
        return last_peak.first > 0;
    }

    void stop_hearing() {
        last_peak = {-1, 0.0};
        last_peak_sdev = 0.01f;
    }

    Channel() {
        _frame_index_ = 0;

        reset();
    }

    void reset() {
        frames_on = frames_off = 0;
        last_frame_sdev = 0.01f;

        stop_hearing();
    }
    
    bool count(bool on) {
        if (on) {
            ++frames_on;
            frames_off = 0;
            return frames_on >= min_frames_on;
        } else {
            ++frames_off;
            
            if (frames_off >= max_frames_off) {
                frames_off = 0;
                frames_on = 0;

                stop_hearing();
            }
            
            return false;
        }
    }
    
    void take(float * spectrum_buffer) {
        memcpy(last_frame_spectrum, spectrum_buffer, sizeof(float) * frequency_output_length);
    }
    
    bool analyze(float * spectrum) {
        ++_frame_index_;
        bool on = false;
        
        Peak this_peak = peak(spectrum);
        double this_sdev = sdev(spectrum);

        START(0) << "this_peak " << this_peak << " this_sdev " << this_sdev END;
        
        if (in_range(this_peak.first, whistle_peak_range)) {
            //----------detection----------------

            if (!hearing()) {
                START(1) << "looking for new peak..." END;
                float sdev_ratio = this_sdev / last_frame_sdev;

                Range integral_range = range_around(this_peak.first, peak_radius);

                float sum_ratio =   sum(spectrum, integral_range) /
                            sum(last_frame_spectrum, integral_range);

                if ( sdev_ratio > min_sdev_start_ratio ) {
                     START(1) << "sdev_ratio " << sdev_ratio << " above thresh" END;
                } else {
                    START(1) << "sdev_ratio " << sdev_ratio << " FAILS" END;
                    goto failed;
                }

                if ( sum_ratio > min_integral_start_ratio ) {
                    START(1) << "sum_ratio " << sum_ratio << " above thresh" END;
                } else {
                    START(1) << "sum_ratio " << sum_ratio << " FAILS" END;
                    goto failed;
                }

                START(2) << "found new peak:" << this_peak.first END;
                set_last_peak(this_peak, this_sdev, spectrum);
                on = true;

            } else {
                START(1) << "continuing peak at " << last_peak.first << "..." END;

                if ( this_peak.second > last_peak.second ) {
                    START(1) << "magnitude increased to " << last_peak.second << "" END;

                    if (in_range(this_peak.first,
                                 range_around(last_peak.first, peak_radius))
                        ) {
                        START(2) << "updating last_peak!" END;
                        set_last_peak(this_peak, this_sdev, spectrum);
                        on = true;
                    } else {
                        START(2) << "NOT COUNTING AS PART OF LAST PEAK" END;
                        reset();
                        set_last_peak(this_peak, this_sdev, spectrum);
                        on = true;

                        START(2) << "RESET COUNT AND PEAK TO " << last_peak END;
                    }

                } else {
                    float sdev_ratio = this_sdev / last_peak_sdev;
                    Range integral_range = range_around(this_peak.first, peak_radius);

                    float sum_ratio = sum(spectrum, integral_range) / last_peak_integral;

                    if ( sdev_ratio < min_sdev_cont_ratio ) {
                        START(2) << "sdev ratio " << sdev_ratio << " fails to meet min!" END;
                        on = false;
                        goto failed;
                    }

                    if ( sum_ratio < min_integral_cont_ratio ) {
                        START(2) << "sum ratio " << sum_ratio << " fails to meet min!" END;
                        on = false;
                        goto failed;
                    }

                    START(3) << "this_peak good enough for continuation, frame on!" << last_peak END;
                    on = true;
                }
            }

            //----------printouts----------------
            
//            START(0) << "inside peak range" END;
//            
//            START(1) << "sdev: " << last_frame_sdev << " -> " << this_sdev
//                << " ratio " << this_sdev / last_frame_sdev END;
//            
//            double peak_sum_now = sum(spectrum, range_around(this_peak.first, peak_radius));
//            double peak_sum_thn = sum(last_frame_spectrum, range_around(this_peak.first, peak_radius));
//            
//            START(1) << "peak sum " << peak_sum_thn << " -> " << peak_sum_now << " ratio " << peak_sum_now / peak_sum_thn END

        } else {
            START(0) << "outside peak range" END;
            on = false;
        }

    failed:
        take(spectrum);
        last_frame_sdev = this_sdev;
        return count(on);
    }
};

Channel channels[2];    

bool init() {
    if (inited) return false;

    NBL_WARN(" [ whistle detector init running ] ")

#ifdef DETECT_LOG_RESULTS
    NBL_INFO("checking vector..")
    NBL_INFO("\tsize %d.", detect_results.size())
#endif

    channels[0]._channel_ = 0;
    channels[1]._channel_ = 1;

    transform = new Transform(transform_input_length);
    reset();

    return (inited = true);
}

bool reset() {
    for (int i = 0; i < 2; ++i) {
        channels[i].reset();
        channels[i]._frame_index_ = 0;
    }

#ifdef DETECT_LOG_RESULTS
    detect_results.clear();
#endif

    return true;
}

bool detect(nbsound::SampleBuffer& buffer) {

    init();

#ifdef DETECT_LOG_RESULTS
    detect_results.clear();
#endif

    for (int i = 0; i < buffer.channels; ++i) {
        int window_start = 0;
        int window_end = transform_input_length;

        while(buffer.is_in_bounds(i, window_end - 1)) {
            
            SampleBuffer window = buffer.window(window_start, window_end);
            transform->transform(window, i);

            bool heard = channels[i].analyze(transform->get_freq_buffer());

            if (heard)
                NBL_WARN(" [ WHISTLE DETECTOR HEARD ] ")

#ifdef DETECT_LOG_RESULTS
            nbl::Block block;
            block.data = transform->get_all();
            block.type = "WhistleDetection1";

            block.dict["Channel"] = json::Number(i);
            block.dict["window_start"] = json::Number(window_start);
            block.dict["window_end"] = json::Number(window_end);

            block.dict["WhistleWasHeard"] = json::Boolean(heard);

            detect_results.push_back(block);
#endif
            
            if (heard) return true;
            window_start += transform_input_length;
            window_end += transform_input_length;
        }
    }
    
    return false;
}

bool cleanup() {
    return true;
}

}

#endif
