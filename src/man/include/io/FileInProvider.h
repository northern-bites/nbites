/**
 * @class FileInProvider
 *
 * Opens a file descriptor for reading from the specified file.
 *
 */

#pragma once

#include <iostream>
#include <fcntl.h>
#include <string>
#include "InProvider.h"
#include "FileDefs.h"

namespace common {
namespace io {

class FileInProvider : public InProvider {

public:
    FileInProvider(std::string file_name) :
                file_name(file_name),
                flags(EXISTING), has_been_opened(false) {
    }

    virtual ~FileInProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return "file name: " + file_name;
    }

    virtual uint64_t getCurrentPosition() const {
        return lseek64(file_descriptor, 0, SEEK_CUR);
    }

    virtual bool rewind(uint64_t offset) const {
        if (offset < getCurrentPosition()) {
            uint64_t true_offset = 0;
            true_offset = lseek64(file_descriptor, -offset, SEEK_CUR);
            //TODO: we could check to see if the true_offset
            //signals an error or just moves the pointer
            //by a smaller offset than what we want;
            //in the latter case we could try to revert
            //the changes done - Octavian

            //if the return value is the same as (-offset) -1
            //then lseek is signaling an error
            if (true_offset != -offset-1) {
                return true;
            }
        }
        return false;
    }

    void openCommunicationChannel() {

        file_descriptor = open(file_name.c_str(), flags);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << debugInfo() << std::endl;
            file_descriptor = -1;
            return;
        }

        has_been_opened= true;
    }

    virtual bool opened() const {
        return has_been_opened;
    }

private:
    std::string file_name;
    OpenFlags flags;
    bool has_been_opened;

};

}
}
