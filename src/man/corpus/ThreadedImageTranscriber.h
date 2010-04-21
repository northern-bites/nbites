#ifndef _ThreadedImageTranscriber_h
#define _ThreadedImageTranscriber_h


#include "ImageTranscriber.h"
#include "synchro.h"

class ThreadedImageTranscriber : public ImageTranscriber , public Thread{
public:
    ThreadedImageTranscriber(boost::shared_ptr<Sensors> sensors,
                             boost::shared_ptr<Synchro> synchro,
                             std::string name)
        : ImageTranscriber(sensors), Thread(synchro, name){};
    virtual ~ThreadedImageTranscriber() { }

};


#endif
