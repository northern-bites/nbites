#ifndef _ThreadedImageTranscriber_h
#define _ThreadedImageTranscriber_h


#include "ImageTranscriber.h"
#include "synchro/synchro.h"

class ThreadedImageTranscriber : public ImageTranscriber , public Thread{
 public:
 ThreadedImageTranscriber(boost::shared_ptr<Sensors> sensors,
                          std::string name)
     : ImageTranscriber(sensors), Thread(name){};
    virtual ~ThreadedImageTranscriber() { }

};


#endif
