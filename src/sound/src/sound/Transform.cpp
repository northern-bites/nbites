//
//  Transform.cpp
//  xcode-sound2
//
//  Created by Philip Koch on 7/21/15.
//

#include <complex>
#include <iostream>
#include "Transform.hpp"

namespace nbsound {

    Transform::Transform(int frames)
    {
        NBL_ASSERT(frames > 0)

        //Must be power of 2
        NBL_ASSERT( (frames & (frames - 1)) == 0)

        frequency_length = SPECTRUM_LENGTH(frames);

        outputmag = (float *) malloc(sizeof(float) * frequency_length);

        _the_input = (float *) fftwf_malloc(sizeof(float) * frames);

        _the_output = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * frequency_length );
        _the_plan = fftwf_plan_dft_r2c_1d(frequency_length, _the_input, _the_output, FFTW_MEASURE);
        
        printf("transform info {%d frequency bins}:\n", frequency_length);
        fftwf_print_plan(_the_plan); printf("\n");
    }
    
    Transform::~Transform() {
        free(outputmag);

        fftwf_free(_the_input);
        fftwf_free(_the_output);
        fftwf_destroy_plan(_the_plan);
    }

    std::string Transform::get_all() {
        NBL_ASSERT(outputmag);
        return std::string{ (const char *) outputmag, sizeof(float) * get_freq_len() };
    }

    float makeFloat(int16_t value, ssize_t max) {
        //-1 because abs of min(int16_t) == 32768 (not 32767)
        NBL_ASSERT_LE( std::abs(value) - 1 , max)
        return ((float) value) / (max - 1);
    }
    
    void Transform::transform(SampleBuffer& buffer, int channel) {
        NBL_ASSERT(SPECTRUM_LENGTH(buffer.frames) == frequency_length);

        SampleBuffer::BufferChannel chnl = buffer[channel];
        ssize_t sample_max = buffer.sample_max();

        for (int i = 0; i < buffer.frames; ++i) {
            _the_input[i] = makeFloat(chnl[i], sample_max);
        }

        memset(_the_output, 0, sizeof(fftwf_complex) * frequency_length);

        fftwf_execute(_the_plan);
        
        for( int i = 0; i < frequency_length; ++i ) {
            std::complex<float>* cptr = reinterpret_cast<std::complex<float>* >(_the_output + i);
            outputmag[i] = std::abs(*cptr);
        }
        
    }

}
