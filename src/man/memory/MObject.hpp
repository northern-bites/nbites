/**
 * MObject.hpp
 *
 * @class MObject : short for Memory Object, is intended to be the interface
 * to the generic wrapper class to the protobuffer subsystem we employ to
 * manage important data
 *
 *      Author: oneamtu
 */

#pragma once

namespace memory {

enum MObject_ID {
    MVISION_ID = 1
};

class MObject {

public:
    virtual void update() = 0;
    virtual void log() const = 0;

};
}
