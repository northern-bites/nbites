/**
 * Memory.hpp
 *
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object pulls data
 * from its corresponding object
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace memory {
class Memory; //forward declaration
}

#include "MVision.h"
#include "Vision.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"
#include "Sensors.h"
#include "Profiler.h"
#include "log/LoggingBoard.h"

namespace memory {

enum MObject_ID {
    MVISION_ID = 1,
    MMOTION_SENSORS_ID,
    MVISION_SENSORS_ID,
    MIMAGE_ID
};

class Memory : public Subscriber<SensorsEvent> {

public:
    Memory();

    Memory( boost::shared_ptr<Profiler> profiler_ptr,
            boost::shared_ptr<Vision> vision_ptr,
            boost::shared_ptr<Sensors> sensors_ptr);
    ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object and maybe log it
     */
    void update(MObject* obj);
    void updateVision();

    /**
     * This function is called whenever one of the Providers we are subscribed
     * to has something new/updated
     */
    void update(SensorsEvent eventID);

public:
    const MVision* getMVision() const {return mVision;}
    const MVisionSensors* getMVisionSensors() const {return mVisionSensors;}
    const MMotionSensors* getMMotionSensors() const {return mMotionSensors;}
    const MImage* getMImage() const {return mImage;}

    MImage* getMutableMImage() {return mImage;}

private:
    boost::shared_ptr<Profiler> _profiler;
    boost::shared_ptr<Sensors> _sensors;
    MVision* mVision;
    MVisionSensors* mVisionSensors;
    MMotionSensors* mMotionSensors;
    MImage* mImage;

    log::LoggingBoard* loggingBoard;
};
}
