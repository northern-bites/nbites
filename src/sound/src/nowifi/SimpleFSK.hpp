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

    NBL_MAKE_ENUM_FULL(Mode,
                       CALIBRATING,
                       LISTENING,
                       SEARCHING,
                       RECORDING    );

    class SimpleFSKRecvr : public RecvrBase {
        NBL_DISALLOW_COPY(SimpleFSKRecvr)

        double running_start_mag;

        Mode current;

        size_t iteration;
        size_t signal_start;
        size_t frame_offset;

        nbsound::SampleBuffer frame_0;
        nbsound::SampleBuffer frame_1;
        /*'frame_2' is argument to parse*/

    public:

        int NUM_FRAMES_TO_CALIBRATE;

        SimpleFSKRecvr(Callback cb) :
            RecvrBase(cb),
            frame_0(1, SIMPLE_FSK_WINDOW_SIZE),
            frame_1(1, SIMPLE_FSK_WINDOW_SIZE),
            current(CALIBRATING),
            iteration(0),
            signal_start(0),
            frame_offset(0),
            running_start_mag(0)
        {
            NUM_FRAMES_TO_CALIBRATE = 30;
        }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

        void do_search( nbsound::SampleBuffer& f0, nbsound::SampleBuffer&f1, nbsound::SampleBuffer& f2, nbsound::Config& conf );
    };

}

#endif /* SimpleFSK_hpp */
