/*
 * Common definitions for the Memory module
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>

#include "io/ProtobufMessage.h"

namespace man {
namespace memory {

typedef google::protobuf::Message ProtoMessage;
typedef boost::shared_ptr<ProtoMessage> ProtoMessage_ptr;
typedef boost::shared_ptr<const ProtoMessage> ProtoMessage_const_ptr;

using namespace common::io;

}
}
