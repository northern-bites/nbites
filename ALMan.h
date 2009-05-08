#ifndef _ALMan_h
#define _ALMan_h

#include "albroker.h"

#include "Man.h"

class ALMan : public Man {
public:

    ALMan(boost::shared_ptr<Sensors> _sensors,
         boost::shared_ptr<Transcriber> _transcriber,
        boost::shared_ptr<ALImageTranscriber> _imageTranscriber,
         boost::shared_ptr<ALEnactor> _enactor,
        boost::shared_ptr<Synchro> synchro);

    virtual ~ALMan();

    virtual void startSubThreads();

    virtual void stopSubThreads();

private:
    boost::shared_ptr<Sensors> sensors;

};

#endif
