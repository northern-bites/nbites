
#pragma once

#include "man/memory/MObject.h"

namespace qtool {
namespace data {

typedef man::memory::MObject_ID MObject_ID;

enum DataEvent {
    NEW_IMAGE = 17,
    NEW_VISION_SENSORS,
    NEW_MOTION_SENSORS,
    NEW_VISION_INFO
};


}
}
