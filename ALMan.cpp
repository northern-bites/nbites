


#include "ALMan.h"



ALMan::ALMan(boost::shared_ptr<Sensors> _sensors,
             boost::shared_ptr<Transcriber> _transcriber,
             boost::shared_ptr<ALImageTranscriber> _imageTranscriber,
             boost::shared_ptr<ALEnactor> _enactor,
             boost::shared_ptr<Synchro> synchro)
    :Man(_sensors,_transcriber,_imageTranscriber,_enactor,synchro),
     alImageTranscriber(_imageTranscriber),
     alEnactor(_enactor)
{

}

ALMan::~ALMan(){

}



void ALMan::startSubThreads(){
#ifndef USE_DCM
    if(alEnactor->start()!=0)
        cout << "Failed to start enactor" <<endl;
    else
        alEnactor->getTrigger()->await_on();
#endif

    Man::startSubThreads();

    // Start Image transcriber thread (it handles its own threading
    if (alImageTranscriber->start() != 0) {
        cerr << "Image transcriber failed to start" << endl;
    }
    else
        alImageTranscriber->getTrigger()->await_on();

}

void ALMan::stopSubThreads(){
#ifdef DEBUG_MAN_THREADING
    cout << "  ALMan stoping:" << endl;
#endif

    alImageTranscriber->stop();
    alImageTranscriber->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Image Transcriber thread is stopped" << endl;
#endif

    Man::stopSubThreads();

#ifndef USE_DCM
    alEnactor->stop();
    alEnactor->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Enactor thread is stopped" << endl;
#endif
#endif


}
