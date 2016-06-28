//
//  Correlation.hpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#ifndef Correlation_hpp
#define Correlation_hpp

#include <stdio.h>
#include "NoWifi.hpp"

namespace nowifi {

    static const int CORRELATION_FREQUENCY = 19000;

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

    class CorrRecvr : public RecvrBase {
    public:
        CorrRecvr(Callback cb) :
            RecvrBase(cb)
        { }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);
    };
}

#endif /* Correlation_hpp */
