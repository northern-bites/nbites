#ifndef _PySpeech_h_DEFINED
#define _PySpeech_h_DEFINED

#include <boost/shared_ptr.hpp>

class Speech;

void c_init_speech();
// C++ backend insertion (must be set before import)
//    steals a reference to the supplied Lights instance
void set_speech_pointer(boost::shared_ptr<Speech> speech_ptr);


#endif /* _PySpeech_h_DEFINED */
