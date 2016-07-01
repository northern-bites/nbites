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

    extern int closest(int point, std::vector<int> points, std::vector<int> keys);

    class SimpleFSKRecvr : public RecvrBase {
        NBL_DISALLOW_COPY(SimpleFSKRecvr)

        double running_start_mag;


        size_t iteration;
        size_t frame_offset;

        std::vector<uint8_t> data;

        nbsound::SampleBuffer frame_0;
        nbsound::SampleBuffer frame_1;
        /*'frame_2' is argument to parse*/

        void do_search( nbsound::SampleBuffer& f0, nbsound::SampleBuffer&f1, nbsound::SampleBuffer& f2, nbsound::Config& conf );

        int do_parse(nbsound::SampleBuffer& start, nbsound::SampleBuffer& end, nbsound::Config& conf);

    public:

        /* for debugging ONLY, not part of normal interface */
        Mode current;
        int NUM_FRAMES_TO_CALIBRATE;

        std::string data_so_far();

        size_t offset() { return frame_offset; }

        SimpleFSKRecvr(Callback cb) :
            RecvrBase(cb),
            frame_0(1, SIMPLE_FSK_WINDOW_SIZE),
            frame_1(1, SIMPLE_FSK_WINDOW_SIZE),
            current(CALIBRATING),
            iteration(0),
            frame_offset(0),
            running_start_mag(0)
        {
            NUM_FRAMES_TO_CALIBRATE = 30;
        }

        void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf);

    };

}

#endif /* SimpleFSK_hpp */
