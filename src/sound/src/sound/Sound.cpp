#include "Sound.hpp"

namespace nbsound {
    
    void * thread_function(void * _handler) {
        printf("... thread starting up ...\n");
        Handler * handler = (Handler *) _handler;
        
        handler->main();
        
        return NULL;
    }
    
    void Handler::start_new_thread(pthread_t& thread, pthread_attr_t * attr) {
        pthread_create(&thread, attr, thread_function, this);
        pthread_detach(thread);
    }

    void alsa_print_info(snd_pcm_t * handle, snd_pcm_hw_params_t * hwp) {
        printf("device [%s] opened with\n",
               snd_pcm_name(handle));
        printf("\tstate=%s\n",
               snd_pcm_state_name(snd_pcm_state(handle)));
        unsigned int val, val2;
        snd_pcm_hw_params_get_access(hwp,
                                     (snd_pcm_access_t *) &val);
        printf("\taccess_type=%s\n",
               snd_pcm_access_name((snd_pcm_access_t)val));

        snd_pcm_hw_params_get_format(hwp, (snd_pcm_format_t *) &val);
        printf("\tformat=%s\n",
               snd_pcm_format_name((snd_pcm_format_t) val)
               );

        snd_pcm_hw_params_get_channels(hwp, &val);
        printf("\tchannels=%d\n", val);

        snd_pcm_hw_params_get_rate(hwp, &val, (int *) &val2);
        printf("\trate=%d fps\n", val);

        snd_pcm_hw_params_get_period_time(hwp,
                                          &val, (int *) &val2);
        printf("\tperiod_time=%d us\n", val);

        snd_pcm_uframes_t frames;
        snd_pcm_hw_params_get_period_size(hwp,
                                          &frames, (int *) &val2);
        printf("\tperiod_size=%d frames\n", (int)frames);

        snd_pcm_hw_params_get_buffer_size(hwp,
                                          (snd_pcm_uframes_t *) &val);
        printf("\tbuffer_size=%d frames\n", val);

        snd_pcm_hw_params_get_periods(hwp, &val, (int *) &val2);
        printf("\tperiods_per_buffer=%d periods\n", val);
    }
}