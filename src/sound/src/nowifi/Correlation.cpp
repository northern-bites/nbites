//
//  Correlation.cpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#include "Correlation.hpp"
#include <cmath>

#include "utilities-test.hpp"

using nbsound::SampleBuffer;

namespace nowifi {

    

    void CorrSender::fill(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

        double samples_per_cycle =
            (double) conf.sample_rate / (double) frequency;

//        printf("samples per cycle %lf\n", samples_per_cycle);

        for (int i = 0; i < buffer.frames; ++i) {
//            double index = iteration++;
//            double remainder = index / samples_per_cycle;
////            printf("rem %lf\n", remainder);
//            remainder = remainder - ((int) remainder);
//
////            printf("rem %lf\n", remainder);

            double fraction = std::fmod( (double) iteration++, samples_per_cycle ) / samples_per_cycle;

            double theta = 2 * M_PI * fraction;

            buffer[0][i] = (int16_t)(buffer.sample_max() * std::sin(theta));

            for (int c = 1; c < buffer.channels; ++c) {
                buffer[c][i] = 0;
            }

//            printf("%lf %d\n", theta, buffer[0][i]);
        }
    }

    void CorrRecvr::parse(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

        bin.tcos = bin.tsin = 0;

        double samples_per_cycle =
            (double) conf.sample_rate / (double) frequency;

        for (int i = 0; i < buffer.frames; ++i) {
            double fraction = std::fmod( (double) i, samples_per_cycle ) / samples_per_cycle;

            double theta = 2 * M_PI * fraction + M_PI;

            double esin = (buffer.sample_max() * std::sin(theta));
            double ecos = (buffer.sample_max() * std::cos(theta));

            bin.tsin += esin * buffer[0][i];
            bin.tcos += ecos * buffer[0][i];
        }

        printf("mag %lf off %lf\n", bin.magnitude(), bin.offset());
    }

    NBL_ADD_TEST_TO(corr_in_out, correlation) {

        nbsound::Config config{48000, 16384};
        nbsound::SampleBuffer buffer{1, 16384};

        for (int i = 0; i < buffer.frames; ++i) {
            buffer[0][i] = (buffer.sample_max() / 2);
        }

        CorrRecvr recvr{ nullptr };
        CorrSender sendr{ };

        recvr.parse(buffer, config);

        CorrelationBin before = recvr.bin;
        sendr.fill(buffer, config);

        recvr.parse(buffer, config);

        CorrelationBin after = recvr.bin;

        printf("{%lf, %lf} -> {%lf, %lf}\n",
               before.magnitude(), before.offset(),
               after.magnitude(), after.offset() );

        return true;
    }
    
}