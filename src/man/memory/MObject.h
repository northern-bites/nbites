/**
 * MObject.hpp
 *
 * @class MObject : short for Memory Object, is intended to be the interface
 * to the generic wrapper class to the protobuffer subsystem we employ to
 * manage important data
 *
 * Most essential functions are virtual, so each inheriting class can modify the
 * way it gets serialized, etc.
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include "MemoryCommon.h"

namespace man {
namespace memory {

//TODO: add a specialized Null object class and use it instead of a null
// pointer for when we don't want an object to be initialized

class MObject {

public:
    typedef boost::shared_ptr<MObject> ptr;
    typedef boost::shared_ptr<const MObject> const_ptr;

protected:
    /*
     * @params:
     * id : the id of this MObject (each MObject should have a unique
     * MObject_ID associated with it)
     * protoMessage : the protocol message associated with this MObject
     */
    MObject(MObject_ID id, ProtoMessage_ptr protoMessage);

public:
    virtual ~MObject(){}
    /**
     * method update - this should be overwritten by a method that sets all of
     * the proto message fields with values from its respective man counterpart
     */
    virtual void update() = 0;

    //TODO: make this pure virtual and implement in other class
    //or find generic way to implement
    virtual const std::string& getName() const {return MObject_names[id];}
    virtual void serializeToString(std::string* write_buffer) const;
    virtual unsigned byteSize() const;

    ProtoMessage_const_ptr getProtoMessage() const { return protoMessage;}
    ProtoMessage_ptr getMutableProtoMessage() { return protoMessage;}

protected:
    MObject_ID id;
    ProtoMessage_ptr protoMessage;
    std::string name;

};

}
}
