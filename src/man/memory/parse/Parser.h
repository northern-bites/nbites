/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <unistd.h>

#include <include/io/FDProvider.h>

namespace man {
namespace memory {
namespace parse {

struct LogHeader {

    uint32_t log_id;
    uint64_t birth_time;

};

class Parser {

public:
    typedef boost::shared_ptr<Parser> ptr;

public:
    Parser() {}

    virtual const LogHeader getHeader() {return log_header;}
    virtual bool getNext() = 0;

    // this method will try to look at a log represented by a file_descriptor
    // and read the first int, which represents the log ID
    // this method will NOT advance the file_descriptor
    static int peekAtLogID(int file_descriptor) {
        int id;
        read(file_descriptor, &id, sizeof(int));
        lseek(file_descriptor, -sizeof(int), SEEK_CUR);
        return id;
    }

protected:
    LogHeader log_header;
};

template <class T>
class TemplatedParser : public Parser {

public:
    TemplatedParser(include::io::FDProvider::const_ptr fdProvider,
            boost::shared_ptr<T> container) :
                fdProvider(fdProvider), container(container) {
    }

    //TODO: change these to void/bool move them to the generic parser
    virtual bool getNext() = 0;
    virtual boost::shared_ptr<const T> getPrev() = 0;


protected:
    include::io::FDProvider::const_ptr fdProvider;
    boost::shared_ptr<T> container;

};

}
}
}
