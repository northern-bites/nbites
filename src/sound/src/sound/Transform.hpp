//
//  Transform.h
//  xcode-sound2
//
//  Created by Philip Koch on 7/21/15.
//

#ifndef __xcode_sound2__Transform__
#define __xcode_sound2__Transform__

#include <stdio.h>
#include "Sound.hpp"

#include "Transform.hpp"

#include <fftw3.h>

#define SPECTRUM_LENGTH( nsamp ) ((int) ((nsamp / 2) + 1))

namespace nbsound {
    class Transform {
    public:
        Transform(int frames);
        void transform(SampleBuffer& buffer, int channel);

        int get_freq_len() {return frequency_length;}
        float get(int f) { return outputmag[f]; }
        float* get_freq_buffer() { return outputmag; }

        std::string get_all();

        ~Transform();

    private:
        int frequency_length;
        float * outputmag;

        float * _the_input;
        fftwf_complex * _the_output;
        fftwf_plan _the_plan;
    };
}

#endif /* defined(__xcode_sound2__Transform__) */
