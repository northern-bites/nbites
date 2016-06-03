//
//  Transform.cpp
//  xcode-sound2
//
//  Created by Philip Koch on 7/21/15.
//

#include <complex>
#include <iostream>
#include "Transform.h"

namespace nbsound {

    Transform::Transform(parameter_t sp)
        : params(sp)
    {
        if (sp.format != NBS_S16_LE) {
            printf("Transform not yet implemented for non - NBS_S16_LE!\n");
            exit(1);
        }
        
        //+1 in case odd number of frames.
        int input_length = sp.frames;
        frequency_length = input_length / 2 + 1;
        
        inputd = (float *) fftwf_malloc(sizeof(float) * input_length);
        output = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * frequency_length );
        
        outputmag = (float *) malloc(sizeof(float) * frequency_length);
        
        transform_plan = fftwf_plan_dft_r2c_1d(input_length, inputd, output, FFTW_MEASURE);
        
        printf("transform info:\n");
        fftwf_print_plan(transform_plan);
        printf("\nfrequency length: %i\n", frequency_length);
    }
    
    Transform::~Transform() {
        free(outputmag);

        fftwf_free(inputd);
        fftwf_free(output);
        
        fftwf_destroy_plan(transform_plan);
    }
    
    float floatFrom(int index, int channel, int nchan, void * data) {
        int16_t val = ((int16_t *) data)[index * nchan + channel];
        return ((float) val) / (INT16_MAX - 1);
    }
    
    void Transform::transform(void * data, int channel) {

        for (int i = 0; i < params.frames; ++i) {
            inputd[i] = floatFrom(i, channel, params.channels, data);
        }
        
        fftwf_execute(transform_plan);
        
        for( int i = 0; i < frequency_length; ++i ) {
            std::complex<float>* cptr = reinterpret_cast<std::complex<float>* >(output + i);
            outputmag[i] = std::abs(*cptr);
        }
        
    }
    
}
