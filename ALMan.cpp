


#include "ALMan.h"



ALMan::ALMan(boost::shared_ptr<Sensors> _sensors,
             boost::shared_ptr<Transcriber> _transcriber,
             boost::shared_ptr<ALImageTranscriber> _imageTranscriber,
             boost::shared_ptr<ALEnactor> _enactor,
             boost::shared_ptr<RoboGuardian> _rbg,
             boost::shared_ptr<Synchro> synchro)
    :Man(_sensors,_transcriber,_imageTranscriber,_enactor,_rbg,synchro){}

ALMan::~ALMan(){

}



void ALMan::startSubThreads(){
#ifndef USE_DCM
    if(enactor->start()!=0)
        cout << "Failed to start enactor" <<endl;
    else
        enactor->getTrigger()->await_on();
#endif

    Man::startSubThreads();

    if(guardian->start() != 0)
        cout << "RoboGuardian failed to start" << endl;
    else
        guardian->getTrigger()->await_on();

    // Start Image transcriber thread (it handles its own threading
    if (imageTranscriber->start() != 0) {
        cerr << "Image transcriber failed to start" << endl;
    }
    else
        imageTranscriber->getTrigger()->await_on();

}

void ALMan::stopSubThreads(){
#ifdef DEBUG_MAN_THREADING
    cout << "  ALMan stoping:" << endl;
#endif

    imageTranscriber->stop();
    imageTranscriber->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Image Transcriber thread is stopped" << endl;
#endif

    guardian->stop();
    guardian->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Guardian thread is stopped" << endl;
#endif

    Man::stopSubThreads();

#ifndef USE_DCM
    enactor->stop();
    enactor->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Enactor thread is stopped" << endl;
#endif
#endif


}
