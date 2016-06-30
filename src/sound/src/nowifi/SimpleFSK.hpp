//
//  SimpleFSK.hpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#ifndef SimpleFSK_hpp
#define SimpleFSK_hpp

#include <stdio.h>
#include <vector>

#include "NoWifi.hpp"
#include "SimpleFSK.hpp"

namespace nowifi {

    extern const int SIMPLE_FSK_WINDOW_SIZE;
    extern const int SIMPLE_FSK_START_F;
    extern const int SIMPLE_FSK_0_F;
    extern const int SIMPLE_FSK_1_F;

    class SimpleFSKSendr : public SendrBase {
        size_t iteration;

    public:

        SimpleFSKSendr() :
            SendrBase()
        {
            iteration = 0;
        }

        void fill(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

        NBL_DISALLOW_COPY(SimpleFSKSendr)
    };

    class SimpleFSKRecvr : public RecvrBase {
        size_t iteration;
        size_t signal_start;
        size_t frame_offset;

    public:

        SimpleFSKRecvr(Callback cb) :
            RecvrBase(cb)
        {

        }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

        NBL_DISALLOW_COPY(SimpleFSKRecvr)
    };

}

#endif /* SimpleFSK_hpp */
