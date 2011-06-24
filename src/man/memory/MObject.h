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

#define REGISTER_MOBJECT(x) {this->setName(x);}

namespace man {
namespace memory {

class MObject {

public:
    typedef boost::shared_ptr<MObject> ptr;
    typedef boost::shared_ptr<const MObject> const_ptr;

    MObject() : name("unknown"){
    }
    /**
     * method update - this should be overwritten by a method that sets all of
     * the proto message fields with values from its respective man counterpart
     */
    virtual void update() = 0;

protected:
    void setName(std::string new_name) { name = new_name; }

private:
    std::string name;

};

}
}
