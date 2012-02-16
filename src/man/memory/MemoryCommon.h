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

namespace MObject_IDs {

enum MObject_ID {
    UNKNOWN_OBJECT = 0,
    MVISION_ID,
    MMOTION_SENSORS_ID,
    MVISION_SENSORS_ID,
    MIMAGE_ID,
    MLOCALIZATION_ID,
    LAST_OBJECT_ID //dummy object
};

static const MObject_ID FIRST_OBJECT_ID = MVISION_ID;

// the ++ give an MObject_ID iterator-like properties

inline void operator++(MObject_ID& id) {
    id = MObject_ID(id+1);
}

inline void operator++(MObject_ID& id, int) {
    id = MObject_ID(id+1);
}

static const std::string MObject_names[] = {
            "unknown",
            "Vision",
            "MotionSensors",
            "VisionSensors",
            "Image",
            "Localization"
};

}

//using the combination of namespace and using namespace
//effectively hides the MObject_ID enum in other namespaces other than
//man vision; if another namespace wishes to use it, it should do
//using namespace man::memory::MObject_IDs;
using namespace MObject_IDs;


}
}
