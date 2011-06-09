#ifndef _TMan_h
#define _TMan_h

/**
 * This class represents the single extra thread variant of the TMan/TTMan pair.
 * It only manages one thread, which is the imageThread. See also TTMan
 */

#include "Man.h"

#include "ThreadedImageTranscriber.h"


class TMan : public Man {
public:

    TMan(boost::shared_ptr<Profiler> _profiler,
            boost::shared_ptr<Sensors> _sensors,
         boost::shared_ptr<Transcriber> _transcriber,
         boost::shared_ptr<ThreadedImageTranscriber> _imageTranscriber,
         boost::shared_ptr<MotionEnactor> _enactor,
         boost::shared_ptr<Synchro> synchro,
         boost::shared_ptr<Lights> _lights,
         boost::shared_ptr<Speech> _speech);

    virtual ~TMan();

    virtual void startSubThreads();

    virtual void stopSubThreads();

private:
    boost::shared_ptr<ThreadedImageTranscriber> threadedImageTranscriber;

};

#endif
