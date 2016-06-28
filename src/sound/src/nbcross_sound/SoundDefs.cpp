#include "nbcross.hpp"
#include "../sound/Sound.hpp"
#include "../sound/Detect.hpp"

#ifndef DETECT_LOG_RESULTS
#error "need DETECT_LOG_RESULTS"
#endif

using namespace nbsound;
using namespace nbl;

NBCROSS_FUNCTION(frequency_domain, true )
    (const std::vector<nbl::logptr> &arguments) {

}

NBCROSS_FUNCTION(whistle_detect, true )
    (const std::vector<nbl::logptr> &arguments) {
        NBL_ASSERT_GT(arguments.size(), 0)
        NBL_INFO("whistle_detect");

        nbl::logptr ptr = arguments[0];
        nbl::Block& block = ptr->blocks[0];

        static const int bytes_per_frame = 2 * 2;   //2 channels, 2 bytes per sample
        int num_frames = block.data.size() / bytes_per_frame;

        SampleBuffer buffer{2, num_frames};
        NBL_ASSERT_EQ(buffer.size_bytes(), block.data.size())
        memcpy(buffer.buffer, block.data.data(), block.data.size());

        detect::reset();

        NBL_INFO("before detect...")
        bool heard = detect::detect(buffer, true);
        
        NBL_INFO("after detect...")
        NBL_INFO("results...")
        for (Block& blck : detect_results) {
            NBL_INFO("\tblock: %s", blck.type.c_str())
        }
        NBL_INFO("after iter...")

        logptr log = Log::explicitLog(detect_results, json::Object{}, "DetectResults");

        log->topLevelDictionary["WhistleHeard"] = json::Boolean(heard);

        RETURN(log);
}