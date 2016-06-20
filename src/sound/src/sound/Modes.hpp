#ifdef USE_CORRELATE

#ifndef _NB_SND_MODES_H_
#define _NB_SND_MODES_H_

#include <string>
#include <algorithm>
#include <stdexcept>
#include <math.h>
#include <cmath>

namespace nbsound {


    typedef struct configuration {
        int sample_rate;
        int target_freq;
        int carrier_base;
        int carrier_wind;

        configuration(int sr, int tg, int cb, int cw) :
            sample_rate(sr), target_freq(tg), carrier_base(cb), carrier_wind(cw)
        {
            if (cw > 0) {
                //sliding
                NBL_ASSERT(sample_rate % carrier_base == 0)
                NBL_ASSERT(carrier_base % target_freq == 0)
            }

            /* signal should repeat every (sample_rate/tf) */
//            callback_buffer_mult = 8192 / (sample_rate / target_freq);
        }

        const std::string toString() const {
            return nbl::utilities::format(
                "configuration:\n\tsr %d\n\ttf %d\n\tcb %d\n\tcw %d",
                sample_rate, target_freq, carrier_base, carrier_wind);
        }

        int optimal_amplitude();

        bool monotone() {
            return carrier_wind == 0;
        }

    } nbs_config_t;

    namespace mode {
        NBL_MAKE_ENUM_FULL(ProcessType, Speaker, Listener)
        extern ProcessType processType;
        extern configuration * config;
        void printConfig();
    }

    extern struct configuration sliding48khz;
    extern struct configuration sliding96khz;

    extern struct configuration mono48khz_16khz;
    extern struct configuration mono96khz_24khz;

    static inline void nyquist_check(double rate, double freq) {
        if ( (rate / 2) < freq) {
            throw std::runtime_error{nbl::utilities::format("rate %f does not satisfy nyquist check (>(2f)) for target frequency %f", rate, freq)};
        }
    }

    static const double TAU = 2 * M_PI;

    static const double EPS = 1e-10;

    static inline bool dequal(double a, double b) {
        return std::abs(a - b) < EPS;
    }

    static inline int framesPerCycle(int rate, int freq) {
        return rate / freq;
    }

    class CorrelateResults {
    public:
        std::vector<std::pair<double,double>> results;
        void push_channel(double amp, double off) {
            results.push_back({amp,off});
        }

        double amplitude(int i) {
            return results[i].first;
        }

        double offset(int i) {
            return results[i].second;
        }
    };

    class Signal {
    public:
        virtual double thetaForFrame(int framei) = 0;
        virtual double amplitudeForFrame(int framei) = 0;

        virtual CorrelateResults correlate(std::vector<nbs_sample_t>& data) = 0;
    };

    class MonotoneSignal : public Signal {
    public:
        double rate, tfhertz, amplitude;
        MonotoneSignal(configuration * conf) :
            rate(conf->sample_rate), tfhertz(conf->target_freq), amplitude(conf->optimal_amplitude())  {
                nyquist_check(rate, tfhertz);
            }

        double thetaForFrame(int framei) {
            return (tfhertz / rate) * framei * TAU;
        }

        double amplitudeForFrame(int framei) {
            return sin(thetaForFrame(framei)) * amplitude;
        }

        CorrelateResults correlate(std::vector<nbs_sample_t>& data) {
            size_t frames = data.size() / 2;
            double tsin[2] = {0,0};
            double tcos[2] = {0,0};

            for (int i = 0; i < frames; ++i) {
                nbs_sample_t left = data[i << 1];
                nbs_sample_t right = data[ (i << 1) + 1];

                double theta = thetaForFrame(i);
                tsin[0] += sin(theta) * left;
                tcos[0] += cos(theta) * left;
                tsin[1] += sin(theta) * right;
                tsin[1] += cos(theta) * right;
            }

            double mag0 = tcos[0] * tcos[0] +
                tsin[0] * tsin[0];

            double mag1 = tcos[1] * tcos[1] +
                tsin[1] * tsin[1];

            double off0 = atan2(tcos[0], tsin[0]);

            double off1 = atan2(tcos[1], tsin[1]);

            CorrelateResults results;
            results.push_channel(mag0, off0);
            results.push_channel(mag1, off1);
            return results;
        }

