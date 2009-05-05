#ifndef ALIMAGE_TRANSCRIBER_H
#define ALIMAGE_TRANSCRIBER_H

#include "ImageTranscriber.h"

class ALImageTranscriber : public ImageTranscriber, public Thread {
public:
    ALImageTranscriber(boost::shared_ptr<Sensors> s);
};

#endif
