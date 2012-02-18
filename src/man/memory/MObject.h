/**
 * MObject.hpp
 *
 * @class MObject : short for Memory Object
 *
 * By default it uses the underlying ProtobufMessage wrapper object to implement the serialization/
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

#include "io/ProtobufMessage.h"
#include "MemoryCommon.h"
#include "Common.h"
#include "ClassHelper.h"

#include "Notifier.h"

namespace man {
namespace memory {

class MObject : public SpecializedNotifier<MObject_ID>, public Subscriber,
                public common::io::ProtobufMessage {

    ADD_SHARED_PTR(MObject)

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
    virtual void updateData() = 0;
    /**
     * this gets called when the provider for this MObject is notifying us for
     * new data
     */
    virtual void update();
    virtual void parseFromBuffer(const char* read_buffer, uint32_t buffer_size);

    virtual MObject_ID getID() const {return my_id;}

protected:
    MObject_ID my_id;

};

}
}