        const std::string toString() const {
            return nbl::utilities::format("MonotoneSignal(rate=%f, targetf=%f, amp=%f)",
                                          rate, tfhertz, amplitude);
        }

        NBL_OSTREAM_FRIEND(MonotoneSignal, inst, inst.toString())
    };

#define NBL_LOGGING_LEVEL NBL_INFO_LEVEL

//    class SlidingSignal : public Signal {
//    public:
//        double rate, amplitude,
//            base_freq_hertz, window_freq_hertz;
//
//        double target_freq_hertz;
//
//        MonotoneSignal outside;
//        MonotoneSignal inside;
////
////        int dropout_begin;
////        int dropout_end;
//
//        int precalc_size;
//        double * precalc;
//
//        SlidingSignal(double r, double bf, double wf, double amp, double tf) :
//            rate(r), base_freq_hertz(bf), window_freq_hertz(wf), amplitude(amp),
//            target_freq_hertz(tf),
//
//            outside(r, 0, amp),
//            inside(r, tf, wf)
//        {
//            NBL_ASSERT_LT(target_freq_hertz, base_freq_hertz);
//            nyquist_check(rate, base_freq_hertz + window_freq_hertz);
//            nyquist_check(rate, target_freq_hertz);
//
////            dropout_end = 20 * framesPerCycle(r, tf);
////            dropout_begin = dropout_end;
////            NBL_WARN("SlidingSignal{} dropout %%%d frames until %%%d frames",
////                    dropout_begin, dropout_end );
//
//            precalc_size = (int)( framesPerCycle(r, tf) * wf );
//            precalc = new double[precalc_size];
//
//            for (int i = 0; i < precalc_size; ++i) {
//                precalc[i] = internalAmplitude(i);
//            }
//        }
//
//        double thetaForFrame(int framei) {
//            double frame_freq = base_freq_hertz + inside.amplitudeForFrame(framei);
//            outside.tfhertz = frame_freq;
//
//            NBL_ASSERT_GE(frame_freq, (base_freq_hertz - window_freq_hertz));
//            NBL_ASSERT_LE(frame_freq, (base_freq_hertz + window_freq_hertz));
//
////            NBL_INFO("Sliding[%f - %f - %f] @ %f",
////                     base_freq_hertz - window_freq_hertz, base_freq_hertz,
////                     base_freq_hertz + window_freq_hertz, frame_freq)
//
//            return outside.thetaForFrame(framei);
//        }
//
//        double internalAmplitude(int framei) {
//            return sin(thetaForFrame(framei)) * amplitude;
//        }
//
//        bool dropout = false;
//        double amplitudeForFrame(int framei) {
////            if ( framei % dropout_begin == 0 ) dropout = true;
////            if ( framei % dropout_end == 0 ) dropout = false;
////            if ( dropout ) return 0;
//
////            return sin(thetaForFrame(framei)) * amplitude;
//            return precalc[(framei % precalc_size)];
//        }
//
//        const std::string toString() const {
//            return nbl::utilities::format("SlidingSignal(rate=%f, base=%f, window=%f, amp=%d)",
//                                          rate, base_freq_hertz, window_freq_hertz, amplitude);
//        }
//
//        NBL_OSTREAM_FRIEND(SlidingSignal, inst, inst.toString())
//    };

    class SlidingSignal : public Signal {
    private:
        double frac(double fi, double fw) {
            return fi / fw;
        }

    public:
        configuration * conf;
        int amplitude;
        int frame_wrap;
        double * frames;

        int correlate_window;

