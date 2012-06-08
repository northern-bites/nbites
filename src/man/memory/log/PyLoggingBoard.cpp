
#include "PyLoggingBoard.h"

#include <boost/python.hpp>

namespace man {
namespace memory {
namespace log {

using boost::shared_ptr;
shared_ptr<LoggingBoard> loggingBoard_ptr;
using namespace boost::python;

BOOST_PYTHON_MODULE(loggingBoard) {

    class_<LoggingBoard, shared_ptr<LoggingBoard> >("LoggingBoard", no_init)
            .def("startLogging", &LoggingBoard::startLogging)
            .def("stopLogging", &LoggingBoard::stopLogging)
            .def_readonly("isLogging", &LoggingBoard::isLogging);

    scope().attr("loggingBoard") = loggingBoard_ptr;
}

void c_init_logging() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initloggingBoard();
    } catch (error_already_set&) {
        PyErr_Print();
    }
}

void set_logging_board_pointer (shared_ptr<LoggingBoard> logging_ptr) {
    loggingBoard_ptr = logging_ptr;
}

}
}
}
