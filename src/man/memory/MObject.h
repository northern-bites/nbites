/**
 * MObject.hpp
 *
 * @class MObject : short for Memory Object, is intended to be the interface
 * to the generic wrapper class to the protobuffer subsystem we employ to
 * manage important data
 *
 * N.B. All derived classes should call setName or REGISTER_MOBJECT in their constructor
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

namespace man {
namespace memory {

//IDs and names are tightly linked, be sure to modify both when needed!
enum MObject_ID {
    MVISION_ID = 1,
    MMOTION_SENSORS_ID,
    MVISION_SENSORS_ID,
    MIMAGE_ID
};

static const std::string MObject_names[] = {
            "unknown",
            "Vision",
            "MotionSensors",
            "VisionSensors",
            "Image"
};

class MObject {

public:
    typedef boost::shared_ptr<MObject> ptr;
    typedef boost::shared_ptr<const MObject> const_ptr;

    MObject(MObject_ID id, std::string name) : id(id), name(name) {
    }
    /**
     * method update - this should be overwritten by a method that sets all of
     * the proto message fields with values from its respective man counterpart
     */
    virtual void update() = 0;
    static const std::string* NameFromID(MObject_ID id) {
        return &MObject_names[static_cast<int>(id)];
    }

private:
    MObject_ID id;
    std::string name;

};

}
}
