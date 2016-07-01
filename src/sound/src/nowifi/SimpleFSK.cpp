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

    void SimpleFSKRecvr::parse(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {
        
        if (iteration > 0) {

        }

        lastFrame.take(buffer);
        ++iteration;
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