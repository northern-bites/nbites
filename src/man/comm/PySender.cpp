#include "PySender.h"

#include <boost/python.hpp>
using namespace boost::python;
using boost::shared_ptr;

shared_ptr<CommandSender> cs_pointer;

BOOST_PYTHON_MODULE(sender)
{
    class_<CommandSender, shared_ptr<CommandSender> >("Sender", no_init)
        .add_property("changed", &CommandSender::hasNewCommand,
                      &CommandSender::setChanged)
        .add_property("command", &CommandSender::getCommand)
        ;

    scope().attr("sender") = cs_pointer;
}

void c_init_sender() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initsender();
    } catch (error_already_set&) {
        PyErr_Print();
    }
}

void set_sender_pointer(shared_ptr<CommandSender> cs_ptr) {
    cs_pointer = cs_ptr;
}
