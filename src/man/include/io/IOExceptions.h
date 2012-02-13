#pragma once

#include <cstring>
#include <exception>

namespace common {
namespace io {

class read_exception: public std::exception {

};

class file_read_exception: public read_exception {

public:
    enum code {
        READ = 4, NOT_OPEN
    };

    file_read_exception(code errcode, int errno = 0) :
            errcode(errcode), errno(errno) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (errno) {
            message = strerror(errno);
        }
        switch (errcode) {
        case (READ):
            return (std::string("file read error ") + message).c_str();
            break;
        case (NOT_OPEN):
            return ("FileIn not open!");
            break;
        default:
            return (std::string("file unknown error ") + message).c_str();
            break;
        }
        return (std::string("file unknown error ") + message).c_str();
    }

private:
    code errcode;
    int errno;
};

class aio_read_exception: public read_exception {

public:
    enum code {
        ENQUE = 4, READ, NOT_OPEN, IN_PROGRESS
    };

    aio_read_exception(code errcode, int errno = 0) :
            errcode(errcode), errno(errno) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (errno) {
            message = strerror(errno);
        }
        switch (errcode) {
        case (ENQUE):
            return (std::string("AIO enque error ") + message).c_str();
            break;
        case (READ):
            return (std::string("AIO read error ") + message).c_str();
            break;
        case (NOT_OPEN):
            return ("SocketIn not open!");
            break;
        case (IN_PROGRESS):
            return "Last read is still in progress!";
            break;
        default:
            return (std::string("AIO unknown error ") + message).c_str();
            break;
        }
        return (std::string("AIO unknown error ") + message).c_str();
    }

private:
    code errcode;
    int errno;
};

class io_exception : public std::exception {

};

class socket_exception: public io_exception {

public:
    enum code {
        TIMED_OUT = 17, FCNTL_ERR, CREATE_ERR
    };

    socket_exception(code errcode, int errno = 0) :
            errcode(errcode), errno(errno) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (errno) {
            message = strerror(errno);
        }
        switch (errcode) {
        case (TIMED_OUT):
            return "Connection timed out!";
            break;
        case (FCNTL_ERR):
            return ("fcntl failed on socked fd!" + message).c_str();
            break;
        case (CREATE_ERR):
            return ("could not create socket fd" + message).c_str();
            break;
        default:
            return "Unknown socket exception!";
            break;
        }
        return "Unknown socket exception!";
    }

private:
    code errcode;
    int errno;

};

class file_exception: public io_exception {

public:
    enum code {
        CREATE_ERR = 17
    };

    file_exception(code errcode, int errno = 0) :
            errcode(errcode), errno(errno) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (errno) {
            message = strerror(errno);
        }
        switch (errcode) {
        case (CREATE_ERR):
            return ("could not create socket fd" + message).c_str();
            break;
        default:
            return "Unknown socket exception!";
            break;
        }
        return "Unknown socket exception!";
    }

private:
    code errcode;
    int errno;

};

}
}
