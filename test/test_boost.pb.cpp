#include <boost/python.hpp>
using namespace boost::python;

#include "test.pb.h"

// typecasting the signature so that the C++ compiler knows
// which function we're referencing out of the multiple
// set_xxx methods with the same name
void (TestMessage::*TestMessage_set_string_test)(const ::std::string&) = &TestMessage::set_string_test;
void (TestMessage::*TestMessage_set_bytes_test)(const ::std::string&) = &TestMessage::set_bytes_test;
void (TestMessage::NestedTestMessage::*TestMessage_NestedTestMessage_set_string_test)(const ::std::string&) = &TestMessage::NestedTestMessage::set_string_test;
::TestMessage_NestedTestMessage* (TestMessage::*TestMessage_repeated_nested_message_test_from_index)(int) = &TestMessage::mutable_repeated_nested_message_test;
::google::protobuf::int32 (TestMessage::*TestMessage_repeated_int32_test_from_index)(int) const = &TestMessage::repeated_int32_test;

BOOST_PYTHON_MODULE(test_pb2) {

  scope the_scope = class_<TestMessage>("TestMessage")
    .add_property("string_test", make_function(&TestMessage::string_test, return_value_policy<copy_const_reference>()), TestMessage_set_string_test)
    .def("has_string_test", &TestMessage::has_string_test)
    .def("clear_string_test", &TestMessage::clear_string_test)

    .add_property("bytes_test", make_function(&TestMessage::bytes_test, return_value_policy<copy_const_reference>()), TestMessage_set_bytes_test)
    .def("has_bytes_test", &TestMessage::has_bytes_test)
    .def("clear_bytes_test", &TestMessage::clear_bytes_test)

    .add_property("int32_test", &TestMessage::int32_test, &TestMessage::set_int32_test)
    .def("has_int32_test", &TestMessage::has_int32_test)
    .def("clear_int32_test", &TestMessage::clear_int32_test)

    .add_property("float_test", &TestMessage::float_test, &TestMessage::set_float_test)
    .def("has_float_test", &TestMessage::has_float_test)
    .def("clear_float_test", &TestMessage::clear_float_test)

    .add_property("enum_test", &TestMessage::enum_test, &TestMessage::set_enum_test)
    .def("has_enum_test", &TestMessage::has_enum_test)
    .def("clear_enum_test", &TestMessage::clear_enum_test)

    .add_property("nested_message_test", make_function(&TestMessage::mutable_nested_message_test, return_value_policy<reference_existing_object>()))
    .def("has_nested_message_test", &TestMessage::has_nested_message_test)
    .def("clear_nested_message_test", &TestMessage::clear_nested_message_test)

    .def("add_repeated_nested_message_test", &TestMessage::add_repeated_nested_message_test, return_value_policy<reference_existing_object>())
    .def("repeated_nested_message_test", TestMessage_repeated_nested_message_test_from_index, return_value_policy<reference_existing_object>())
    .def("repeated_nested_message_test_size", &TestMessage::repeated_nested_message_test_size)
    .def("clear_repeated_nested_message_test", &TestMessage::clear_repeated_nested_message_test)

    .def("add_repeated_int32_test", &TestMessage::add_repeated_int32_test)
    .def("set_repeated_int32_test", &TestMessage::set_repeated_int32_test)
    .def("repeated_int32_test", TestMessage_repeated_int32_test_from_index)
    .def("repeated_int32_test_size", &TestMessage::repeated_int32_test_size)
    .def("clear_repeated_int32_test", &TestMessage::clear_repeated_int32_test)
  ;

  enum_<TestMessage::TestEnum>("TestEnum")
    .value("TEST_ENUM_0", TestMessage::TEST_ENUM_0)
    .value("TEST_ENUM_1", TestMessage::TEST_ENUM_1)
    .value("TEST_ENUM_2", TestMessage::TEST_ENUM_2)
    .export_values()
  ;

  class_<TestMessage::NestedTestMessage>("NestedTestMessage")
    .add_property("string_test", make_function(&TestMessage::NestedTestMessage::string_test, return_value_policy<copy_const_reference>()), TestMessage_NestedTestMessage_set_string_test)
    .def("has_string_test", &TestMessage::NestedTestMessage::has_string_test)
    .def("clear_string_test", &TestMessage::NestedTestMessage::clear_string_test)
  ;
}