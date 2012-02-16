/**
 * @class FileFDProvider
 *
 * Opens a file descriptor from the specified file.
 *
 */


#pragma once

#include <iostream>
#include <fcntl.h>
#include <string>
#include "IOProvider.h"

namespace common {
namespace io {

enum OpenType {
    //O_APPEND is crucial if you want to use aio_write on the file descriptor
    //you open
    NEW = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
    EXISTING = O_RDONLY
};

class FileFDProvider : public IOProvider {

public:
    FileFDProvider(std::string file_name,
            int flags = EXISTING ) :
                IOProvider(),
                file_name(file_name),
                flags(flags) {
    }

    virtual ~FileFDProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return "file name: " + file_name;
    }

    void openCommunicationChannel() {

        file_descriptor = open(file_name.c_str(),
                flags, S_IRWXU | S_IRWXG | S_IRWXO);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << debugInfo() << std::endl;
            file_descriptor = -1;
        }
    }

private:
    std::string file_name;
    int flags;

};

}
}
