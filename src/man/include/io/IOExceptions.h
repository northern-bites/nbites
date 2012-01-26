
#pragma once

#include <errno.h>
#include <exception>

namespace common {
namespace io {

class aio_read_exception : public std::exception {

public:
    enum code {
        ENQUE = 4,
        READ
    };

    aio_read_exception(code errcode) : errcode(errcode) {}

    virtual const char* what() const throw() {
        switch (errcode) {
        case (ENQUE) :
        return (std::string("enque error ") + strerror(errno)).c_str();
        break;
        case (READ) :
        return (std::string("read error ") + strerror(errno)).c_str();
        break;
        default:
        return (std::string("unknown error ") + strerror(errno)).c_str();
        break;
        }
        return (std::string("unknown error ") + strerror(errno)).c_str();
    }

private:
    code errcode;
};

}
}
