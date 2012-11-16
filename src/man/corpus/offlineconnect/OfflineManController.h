/**
 *
 * @class OfflineManController
 *
 * Provides a way to control the ThreadedImageTranscriber and OfflineEnactor
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <string>
#include "corpus/ThreadedImageTranscriber.h"
#include "ClassHelper.h"
#include "man/memory/Memory.h"

#include "TMan.h"

#include "Profiler.h"
#include "corpus/Sensors.h"
#include "corpus/RoboGuardian.h"
#include "corpus/offlineconnect/OfflineImageTranscriber.h"
#include "corpus/offlineconnect/OfflineTranscriber.h"
#include "corpus/offlineconnect/OfflineEnactor.h"

namespace man {
namespace corpus{

class OfflineManController {

    ADD_SHARED_PTR(OfflineManController);

public:
    
    OfflineManController(memory::Memory::const_ptr fakeMemory) {

        RobotMemory::ptr memory(new RobotMemory());

        boost::shared_ptr<Speech> speech(new Speech());
        boost::shared_ptr<Sensors> sensors(new Sensors(speech,
            memory->get<MVisionSensors>(),
            memory->get<MMotionSensors>()));
        boost::shared_ptr<Transcriber> transcriber(new OfflineTranscriber(sensors,
            fakeMemory->get<MVisionSensors>(),
            fakeMemory->get<MMotionSensors>()));
        imageTranscriber = boost::shared_ptr<ThreadedImageTranscriber>(new OfflineImageTranscriber(sensors,
            fakeMemory->get<MRawImages>()));
        boost::shared_ptr<MotionEnactor> enactor(new OfflineEnactor());
        boost::shared_ptr<Lights> lights(new Lights());

        man = boost::shared_ptr<TMan>(new TMan(memory, sensors, transcriber,
          imageTranscriber,
          enactor, lights, speech));
    }

    void loadTable(const std::string &path) {
        imageTranscriber->initTable(path);
    }

    void signalNextImageFrame() {
        imageTranscriber->signalNewImage();
    }

    boost::shared_ptr<Man> getMan() { return man; } 

private:
    boost::shared_ptr<TMan> man;
    boost::shared_ptr<ThreadedImageTranscriber> imageTranscriber;
    memory::Memory::const_ptr fakeMemory;
};

}
}
