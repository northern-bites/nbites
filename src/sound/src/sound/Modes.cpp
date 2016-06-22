#ifdef USE_CORRELATE

#include "Modes.hpp"

#include "Sound.hpp"

#include <set>

namespace nbsound {

    struct configuration sliding48khz{48000, 1000, 8000, 1000};
    struct configuration sliding96khz{96000, 1000, 16000, 1000};

    struct configuration mono48khz_16khz{48000, 8000, 8000, 0};
    struct configuration mono96khz_24khz{96000, 16000, 16000, 0};

    int nbs_config_t::optimal_amplitude() {
        return std::numeric_limits<nbs_sample_t>::max() / 2;
    }

    namespace mode {
        ProcessType processType = Listener;
        struct configuration * config = &sliding96khz;

//        Signal * slidingSignalFromConfig() {
//            return new SlidingSignal(sampleRate, slidingBaseFreq, slidingWindFreq,
//                                 std::numeric_limits<int16_t>::max() / 2, targetFreqHertz);
//        }
//
//        Signal * monotoneSignalFromConfig() {
//            return new MonotoneSignal(sampleRate, targetFreqHertz,
//                                      std::numeric_limits<int16_t>::max() / 2);
//        }

        void printConfig() {
            printf("nbsound process configuration\n");
            printf("\t%s\n",
                   ProcessTypeStrings[processType]);
            NBL_ASSERT(config != nullptr);
            printf("\t%s\n", config->toString().c_str());
            if (config->monotone()) printf("\t(monotone)\n");
            else printf("\t(sliding)\n");
        }
    }

    NBL_ADD_TEST_TO(modes_static, modes) {
        NBL_ASSERT_THROW_OF(std::runtime_error, nyquist_check(1500, 800), "nyquist check failed")

        return true;
    }

//    NBL_ADD_TEST_TO(monotone, modes) {
//
//        NBL_ASSERT( dequal(sin(0), sin(TAU)) )
//        NBL_ASSERT( dequal(sin(M_PI), sin(TAU+M_PI)) )
//
//        MonotoneSignal signal{2, 1, 1.0};
//
//        NBL_ASSERT(dequal(signal.thetaForFrame(0), 0.0))
//        NBL_ASSERT(dequal(signal.thetaForFrame(1), M_PI))
//        NBL_ASSERT(dequal(signal.thetaForFrame(2), TAU))
//
//        NBL_ASSERT( dequal(signal.amplitudeForFrame(1), signal.amplitudeForFrame(3)) )
//        NBL_ASSERT( dequal(signal.amplitudeForFrame(2), signal.amplitudeForFrame(4)) )
//
//        MonotoneSignal signal2{4, 1, 1.0};
//
//        NBL_ASSERT( dequal(signal2.thetaForFrame(0), 0.0)       )
//        NBL_ASSERT( dequal(signal2.thetaForFrame(1), M_PI / 2)  )
//        NBL_ASSERT( dequal(signal2.thetaForFrame(2), M_PI)      )
//
//        return true;
//    }

    bool dlt(const double& a, const double& b) {
        return a < b && !(dequal(a,b));
    }

    struct dlt_wrap {
        bool operator() (const double& a, const double& b) const {
            return dlt(a, b);
        }
    };

    NBL_ADD_TEST_TO(sliding, modes) {
        //(double r, double bf, double wf, double amp, double tf)
//        SlidingSignal signal{48000, 16000, 50, 1, 20};

//        printf("%d frames per cycle\n", framesPerCycle(48000, 20));
//
//        std::set<double, dlt_wrap> sample_set{};
//
//        for (int frame = 0; true; ++frame) {
//            double value = signal.amplitudeForFrame(frame);
//            if (sample_set.count(value)) {
//                printf("%d found %f\n", frame, value);
//            }
//
//            sample_set.insert(value);
//        }
//
//        for (int i = 0; i < 200; ++i) {
//            printf("%d\t: v %f f %f\n",
//                   i, signal.amplitudeForFrame(i), signal.thetaForFrame(i));
//        }

        return true;
    }
    
}

#endif