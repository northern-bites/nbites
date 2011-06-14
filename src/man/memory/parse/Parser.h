/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

namespace memory {

namespace parse {

struct LogHeader {

    uint32_t log_id;
    uint64_t birth_time;

};

class Parser {

public:
    Parser() {};
    virtual ~Parser();

    virtual const LogHeader getHeader() = 0;

protected:
    LogHeader log_header;
};

template <class T>
class TemplatedParser : Parser {

public:
    TemplatedParser(boost::shared_ptr<T> container) : container(container) {
    }

    virtual ~TemplatedParser() {}


    virtual boost::shared_ptr<const T> getNext() = 0;
    virtual boost::shared_ptr<const T> getPrev() = 0;


protected:
    boost::shared_ptr<T> container;

};

}
}
