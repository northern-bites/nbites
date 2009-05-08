


#include "ALMan.h"



ALMan::ALMan(boost::shared_ptr<Sensors> _sensors,
             boost::shared_ptr<Transcriber> _transcriber,
             boost::shared_ptr<ALImageTranscriber> _imageTranscriber,
             boost::shared_ptr<ALEnactor> _enactor,
             boost::shared_ptr<RoboGuardian> _rbg,
             boost::shared_ptr<Synchro> synchro
             ,AL::ALPtr<AL::ALBroker> broker)
    :Man(_sensors,_transcriber,_imageTranscriber,_enactor,_rbg,synchro){}

ALMan::~ALMan(){

}



void ALMan::startSubThreads(){


}

void ALMan::stopSubThreads(){

}
