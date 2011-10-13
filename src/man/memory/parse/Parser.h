/**
 * Abstract log parser
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <unistd.h>
#include <stdint.h>
#include <include/io/FDProvider.h>

namespace man {
namespace memory {
namespace parse {

struct LogHeader {

    int32_t log_id;
    int64_t birth_time;

};

class Parser {

public:
    typedef boost::shared_ptr<Parser> ptr;

protected:
    typedef include::io::FDProvider FDProvider;

public:
    Parser(FDProvider::const_ptr fdProvider) :
                fdProvider(fdProvider) {
    }

    virtual LogHeader getHeader() const {return log_header;}

    virtual bool getNext() = 0;
    virtual bool getPrev() = 0;

    // this method will try to look at a log represented by a file_descriptor
    // and read the first int, which represents the log ID
    // this method will NOT advance the file_descriptor
    static int peekAtLogID(int file_descriptor) {
        int id;
        read(file_descriptor, &id, sizeof(int));
        lseek(file_descriptor, -sizeof(int), SEEK_CUR);
        return id;
    }

    uint32_t getBytesRead() {
        return fdProvider->getCurrentPosition();
    }

protected:
    template <class T>
    void readValue(T &value) {
        read(fdProvider->getFileDescriptor(), &value, sizeof(value));
    }

    bool readCharBuffer(char* buffer, uint32_t size) {
        uint32_t result = 0;
        result = read(fdProvider->getFileDescriptor(), buffer, size);

        if (result != size)
            return false;
        else
            return true;
    }

protected:
    FDProvider::const_ptr fdProvider;
    LogHeader log_header;
};

}
}
}
