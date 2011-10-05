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
#include "FDProvider.h"

namespace man {
namespace include {
namespace io {

enum OpenType {
    NEW = O_WRONLY | O_CREAT | O_TRUNC,
    EXISTING = O_RDONLY
};

class FileFDProvider : public FDProvider {

public:
    FileFDProvider(const char* file_name,
            int	flags = EXISTING) :
                FDProvider(),
                file_name(file_name),
                flags(flags) {
        openFileDescriptor();
    }

    FileFDProvider(std::string file_name,
            int flags = EXISTING ) :
                FDProvider(),
                file_name(file_name),
                flags(flags) {
        openFileDescriptor();
    }

    virtual ~FileFDProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return "file name: " + file_name;
    }

    virtual bool rewind(uint32_t offset) const {
        if (offset < getCurrentPosition()) {
            uint32_t true_offset = 0;
            true_offset = lseek(file_descriptor, -offset, SEEK_CUR);
            //TODO: we could check to see if the true_offset
            //signals an error or just moves the pointer
            //by a smaller offset than what we want;
            //in the latter case we could try to revert
            //the changes done - Octavian
            if (true_offset == -offset) {
                return true;
            }
        }
        return false;
    }

protected:
    void openFileDescriptor() {

        file_descriptor = open(file_name.c_str(),
                flags, S_IRWXU | S_IRWXG | S_IRWXO);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << file_name << std::endl;
            file_descriptor = 0;
        }
    }

private:
    std::string file_name;
    int flags;

};

}
}
}