        SlidingSignal(configuration * toUse) : conf(toUse) {
            NBL_ASSERT_NE(conf, NULL);
            NBL_ASSERT_LT(conf->target_freq, conf->carrier_base);
            nyquist_check(conf->sample_rate, conf->carrier_base + conf->carrier_wind);

            amplitude = conf->optimal_amplitude();

            frame_wrap = conf->sample_rate / conf->target_freq;
            frames = new double[frame_wrap];

            for (int i = 0; i < frame_wrap; ++i) {
                double freq = conf->carrier_base;
                double frc = frac(i, frame_wrap);
                freq += (conf->carrier_wind) * sin( TAU * frc );

                NBL_ASSERT_GE(freq, (conf->carrier_base - conf->carrier_wind));
                NBL_ASSERT_LE(freq, (conf->carrier_base + conf->carrier_wind));

                double frames_per_cycle = (conf->sample_rate / freq);
                double frc2 = (fmod(i, frames_per_cycle) / frames_per_cycle);

                NBL_ASSERT_LE(frc2, 1.0);
                NBL_ASSERT_GE(frc2, 0.0);

                double val = amplitude * sin(TAU * frc2);
                frames[i] = val;
            }

            correlate_window = frame_wrap / 4;
        }

        CorrelateResults correlate(std::vector<nbs_sample_t>& data) {
            int nframes = (int) data.size() / 2;
            int ntop = nframes - correlate_window;
            double topLeft[ntop];
            double topRght[ntop];

            int begin = correlate_window / 2;
            int end = (nframes) - (correlate_window / 2);

            NBL_ASSERT_EQ(end - begin, ntop);

            MonotoneSignal signal(conf);
            signal.tfhertz = conf->carrier_base;

            for (int i = begin; i < end; ++i ) {
                int topi = i - (correlate_window / 2);
                NBL_ASSERT_GE(topi, 0)
                NBL_ASSERT_LT(topi, ntop)

                double tsin[2] = {0,0};
                double tcos[2] = {0,0};

                for (int j = 0; j < correlate_window; ++j) {
                    int actual = i - (correlate_window / 2) + j;

                    NBL_ASSERT_GE(actual, 0)
                    NBL_ASSERT_LT(actual, nframes)

                    nbs_sample_t left = data[actual << 1];
                    nbs_sample_t right = data[ (actual << 1) + 1];

                    double theta = signal.thetaForFrame(j);
                    tsin[0] += sin(theta) * left;
                    tcos[0] += cos(theta) * left;
                    tsin[1] += sin(theta) * right;
                    tsin[1] += cos(theta) * right;
                }

                topLeft[topi] = tcos[0] * tcos[0] +
                    tsin[0] * tsin[0];
                topRght[topi] = tcos[1] * tcos[1] +
                    tsin[1] * tsin[1];
            }

            double tsin[2] = {0,0};
            double tcos[2] = {0,0};
            signal.tfhertz = conf->target_freq;

            for (int i = 0; i < ntop; ++i) {
                double theta = signal.thetaForFrame(i);
                tsin[0] += sin(theta) * topLeft[i];
                tcos[0] += cos(theta) * topLeft[i];
                tsin[1] += sin(theta) * topRght[i];
                tsin[1] += cos(theta) * topRght[i];
            }

            double mag0 = tcos[0] * tcos[0] +
                tsin[0] * tsin[0];

            double mag1 = tcos[1] * tcos[1] +
                tsin[1] * tsin[1];

            double off0 = atan2(tcos[0], tsin[0]);
            double off1 = atan2(tcos[1], tsin[1]);

            CorrelateResults results;
            results.push_channel(mag0, off0);
            results.push_channel(mag1, off1);
            return results;
        }

        double thetaForFrame(int framei) { throw std::runtime_error("not supported"); }
        double amplitudeForFrame(int framei) {
            return frames[framei % frame_wrap];
        }

        void print() const {
            for (int i = 0; i < frame_wrap; ++i) {
                printf("  %d\t%f\n",
                       i, frames[i]);
            }
        }
    };


}

#endif

#endif