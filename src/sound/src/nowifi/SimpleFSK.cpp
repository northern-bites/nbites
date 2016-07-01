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
    const int SIMPLE_FSK_START_F = 16000;
    const int SIMPLE_FSK_0_F = 15000;
    const int SIMPLE_FSK_1_F = 17000;

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
                } else {
                    running_start_mag += start_magnitude;
                }
            } break;

            case SEARCHING: {
                do_search(frame_0, frame_1, frame_2, conf);
                current = RECORDING;
            } break;

            case RECORDING: {

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
        SampleBufferArray array;

        array.add(f0);
        array.add(f1);
        array.add(f2);

        double mag_max = 0;
        int i_max = 0;

        for (int i = 0; i < (2 * SIMPLE_FSK_WINDOW_SIZE); ++i) {
            detect_start.correlate(array, conf, f0.sample_max(), i, SIMPLE_FSK_WINDOW_SIZE / 16);

            double value = detect_start.bin.magnitude();

            if (value > mag_max) {
                mag_max = value;
                i_max = i;
            }
        }

        NBL_WARN("max correlation at %d ( %lf )", i_max, mag_max);

        size_t rel1 = i_max / SIMPLE_FSK_WINDOW_SIZE;

        NBL_WARN("max frame offset is %zd", rel1);

        frame_offset = ( i_max + (SIMPLE_FSK_WINDOW_SIZE / 2) ) %
            SIMPLE_FSK_WINDOW_SIZE ;

        NBL_WARN("frame_offset is %zd", frame_offset);

        switch (rel1) {
            case 0:
                NBL_ERROR("max is in frame 0, do_search() should have been called earlier!")
                break;
            case 1:

                break;
            case 2:

                break;
                
            default:
                break;
        }

        exit(1);
    }
    
}

using namespace nowifi;

NBL_ADD_TEST_TO(simple_fsk_send, fsk) {
    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 9};

    uint8_t alt = 0x55;
    std::string data((char *) &alt, 1);

    SimpleFSKSendr test;
    test.send(data);

    int cycle = 0;
    while (test.isSending()) {
        SampleBuffer window = full_buffer.window(cycle * SIMPLE_FSK_WINDOW_SIZE, (cycle + 1) * SIMPLE_FSK_WINDOW_SIZE);

        test.fill(window, config);
    }

    return true;
}

NBL_ADD_TEST_TO(corr_and_transform, fsk) {
    Config config{48000, 8192};
    SampleBuffer buffer{1, 8192};

    nowifi::CorrSender corr(nowifi::SIMPLE_FSK_START_F);

    corr.fill(buffer, config);

    Transform transform(8192);
    transform.transform(buffer, 0);

    float expected = transform.bin_for_frequency(SIMPLE_FSK_START_F, config.sample_rate);

    printf("freq len %d\n", transform.get_freq_len());
    printf("bin expected at %f\n",
           expected);

    int max = 0;
    float fmax = 0;
    for (int i = 0; i < transform.get_freq_len(); ++i) {
        if (transform.get(i) > fmax) {
            max = i;
            fmax = transform.get(i);
        }
    }

    printf("max %f @ %d, expected %f\n",
           fmax, max, expected);

    NBL_ASSERT(fabs(max - expected) < 1.0);


    return true;
}

NBL_ADD_TEST_TO(transform, fsk) {
    Config config{48000, 4096};
    SampleBuffer buffer{1, 4096};
    nowifi::CorrSender sender;

    Transform * transform = new Transform(4096);
    sender.fill(buffer, config);

    transform->transform(buffer, 0);

//    for (int i = 0; i < transform->get_freq_len(); ++i) {
//        printf("%d %f\n", i, transform->get(i));
//    }

    printf("bins %lf %lf\n",
           transform->bin_for_frequency(8192, 48000),
           transform->bin_for_frequency(19000, 48000) );

    printf("freqs %lf %lf\n",
           transform->frequency_at(350, 48000),
           transform->frequency_at(811, 48000) );

    delete transform;

    return true;
}

NBL_ADD_TEST_TO(sfsk_recv, fsk) {

    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 9};
    memset(full_buffer.buffer, 0, full_buffer.size_bytes());
    NBL_INFO("zero fill done");

    uint8_t alt = 0x55;
    std::string data((char *) &alt, 1);

    SimpleFSKSendr test;
    size_t offset = SIMPLE_FSK_WINDOW_SIZE / 2;
    test.send(data);

    SampleBuffer window = full_buffer.window(SIMPLE_FSK_WINDOW_SIZE * 4 + offset, SIMPLE_FSK_WINDOW_SIZE * 5 + offset);
    test.fill(window, config);

    for (int i = 0; i < full_buffer.size_samples(); ++i) {
        if (full_buffer[0][i]) {
            printf("start at %d\n", i);
            break;
        }
    }

    for (int i = full_buffer.size_samples(); i > 0; --i) {
        if (full_buffer[0][i]) {
            printf("end at %d\n", i);
            break;
        }
    }

    NBL_INFO("offset fill done");
    SimpleFSKRecvr recvr(nullptr);
    recvr.NUM_FRAMES_TO_CALIBRATE = 2;
//    NBL_INFO("after constructor");

    for (int i = 0; i < 9; ++i) {
//        NBL_INFO("loop %d", i);
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);

        recvr.parse(window, config);
    }
        
    return true;
}














