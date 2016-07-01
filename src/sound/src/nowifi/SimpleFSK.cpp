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

    void SimpleFSKSendr::fill(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {
        if (iteration == 0) {
            printf("iteration 0, FIRST FRAME");
        } else {
            size_t bit = (iteration / SIMPLE_FSK_WINDOW_SIZE) - 1;
            size_t byte = bit / 8;
            printf("data frame %zd");
        }
    }

    void SimpleFSKRecvr::parse(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

    }
    
}

using namespace nowifi;

NBL_ADD_TEST_TO(corr_and_transform, fsk) {

    Config config{48000, 8192};
    SampleBuffer buffer{1, 8192};

    nowifi::CorrSender corr(nowifi::SIMPLE_FSK_START_F);

    corr.fill(buffer, config);

    Transform transform(8192);
    transform.transform(buffer, 0);

    float expected = transform.bin_for_frequency(SIMPLE_FSK_START_F, config.sample_rate);

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