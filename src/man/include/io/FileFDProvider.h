/**
 * @class FileFDProvider
 *
 * Opens a file descriptor from the specified file.
 *
 */


#pragma once

#include <iostream>
#include <fcntl.h>
#include "FDProvider.h"

class FileFDProvider : public FDProvider {

public:
    FileFDProvider(const char* fileName,
            int	flags = O_WRONLY | O_CREAT | O_TRUNC,
            int permissions = S_IRWXU | S_IRWXG | S_IRWXO) :
                FDProvider(),
                fileName(fileName),
                flags(flags), permissions(permissions){
        openFileDescriptor();
    }

    void openFileDescriptor() {

        file_descriptor = open(fileName,
                flags, permissions);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << fileName << std::endl;
            file_descriptor = 0;
        }
    }

private:
    const char* fileName;
    int flags, permissions;

};
