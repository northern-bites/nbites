//
//  SimpleFSK.cpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#include "SimpleFSK.hpp"
#include "utilities-test.hpp"
#include "../sound/Transform.hpp"
#include "Correlation.hpp"

using namespace nbsound;

namespace nowifi {

    //48khz sampling rate
    const int SIMPLE_FSK_WINDOW_SIZE = 8192;
    const int SIMPLE_FSK_START_F = 15000;
    const int SIMPLE_FSK_0_F = 12000;
    const int SIMPLE_FSK_1_F = 18000;

    const double SIMPLE_FSK_START_THRESH = 2.0;

    CorrSender filler_start(SIMPLE_FSK_START_F);
    CorrSender filler_0(SIMPLE_FSK_0_F);
    CorrSender filler_1(SIMPLE_FSK_1_F);

    CorrRecvr  detect_start(nullptr, SIMPLE_FSK_START_F);
    Transform  detect_data(SIMPLE_FSK_WINDOW_SIZE);

    void SimpleFSKSendr::fill(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

        if (!isSending()) {
            memset(buffer.buffer, 0, buffer.size_bytes());
            return;
        }

        if (current_offset == 0) {
            NBL_WARN("current_offset 0, FIRST FRAME");
            filler_start.fill(buffer, conf);
        } else {
            size_t bit = (current_offset) - 1;
            size_t byte = bit / 8;
            NBL_INFO("data frame %zd bit %zd byte %zd",
                   current_offset, bit, byte);

            if (! (byte < current.size() ) ) {
                NBL_WARN("send finished [of %zd bytes, at current_offset %zd]",
                       current.size(), current_offset);

                sending = false;
                current.clear();

                /* send hangup frame */
                filler_start.fill(buffer, conf);
                return;
            }

            uint8_t theByte = current[byte];
            int bitIndex = bit & 0x07;
            int bitMask = (1 << bitIndex);
            uint8_t bitSet = theByte & bitMask;

            NBL_INFO("bit %zd of 0x%x [index %d mask 0x%x] was %x",
                     bit, theByte, bitIndex, bitMask, bitSet
                     );

            if (bitSet) {
                filler_1.fill(buffer, conf);
            } else {
                filler_0.fill(buffer, conf);
            }
        }

        ++current_offset;
    }

    void SimpleFSKRecvr::parse(nbsound::SampleBuffer &frame_2, nbsound::Config &conf) {
        NBL_INFO("iteration %zd mode %s",
                 iteration, ModeStrings[current]);

        double start_magnitude = 0;

        if (current == CALIBRATING || current == LISTENING) {
            detect_start.parse(frame_2, conf);
            start_magnitude = detect_start.bin.magnitude();

            NBL_INFO("correlated, magnitude is %lf", start_magnitude)
        }

        switch (current) {
            case CALIBRATING: {
                running_start_mag += start_magnitude;

                if (iteration == (NUM_FRAMES_TO_CALIBRATE - 1)) {
                    current = LISTENING;
                }
            } break;

            case LISTENING: {
                double average = running_start_mag / iteration;

                NBL_INFO("%le average, %le magnitude", average, start_magnitude)
                if ( start_magnitude > (average * SIMPLE_FSK_START_THRESH) ) {

                    NBL_INFO("%le mag %le average :: MOVING TO SEARCHING")
                    current = SEARCHING;
                    start = time(NULL);
                    recving = true;
                    data.clear();

                } else {
                    running_start_mag += start_magnitude;
                }
            } break;

            case SEARCHING: {
                do_search(frame_0, frame_1, frame_2, conf);
                current = RECORDING;
            } break;

            case RECORDING: {
                int result = do_parse(frame_1, frame_2, conf);
                if (result < 0) {
                    if ( data.empty() ) {
                        NBL_WARN("ignoring empty first frame");
                        break;
                    }

                    NBL_WARN("!!! finished !!!");

                    std::string output = data_so_far();
                    data.clear();
                    recving = false;
                    current = LISTENING;

                    finish(output);
                } else {
                    data.push_back(result);
                }
            } break;

            default:
                NBL_ERROR("bad mode in parse(): (%d)",
                          current); assert(false);
        }

        frame_0.take(frame_1);
        frame_1.take(frame_2);
        ++iteration;
    }

