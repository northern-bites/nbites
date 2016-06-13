//
//  Transform.h
//  xcode-sound2
//
//  Created by Philip Koch on 7/21/15.
//

#ifndef __xcode_sound2__Transform__
#define __xcode_sound2__Transform__

#include <stdio.h>
#include "Sound.h"

#include "Transform.h"
#ifndef __APPLE__
#include <fftw3.h>
#else
#include "fftw3.h"
#endif

namespace nbsound {
    class Transform {
    public:
        Transform(parameter_t sound_param);
        void transform(void * data, int channel);
        int get_freq_len() {return frequency_length;}
        ~Transform();
        
        float * outputmag;
                
    private:
        parameter_t params;
        int frequency_length;
        
        float * inputd;
        
        fftwf_complex * output;
        fftwf_plan transform_plan;
    };
}


#endif /* defined(__xcode_sound2__Transform__) */