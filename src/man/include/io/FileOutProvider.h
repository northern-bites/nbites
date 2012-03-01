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
                flags(NEW), is_open(false) {
    }

    virtual ~FileOutProvider() {
        this->closeCommunicationChannel();
    }

    virtual std::string debugInfo() const {
        return "file name: " + file_name;
    }

    void openCommunicationChannel() throw (file_exception) {

        file_descriptor = open(file_name.c_str(), flags, PERMISSIONS_ALL);

        if (file_descriptor < 0) {
            throw file_exception(file_exception::CREATE_ERR);
        }

        is_open = true;
    }

    virtual bool opened() const {
        return is_open;
    }

    virtual void closeCommunicationChannel() {
        close(file_descriptor);
        file_descriptor = -1;
        is_open = false;
    }

private:
    std::string file_name;
    OpenFlags flags;
    mutable bool is_open;

};

}
}
