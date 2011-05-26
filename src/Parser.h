/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>

#include "ProtoDefs.h"

namespace memory {

namespace log {

namespace proto = google::protobuf;

struct LogHeader {

    proto::uint32 log_id;
    proto::uint64 birth_time;

};

class Parser {

public:
    Parser(boost::shared_ptr<proto::Message> message) {
        current_message = message;
    }

    virtual ~Parser() {}

    virtual const LogHeader getHeader() = 0;
    virtual boost::shared_ptr<const proto::Message> getNextMessage() = 0;
    virtual boost::shared_ptr<const proto::Message> getPrevMessage() = 0;


protected:
    LogHeader log_header;
    boost::shared_ptr<proto::Message> current_message;

};

}
}
