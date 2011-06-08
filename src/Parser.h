/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include "ProtoDefs.h"

namespace memory {

namespace log {

struct LogHeader {

    uint32_t log_id;
    uint64_t birth_time;

};

template <class T>
class Parser {

public:
    Parser(boost::shared_ptr<T> message) {
        current_message = message;
    }

    virtual ~Parser() {}

    virtual const LogHeader getHeader() = 0;
    virtual boost::shared_ptr<const T> getNextMessage() = 0;
    virtual boost::shared_ptr<const T> getPrevMessage() = 0;


protected:
    LogHeader log_header;
    boost::shared_ptr<T> current_message;

};

}
}
