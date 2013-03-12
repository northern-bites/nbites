#pragma once

#include <cstring>
#include <exception>

// Log version--in case we need to upgrade and stay backwards compatible
static const std::string VERSION = "2.0";
// Header. This could be updated with more useful information.
static const std::string HEADER = "NORTHERN BITES LOG FILE VERSION " + VERSION;

// 1 GB. We don't want to write files of absurd sizes.
static const unsigned int FILE_MAX_SIZE = 1073741824;

// IO Exceptions
class read_exception: public std::exception {

};

class file_read_exception: public read_exception {

public:
    enum code {
        READ = 4, NOT_OPEN
    };

    file_read_exception(code errcode, int err_no = 0) :
            errcode(errcode), err_no(err_no) {
    }

    virtual const char* what() const throw () {
        std::string error_message = "";
        if (err_no) {
            error_message = strerror(err_no);
        }
        std::string message = "";

        switch (errcode) {
        case (READ):
            message = "File read error ";
            break;
        case (NOT_OPEN):
            message = "File not open!";
            break;
        default:
            message = "File unknown error ";
            break;
        }

        return (message + ":" + error_message).c_str();
    }

private:
    code errcode;
    int err_no;
};

class aio_read_exception: public read_exception {

public:
    enum code {
        ENQUEUE = 4, READ, NOT_OPEN, IN_PROGRESS
    };

    aio_read_exception(code errcode, int err_no = 0) :
            errcode(errcode), err_no(err_no) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (err_no) {
            message = strerror(err_no);
        }
        switch (errcode) {
        case (ENQUEUE):
            return (std::string("AIO enqueue error ") + message).c_str();
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
    int err_no;
};

class io_exception : public std::exception {

};

class socket_exception: public io_exception {

public:
    enum code {
        TIMED_OUT = 17, FCNTL_ERR, CREATE_ERR, BIND_ERR, LISTEN_ERR, ACCEPT_ERR
    };

    socket_exception(code errcode, int err_no = 0) :
            errcode(errcode), err_no(err_no) {
    }

    virtual const char* what() const throw () {
        std::string err_message = "";
        if (err_no) {
            err_message = strerror(err_no);
        }

        std::string message = "";
        switch (errcode) {
        case (TIMED_OUT):
            message = "Connection timed out!";
            break;
        case (FCNTL_ERR):
            message = "fcntl failed on socked fd!";
            break;
        case (CREATE_ERR):
            message = "could not create socket fd";
            break;
        case (BIND_ERR):
            message = "could not bind socket fd";
            break;
        case (LISTEN_ERR):
            message = "could not listen on socket fd";
            break;
        case (ACCEPT_ERR):
            message = "could not accept connections on socket fd";
            break;
        default:
            message = "Unknown socket exception!";
            break;
        }
        return (message + ":" + err_message).c_str();
    }

private:
    code errcode;
    int err_no;

};

class file_exception: public io_exception {

public:
    enum code {
        CREATE_ERR = 17
    };

    file_exception(code errcode, int err_no = 0) :
            errcode(errcode), err_no(err_no) {
    }

    virtual const char* what() const throw () {
        std::string message;
        if (err_no) {
            message = strerror(err_no);
        }
        switch (errcode) {
        case (CREATE_ERR):
            return ("could not create file" + message).c_str();
            break;
        default:
            return "Unknown file exception!";
            break;
        }
        return "Unknown file exception!";
    }

private:
    code errcode;
    int err_no;

};
