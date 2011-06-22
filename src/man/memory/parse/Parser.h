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

class Parser {

public:
    Parser() {}

    virtual const LogHeader getHeader() {return log_header;}
    virtual bool getNext() = 0;

protected:
    LogHeader log_header;
};

template <class T>
class TemplatedParser : public Parser {

public:
    TemplatedParser(boost::shared_ptr<T> container) : container(container) {
    }


    //TODO: change these to void/bool move them to the generic parser
    virtual bool getNext() = 0;
    virtual boost::shared_ptr<const T> getPrev() = 0;


protected:
    boost::shared_ptr<T> container;

};

}
}
}
