#include "memory/MVisionSensors.h"
#include "memory/MMotionSensors.h"
#include "corpus/Transcriber.h"

#pragma once

namespace man {
namespace corpus {

class OfflineTranscriber : public Transcriber{
public:
    OfflineTranscriber(boost::shared_ptr<Sensors> s,
    		memory::MVisionSensors::const_ptr mvisionSensors,
    		memory::MMotionSensors::const_ptr mmotionSensors);
    virtual ~OfflineTranscriber();

    void postMotionSensors();
    void postVisionSensors();

private:
    memory::MVisionSensors::const_ptr mvisionSensors;
    memory::MMotionSensors::const_ptr mmotionSensors;
};

}
}
