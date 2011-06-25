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

//TODO: create a separate C++ class

class FileFDProvider : public FDProvider {

public:
    FileFDProvider(const char* file_name,
            int	flags = O_WRONLY | O_CREAT | O_TRUNC,
            int permissions = S_IRWXU | S_IRWXG | S_IRWXO) :
                FDProvider(),
                file_name(file_name),
                flags(flags), permissions(permissions){
        openFileDescriptor();
    }

    FileFDProvider(std::string file_name,
            int flags = O_WRONLY | O_CREAT | O_TRUNC,
            int permissions = S_IRWXU | S_IRWXG | S_IRWXO) :
                FDProvider(),
                file_name(file_name),
                flags(flags), permissions(permissions){
        openFileDescriptor();
    }

    virtual ~FileFDProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() {
        return "file name: " + file_name;
    }

protected:
    void openFileDescriptor() {

        file_descriptor = open(file_name.c_str(),
                flags, permissions);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << file_name << std::endl;
            file_descriptor = 0;
        }
    }

private:
    std::string file_name;
    int flags, permissions;

};

}
}
}
