/*
 * Common definitions for the Memory module
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>

namespace man {
namespace memory {

typedef google::protobuf::Message ProtoMessage;
typedef boost::shared_ptr<ProtoMessage> ProtoMessage_ptr;
typedef boost::shared_ptr<const ProtoMessage> ProtoMessage_const_ptr;

enum MObject_ID {
    UNKOWN_OBJECT = 0,
    MVISION_ID,
    MMOTION_SENSORS_ID,
    MVISION_SENSORS_ID,
    MIMAGE_ID,
    MLOCALIZATION_ID,
    LAST_OBJECT_ID //dummy object
};

inline void operator++(MObject_ID& id) {
    id = MObject_ID(id+1);
}

inline void operator++(MObject_ID& id, int) {
    id = MObject_ID(id+1);
}

static const MObject_ID FIRST_OBJECT_ID = MVISION_ID;

static const std::string MObject_names[] = {
            "unknown",
            "Vision",
            "MotionSensors",
            "VisionSensors",
            "Image",
            "Localization"
};

}
}
