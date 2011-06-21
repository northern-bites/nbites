/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

namespace man {
namespace memory {
namespace parse {

struct LogHeader {

    uint32_t log_id;
    uint64_t birth_time;

};

template <class T>
class Parser {

public:
    Parser(boost::shared_ptr<T> container) : container(container) {
    }

    virtual ~Parser() {}

    virtual const LogHeader getHeader() = 0;
    virtual boost::shared_ptr<const T> getNext() = 0;
    virtual boost::shared_ptr<const T> getPrev() = 0;


protected:
    LogHeader log_header;
    boost::shared_ptr<T> container;

};

}
}
}
