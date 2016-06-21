#include "Detect.hpp"
#include "Transform.hpp"

#include <iostream>
#include <cmath>
#include <signal.h>

#if DETECT_METHOD == DETECT_WITH_IMPULSE

using namespace nbsound;

namespace detect {

    bool inited = false;
    Transform * transform = nullptr;

    const int transform_input_length = 4096;
    const int transform_output_length = SPECTRUM_LENGTH( transform_input_length );

    double spectrum_last_frame[2][transform_output_length];

    const std::pair<int, int> start_range = {70,160};
    const float start_min_thresh = 50.0;
    //const float start_min_thresh = 0.0;

    const int mid_integral_radius = 15;
    const int mid_vary_radius = 10;

    const float sdev_min = 600.0;

    const int on_windows_target = 4;
    const int off_windows_fail = 2;

    int on_count[2];
    int off_count[2];
    int middle[2];

    void window_on(int ch) {
        ++on_count[ch];
        off_count[ch] = 0;
    }

    void window_off(int ch) {
        ++off_count[ch];
        if (off_count[ch] >= off_windows_fail) {
            off_count[ch] = 0;
            on_count[ch] = 0;
            middle[ch] = 0;
        }
    }

    std::pair<int, float> peak1() {
        int ri = -1;
        float rv = 0;
        for (int i = 0; i < transform->get_freq_len(); ++i) {
            double iv = std::abs(transform->get(i));
            if (iv > rv) {
                rv = iv;
                ri = i;
            }
        }

        return {ri, rv};
    }

    double sum(int start, int end) {
        double total = 0;
        int _s = std::max(start, 0);
        int _e = std::min(transform->get_freq_len(), end);

        for (int i = _s; i < _e; ++i) {
            float val = transform->get(i);
            total += val*val;
        }

        return total;
    }

    double mean() {
        double fullsum = sum(0, transform->get_freq_len());
        return fullsum / transform->get_freq_len() ;
    }

    double stddev(double mean) {
        double sdev = 0;

        for (int i = 0; i < transform->get_freq_len(); ++i) {
            double val = transform->get(i) - mean;
            sdev += (val*val);
        }

        return std::sqrt(sdev);
    }

    bool window_check(int i, int start, int end) {
        std::pair<int, float> peak = peak1();
        printf("c(%d) m(%d) on/off %d/%d w{%d -> %d} peak = {%d, %f}\n",
               i, middle[i], on_count[i], off_count[i],
               start, end, peak.first, peak.second);

        if (peak.first >= start_range.first && peak.first <= start_range.second) {
            printf("\tranged\n");
            if (peak.second >= start_min_thresh) {
                printf("\tmin_threshed\n");

                double total = sum(peak.first - mid_integral_radius, peak.first + mid_integral_radius);
                printf("\t\tsum %lf\n", total);

                double theMean = mean();
                double theDev = stddev(theMean);

                printf("\t\tmean %lf stddev %lf (needs %lf)\n",
                       theMean, theDev, sdev_min);

                if (theDev < sdev_min) {
                    return false;
                }

                if (middle[i] <= 0) {
                    middle[i] = peak.first;
                    printf("\t\tMIDDLE SET TO %d\n", peak.first);
                    window_on(i);
                    return true;

                } else if ( std::abs(middle[i] - peak.first) < mid_vary_radius ) {
                    window_on(i);
                    return true;

                } else {
                    printf("\t\t%d is %d from %d\n",
                           peak.first, std::abs(middle[i] - peak.first), middle[i]);
                }
            }
        }

        return false;
    }

    bool init() {
        if (inited) return false;

        NBL_WARN(" [ whistle detector init running ] ")

#ifdef DETECT_LOG_RESULTS
        NBL_INFO("checking vector..")
        NBL_INFO("\tsize %d.", detect_results.size())
#endif

        transform = new Transform(transform_input_length);
        reset();

        return (inited = true);
    }

    bool reset() {
        for (int i = 0; i < 2; ++i) {
            on_count[i] = 0;
            off_count[i] = 0;
            middle[1] = 0;
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


                if (!window_check(i, window_start, window_end)) {
                    window_off(i);
                }

                bool heard = false;

                if (on_count[i] >= on_windows_target) {
                    NBL_WARN(" [ whistle detector HEARD ] ")
                    heard = true;
                }

#ifdef DETECT_LOG_RESULTS
                nbl::Block block;
                block.data = transform->get_all();
                block.type = "WhistleDetection1";

                block.dict["Channel"] = json::Number(i);
                block.dict["window_start"] = json::Number(window_start);
                block.dict["window_end"] = json::Number(window_end);

                block.dict["WhistleWasHeard"] = json::Boolean(heard);

                json::Array ja_off, ja_on, ja_mid;
                for (int i = 0; i < 2; ++i) {
                    ja_on.push_back(json::Number(off_count[i]));
                    ja_off.push_back(json::Number(on_count[i]));
                    ja_mid.push_back(json::Number(middle[i]));
                }

                block.dict["WhistleOff"] = ja_off;
                block.dict["WhistleOn"] = ja_on;
                block.dict["WhistleMiddle"] = ja_mid;
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
