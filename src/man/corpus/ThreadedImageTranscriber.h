#ifndef _ThreadedImageTranscriber_h
#define _ThreadedImageTranscriber_h

#include "ImageTranscriber.h"
#include "synchro/synchro.h"
#include "ClassHelper.h"

class ThreadedImageTranscriber : public ImageTranscriber , public Thread{

ADD_SHARED_PTR(ThreadedImageTranscriber);

public:
	ThreadedImageTranscriber(boost::shared_ptr<Sensors> sensors,
			std::string name)
		: ImageTranscriber(sensors), Thread(name){};
	virtual ~ThreadedImageTranscriber() { }

	virtual void run() = 0;

	virtual void signalNewImage() {
		this->signalToResume();
	}

protected:
	virtual void waitForNewImage() {
		this->waitForSignal();
	}

};


#endif
