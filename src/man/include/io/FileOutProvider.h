/**
 * @class FileOutProvider
 *
 * Opens a file descriptor for reading from the specified file.
 *
 */

#pragma once

#include <iostream>
#include <fcntl.h>
#include <string>
#include "OutProvider.h"
#include "FileDefs.h"

namespace common {
namespace io {

class FileOutProvider : public OutProvider {

public:
    FileOutProvider(std::string file_name) :
                file_name(file_name),
                flags(NEW), has_been_opened(false) {
    }

    virtual ~FileOutProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return "file name: " + file_name;
    }

    void openCommunicationChannel() {

        file_descriptor = open(file_name.c_str(), flags, PERMISSIONS_ALL);

        if (file_descriptor < 0) {
            std::cout << "Could not open file: " << debugInfo() << std::endl;
            file_descriptor = -1;
            return;
        }

        has_been_opened = true;
    }

    virtual bool opened() const {
        return has_been_opened;
    }

    virtual void closeCommunicationChannel() const {
        close(file_descriptor);
        has_been_opened = false;
    }

private:
    std::string file_name;
    OpenFlags flags;
    mutable bool has_been_opened;

};

}
}
