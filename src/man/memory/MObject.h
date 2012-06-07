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

template <class ProtoType>
class MObject : public common::io::NotifyingProtobufMessage {

    ADD_SHARED_PTR(MObject);

    typedef boost::shared_ptr<ProtoType> data_ptr;
    typedef boost::shared_ptr<const ProtoType> data_const_ptr;

protected:
    /*
     * @params:
     * id : the id of this MObject (each MObject should have a unique
     * MObject_ID associated with it)
     * protoMessage : the protocol message associated with this MObject
     */
    MObject(MObject_ID id, data_ptr protoMessage) :
        NotifyingProtobufMessage(protoMessage, MObject_names[id], (int32_t) id),
        my_id(id),
        data(protoMessage) {   }

public:
    virtual ~MObject(){}

    /**
     * method update - this should be overwritten by a method that fills all of
     * the proto message fields with relevant values
     */
    virtual void updateData() = 0;

    data_const_ptr get() const { return data; }
    data_ptr get() { return data; }

    virtual MObject_ID getID() const { return my_id; }

protected:
    MObject_ID my_id;
    data_ptr data;

};

}
}
