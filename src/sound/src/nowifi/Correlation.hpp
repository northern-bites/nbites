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
        int frequency;

    public:
        CorrSender() :
            SendrBase()
        {
            frequency = CORRELATION_FREQUENCY;
            iteration = 0;
        }

        CorrSender(int f) :
            SendrBase()
        {
            frequency = f;
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
        int frequency;
        
    public:

        CorrelationBin bin;

        CorrRecvr(Callback cb) :
            RecvrBase(cb)
        {
            frequency = CORRELATION_FREQUENCY;
        }

        CorrRecvr(Callback cb, int f) :
            RecvrBase(cb)
        {
            frequency = f;
        }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

//        void correlate(nbsound::SampleBufferArray& array, nbsound::Config& conf, size_t smax, size_t offset, size_t length);
    };
}

#endif /* Correlation_hpp */
