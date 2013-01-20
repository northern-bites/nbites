#include <boost/python.hpp>
using namespace boost::python;

#include "test.pb.h"

void (TestMessage::*TestMessage_set_string_test)(const ::std::string&) = &TestMessage::set_string_test;
void (TestMessage::*TestMessage_set_bytes_test)(const ::std::string&) = &TestMessage::set_bytes_test;

BOOST_PYTHON_MODULE(test_pb2) {

  class_<TestMessage>("TestMessage")
    .add_property("string_test", make_function(&TestMessage::string_test, return_value_policy<copy_const_reference>()), TestMessage_set_string_test)
    .def("has_string_test", &TestMessage::has_string_test)
    .def("clear_string_test", &TestMessage::clear_string_test)

    .add_property("bytes_test", make_function(&TestMessage::bytes_test, return_value_policy<copy_const_reference>()), TestMessage_set_bytes_test)
    .def("has_bytes_test", &TestMessage::has_bytes_test)
    .def("clear_bytes_test", &TestMessage::clear_bytes_test)

    .add_property("int32_test", &TestMessage::int32_test, &TestMessage::set_int32_test)
    .def("has_int32_test", &TestMessage::has_int32_test)
    .def("clear_int32_test", &TestMessage::clear_int32_test)
  ;
}