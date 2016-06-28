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

    return true;
}