#include "Detect.hpp"
#include "Transform.hpp"
#include "utilities-pp.hpp"

#include <iostream>
#include <cmath>
#include <signal.h>
#include <cmath>

using namespace nbsound;

namespace detect {

    bool inited = false;
    Transform * transform = nullptr;

    Channel channels[2];

    bool init() {
        if (inited) return false;

        NBL_WARN(" [ whistle detector init running ] ")

#ifdef DETECT_LOG_RESULTS
        NBL_INFO("checking vector..")
        NBL_INFO("\tsize %d.", detect_results.size())
#endif

        channels[0]._left_ = true;
        channels[1]._left_ = false;

        transform = new Transform(AMPLITUDE_LENGTH);
        reset();

        return (inited = true);
    }

    bool reset() {
        for (int i = 0; i < 2; ++i) {
            channels[i].reset();
        }

        Channel::_iteration_ = 0;

#ifdef DETECT_LOG_RESULTS
        detect_results.clear();
#endif

        return true;
    }

    bool detect(nbsound::SampleBuffer& buffer, bool initialized) {

        init();

#ifdef DETECT_LOG_RESULTS
        detect_results.clear();
#endif

        for (int i = 0; i < buffer.channels; ++i) {
            int window_start = 0;
            int window_end = AMPLITUDE_LENGTH;

            while(buffer.is_in_bounds(i, window_end - 1)) {

                SampleBuffer window = buffer.window(window_start, window_end);
                transform->transform(window, i);

                bool heard = channels[i].analyze(transform->get_freq_buffer(), initialized);

                if (heard) {
                    NBL_WARN(" [ WHISTLE DETECTOR HEARD ] ")
                }

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

                window_start += AMPLITUDE_LENGTH;
                window_end += AMPLITUDE_LENGTH;
            }
        }

        return false;
    }

    bool cleanup() {
        return false;
    }


    const int PEAK_RADIUS = 30;
    const int SUM_PEAK_RADIUS = 15;
    const int FRAMES_ON = 2;
    const int FRAMES_OFF = 2;

    const float SDEV_IMPULSE_START = 3.0;
    const float SUM_IMPULSE_START = 5.0;

    const float CONT_SDEV_RATIO_TO_PEAK = 0.6;
    const float CONT_SUM_RATIO_TO_PEAK = 0.5;

    const float MAX_SDRATIO_ALWAYS = 0.4;
    const float MAX_SDRATIO_PEAK = 0.4;

    const float MIN_SDRATIO_CLOSE = 0.25;

    bool Channel::_analyze() {

        print(0, "peak @ %d {%f}: sum %f sdev %f sdratio %f", current.center, current.value, this_attr.sum, this_attr.sdev, this_attr.sdratio);

        const Range& range_check = this_attr.sdratio < MIN_SDRATIO_CLOSE ? WHISTLE_RANGE_CLOSE : WHISTLE_RANGE ;

        if (!range_check.contains(current)) {
            print(1, "not in range {%d, %d}",
                  range_check.left, range_check.right);

            return count(false, FRAMES_ON, FRAMES_OFF);
        }

        if ( this_attr.sdratio > MAX_SDRATIO_ALWAYS ) {
            print(1, "frame sdev ratio fails %f (%f)",
                  this_attr.sdratio, MAX_SDRATIO_ALWAYS);

            return count(false, FRAMES_ON, FRAMES_OFF);
        }

        if (last_peak.valid() && Range::around(last_peak).contains(current)) {

            print(1, "continuing peak at %d...",
                  last_peak.center );
            bool passing = true;
            //do checks for continuing peak...

//            float sdev_ratio_to_peak = this_attr.sdev / peak_attr.sdev;
//            if (passing && (sdev_ratio_to_peak < CONT_SDEV_RATIO_TO_PEAK)) {
//                print(1, "sdev ratio to peak fails: %f (%f)",
//                      sdev_ratio_to_peak, CONT_SDEV_RATIO_TO_PEAK);
//                passing = false;
//            }

            float sum_ratio_to_peak = this_attr.sum / peak_attr.sum;
            if (passing && (sum_ratio_to_peak < CONT_SUM_RATIO_TO_PEAK)) {
                print(1, "sum ratio to peak fails: %f (%f)",
                      sum_ratio_to_peak, CONT_SUM_RATIO_TO_PEAK);
                passing = false;
            }

            //Should we update the peak index? (sliding whistle)

            if (passing) {
                if (current.value > last_peak.value) {
                    this_peak = current;
                    peak_attr = this_attr;
                } else {
                    this_peak = last_peak;
                }

                return count(true, FRAMES_ON, FRAMES_OFF);
            }
        }

        //Check for the start of a peak

        bool passing = true;

        print(1, "looking for new peak...");

//        float sdev_impulse = this_attr.sdev / last_attr.sdev;
//        if ( passing && (sdev_impulse < SDEV_IMPULSE_START) ) {
//            print(1, "new peak sdev impulse %f fails (%f)",
//                  sdev_impulse, SDEV_IMPULSE_START);
//            passing = false;
//        }

        float sum_impulse = this_attr.sum / sum_over(last_spectrum, Range::around(this_peak));
        if ( passing && (sum_impulse < SUM_IMPULSE_START) ) {
            print(1, "new peak sum impulse %f fails (%f)", sum_impulse, SUM_IMPULSE_START);
            passing = false;
        }

        if (passing && (this_attr.sdratio > MAX_SDRATIO_PEAK)) {
            print(1, "new peak sdratio %f fails (%f)", this_attr.sdratio, MAX_SDRATIO_PEAK);
            passing = false;
        }

        if (passing) {
            this_peak = current;
            peak_attr = this_attr;
            return count(true, FRAMES_ON, FRAMES_OFF);
        } else {
            return count(false, FRAMES_ON, FRAMES_OFF);
        }
    }
    
}


























