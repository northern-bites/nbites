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
        NBL_DISALLOW_COPY(SimpleFSKSendr)

    public:

        SimpleFSKSendr() :
            SendrBase()
        {  }

        void fill(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

    };

    class SimpleFSKRecvr : public RecvrBase {
        NBL_DISALLOW_COPY(SimpleFSKRecvr)

        double

        size_t iteration;
        size_t signal_start;
        size_t frame_offset;

        nbsound::SampleBuffer lastFrame;

    public:

        SimpleFSKRecvr(Callback cb) :
            RecvrBase(cb),
            lastFrame(1, SIMPLE_FSK_WINDOW_SIZE)
        {
            
        }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

    };

}

#endif /* SimpleFSK_hpp */
