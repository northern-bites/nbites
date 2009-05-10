#ifndef _ALMan_h
#define _ALMan_h

#include "albroker.h"

#include "Man.h"



#include "ALImageTranscriber.h"
#include "ALTranscriber.h"


#ifdef USE_DCM
#  if defined USE_DCM && defined MAN_IS_REMOTE
#    error "DCM not compatible with remote!!!"
#  endif

#include "NaoEnactor.h"
typedef NaoEnactor EnactorT;
#else
#include "ALEnactor.h"
typedef ALEnactor EnactorT;
#endif

class ALMan : public Man {
public:

    ALMan(boost::shared_ptr<Sensors> _sensors,
         boost::shared_ptr<ALTranscriber> _transcriber,
        boost::shared_ptr<ALImageTranscriber> _imageTranscriber,
         boost::shared_ptr<EnactorT> _enactor,
        boost::shared_ptr<Synchro> synchro);

    virtual ~ALMan();

    virtual void startSubThreads();

    virtual void stopSubThreads();

private:
    boost::shared_ptr<ALImageTranscriber> alImageTranscriber;
    boost::shared_ptr<EnactorT> alEnactor;

};

#endif