    void SimpleFSKRecvr::do_search(nbsound::SampleBuffer &f0, nbsound::SampleBuffer &f1, nbsound::SampleBuffer &f2, nbsound::Config &conf) {
        double magnitudes[3];

        detect_start.parse(f0, conf);
        magnitudes[0] = detect_start.bin.magnitude();

        detect_start.parse(f1, conf);
        magnitudes[1] = detect_start.bin.magnitude();

        detect_start.parse(f2, conf);
        magnitudes[2] = detect_start.bin.magnitude();

        int     max_i = 0;
        double  max_m = 0;

        for (int i = 0; i < 3; ++i) {
            if ( magnitudes[i] > max_m ) {
                max_m = magnitudes[i];
                max_i = i;
            }
        }

        NBL_INFO("max in %d (%lf)", max_i, max_m);

        int frame_start;
        double f;

        SampleBuffer * start = NULL, * end = NULL;

        switch (max_i) {
            case 0:
                NBL_ERROR("case 0! this is not supposed to happen!")

                f = magnitudes[1] /
                    ( magnitudes[0] + magnitudes[1] );
                NBL_INFO("case 0: fraction %lf", f)
                frame_start = f * SIMPLE_FSK_WINDOW_SIZE;

                start = &f0;
                end = &f1;
                break;

            case 1:
                if ( magnitudes[0] > magnitudes[2] ) {
                    f = magnitudes[0] /
                        (magnitudes[0] + magnitudes[1]);
                    NBL_INFO("case 1 low: fraction %lf", f)
                    frame_start = (1 - f) * SIMPLE_FSK_WINDOW_SIZE;
                    start = &f0;
                    end = &f1;
                } else {
                    f = magnitudes[2] /
                        (magnitudes[1] + magnitudes[2]);
                    NBL_INFO("case 1 hgh: fraction %lf", f)
                    frame_start = (1 + f) * SIMPLE_FSK_WINDOW_SIZE;
                    start = &f1;
                    end = &f2;
                }

                break;

            case 2:
                f = magnitudes[1] /
                    ( magnitudes[1] + magnitudes[2] );
                NBL_INFO("case 2: fraction %lf", f)
                frame_start = (2 - f) * SIMPLE_FSK_WINDOW_SIZE;

                start = &f1;
                end = &f2;

                break;
                
            default:
                break;
        }

        NBL_INFO("calculated frame_start is at %d", frame_start)

        NBL_ASSERT_GE(frame_start, 0)
        NBL_ASSERT_LE(frame_start, 2 * SIMPLE_FSK_WINDOW_SIZE)

        NBL_ASSERT(start && end);

        frame_offset = frame_start % SIMPLE_FSK_WINDOW_SIZE;

        NBL_WARN("frame offset is %d samples.", frame_offset)

        /* need to run detection on these two frames... */
        do_parse(*start, *end, conf);
    }

    static inline void combine(SampleBuffer& dest, SampleBuffer& one, SampleBuffer& two, size_t offset) {
        size_t from_one = SIMPLE_FSK_WINDOW_SIZE - offset;
        size_t from_two = SIMPLE_FSK_WINDOW_SIZE - from_one;

        memcpy(dest.buffer, one.buffer + offset, from_one);
        memcpy(dest.buffer + from_one, two.buffer, from_two);
    }

    int closest(int point, std::vector<int> points, std::vector<int> keys) {
        if (point <= points[0]) return keys[0];
        if (point >= points[points.size() - 1]) return keys[points.size() - 1];

        int up = 0, down = 0;
        //must be in between
        for (int i = 0; i < points.size(); ++i) {
            if (points[i] <= point && points[i + 1] >= point) {
                NBL_WARN("%d <= %d <= %d",
                         points[i], point, points[i + 1])
                down = i; up = i + 1;
            }
        }

        int dist_down = point - points[down];
        int dist_up = points[up] - point;

        if (dist_down < dist_up) {
            NBL_WARN("down [%d %d]", dist_down, dist_up)
            return keys[down];
        } else if (dist_up < dist_down) {
            NBL_WARN("up [%d %d]", dist_down, dist_up)
            return keys[up];
        } else {
            //round down.
            NBL_WARN("equal [%d %d]", dist_down, dist_up)
            return keys[down];
        }
    }

    std::vector<int> KEYS = {0, -1, 1};

    int SimpleFSKRecvr::do_parse(nbsound::SampleBuffer& start, nbsound::SampleBuffer& end, nbsound::Config& conf) {

        SampleBuffer full{1, start.frames};
        combine(full, start, end, frame_offset);

        detect_data.transform(full, 0);

        int     mi = 0;
        float   mf = 0;

        for (int i = 0; i < detect_data.get_freq_len(); ++i) {
            if (detect_data.get(i) > mf) {
                mi = i;
                mf = detect_data.get(i);
            }
        }

        NBL_ASSERT(mf > 0);

        int bin0 = detect_data.bin_for_frequency(SIMPLE_FSK_0_F, conf.sample_rate);
        int bin1 = detect_data.bin_for_frequency(SIMPLE_FSK_1_F, conf.sample_rate);
        int binD = detect_data.bin_for_frequency(SIMPLE_FSK_START_F, conf.sample_rate);

        printf(" [%4d %4d %4d] %d\n",
               bin0, binD, bin1, mi);

        std::vector<int> points = {bin0, binD, bin1};
        int ret = closest(mi, points, KEYS);
        return ret;
    }

    std::string SimpleFSKRecvr::data_so_far() {
        std::string buffer;

        int offset = 0;

        while ( (data.size() - offset) >= 8 ) {
            uint8_t byte = 0;
            for (int i = 0; i < 8; ++i) {
                byte |= data[offset + i] << i;
            }

            buffer.push_back(byte);
            offset += 8;
        }

        if (offset != data.size()) {
            NBL_ERROR("throwing away %d bits of unmatched data!",
                      data.size() - offset);
        }
        return buffer;
    }
    
}










