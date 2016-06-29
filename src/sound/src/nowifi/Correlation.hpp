//
//  Correlation.hpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#ifndef Correlation_hpp
#define Correlation_hpp

#include <cmath>
#include <stdio.h>
#include "NoWifi.hpp"

namespace nowifi {

    static const int CORRELATION_FREQUENCY = 8192;

    class CorrSender : public SendrBase {
        size_t iteration;

    public:
        CorrSender() :
            SendrBase()
        {
            iteration = 0;
        }

        void fill(nbsound::SampleBuffer& buffer, nbsound::Config& conf);
    };

    struct CorrelationBin {
        double tcos, tsin;

        CorrelationBin() { tcos = tsin = 0; }

        double offset() {
            return std::atan2(tcos, tsin);
        }

        double magnitude() {
            return std::sqrt( (tcos * tcos) + (tsin * tsin) );
        }
    };

    class CorrRecvr : public RecvrBase {
    public:

        CorrelationBin bin;

        CorrRecvr(Callback cb) :
            RecvrBase(cb)
        { }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);
    };
}

#endif /* Correlation_hpp */
