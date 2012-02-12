/**
 * MObject.hpp
 *
 * @class MObject : short for Memory Object, is intended to be the interface
 * to the generic wrapper class to the protobuffer subsystem we employ to
 * manage important data
 *
 * By default it uses the underlying protocol buffer object to implement the serialization/
 * de-serialization of the data, but most of the function calls all virtual
 * so they can be overwritten (for example if you want to handle serialization in some different
 * way or want your inheriting MObject to not use ProtoBuffers at all)
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <stdint.h>

#include "MemoryCommon.h"
#include "Common.h"
#include "ClassHelper.h"

#include "Notifier.h"
#include "synchro/mutex.h"

namespace man {
namespace memory {

class MObject : public SpecializedNotifier<MObject_ID>, public Subscriber {

    ADD_SHARED_PTR(MObject)
    ADD_NULL_INSTANCE(MObject)

public:
    static long long int time_stamp() { return realtime_micro_time(); }

protected:
    /*
     * @params:
     * id : the id of this MObject (each MObject should have a unique
     * MObject_ID associated with it)
     * protoMessage : the protocol message associated with this MObject
     */
    MObject(MObject_ID id = UNKNOWN_OBJECT,
            ProtoMessage_ptr protoMessage = ProtoMessage_ptr());

public:
    virtual ~MObject(){}

    /**
     * method update - this should be overwritten by a method that fills all of
     * the proto message fields with relevant values
     */
    virtual void updateData() {}
    virtual void update();

    //TODO: make this pure virtual and implement in other class
    //or find generic way to implement
    virtual const std::string& getName() const {return MObject_names[my_id];}
    virtual MObject_ID getID() const {return my_id;}
    virtual long long getBirthTime() const {return birth_time;}
    virtual void serializeToString(std::string* write_buffer) const;
    virtual void parseFromBuffer(const char* read_buffer, uint32_t buffer_size);
    virtual unsigned byteSize() const;

    ProtoMessage_const_ptr getProtoMessage() const { return protoMessage;}
    ProtoMessage_ptr getMutableProtoMessage() { return protoMessage;}

protected:
    MObject_ID my_id;
    ProtoMessage_ptr protoMessage;
    mutex objectMutex;
    long long int birth_time;

};

}
}
