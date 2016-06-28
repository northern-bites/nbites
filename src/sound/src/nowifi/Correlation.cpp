//
//  Correlation.cpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#include "Correlation.hpp"
#include <cmath>

namespace nowifi {

    

    void CorrSender::fill(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

        double samples_per_cycle = conf.sample_rate / CORRELATION_FREQUENCY;

        for (int i = 0; i < buffer.frames; ++i) {
            double fraction = std::fmod( (double) iteration++, samples_per_cycle ) / samples_per_cycle;

            double theta = 2 * M_PI * fraction;

            buffer[0][i] = (int16_t)(buffer.sample_max() * std::sin(theta));
        }
    }

    void CorrRecvr::parse(nbsound::SampleBuffer &buffer, nbsound::Config &conf) {

        double samples_per_cycle = conf.sample_rate / CORRELATION_FREQUENCY;



        for (int i = 0; i < buffer.frames; ++i) {
            double fraction = std::fmod( (double) iteration++, samples_per_cycle ) / samples_per_cycle;
            double theta = 2 * M_PI * fraction;

            buffer[0][i] = (int16_t)(buffer.sample_max() * std::sin(theta));
        }
    }
    
}