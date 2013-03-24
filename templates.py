
HEADER = """\
#include <boost/python.hpp>
using namespace boost::python;

#include "%(proto_header_file)s"

"""

MODULE_DECLARATION = """
BOOST_PYTHON_MODULE(%(module_name)s) {

"""

PACKAGE_SCOPE_DUMMY_CLASS = "class DummyClassFor%(scope_name)s {};\n"
PACKAGE_SCOPE = "  scope %(scope_name)s_scope = class_<DummyClassFor%(scope_name)s>(\"%(scope_name)s\");\n"

# These scopes ensure that nested classes and enums are also
# properly namespaced/scoped in python
MESSAGE_SCOPE = "  {\n"
MESSAGE_DECLARATION = "  scope the_scope = class_<%(scoped_message_name)s>(\"%(message_name)s\")\n"
MESSAGE_DECLARATION_END = "  ;\n\n"
MESSAGE_SCOPE_END = "  }\n"

SINGLE_PRIMITIVE="""\
    .add_property("%(field_name)s", &%(scope)s::%(field_name)s, &%(scope)s::set_%(field_name)s)
    .def("has_%(field_name)s", &%(scope)s::has_%(field_name)s)
    .def("clear_%(field_name)s", &%(scope)s::clear_%(field_name)s)

"""
SINGLE_STRING="""\
    .add_property("%(field_name)s", make_function(&%(scope)s::%(field_name)s, return_value_policy<copy_const_reference>()),(void (%(scope)s::*)(const ::std::string&))(&%(scope)s::set_%(field_name)s))
    .def("has_%(field_name)s", &%(scope)s::has_%(field_name)s)
    .def("clear_%(field_name)s", &%(scope)s::clear_%(field_name)s)

"""
SINGLE_MESSAGE="""\
    .add_property("%(field_name)s", make_function(&%(scope)s::mutable_%(field_name)s, return_value_policy<reference_existing_object>()))
    .def("has_%(field_name)s", &%(scope)s::has_%(field_name)s)
    .def("clear_%(field_name)s", &%(scope)s::clear_%(field_name)s)

"""

REPEATED_PRIMITIVE="""\
    .def("add_%(field_name)s", &%(scope)s::add_%(field_name)s)
    .def("set_%(field_name)s", &%(scope)s::set_%(field_name)s)
    .def("%(field_name)s", (%(field_type)s (%(scope)s::*)(int) const)(&%(scope)s::%(field_name)s))
    .def("%(field_name)s_size", &%(scope)s::%(field_name)s_size)
    .def("clear_%(field_name)s", &%(scope)s::clear_%(field_name)s)

"""
REPEATED_STRING=""#NOT IMPLEMENTED YET!"
REPEATED_MESSAGE="""\
    .def("add_%(field_name)s", &%(scope)s::add_%(field_name)s, return_value_policy<reference_existing_object>())
    .def("%(field_name)s", (%(scope)s::%(field_type)s* (%(scope)s::*)(int))(&%(scope)s::mutable_%(field_name)s), return_value_policy<reference_existing_object>())
    .def("%(field_name)s_size", &%(scope)s::%(field_name)s_size)
    .def("clear_%(field_name)s", &%(scope)s::clear_%(field_name)s)

"""

ENUM_DECLARATION = "  enum_<%(scoped_enum_name)s>(\"%(enum_name)s\")\n"
ENUM_FIELD = "    .value(\"%(enum_field_name)s\", %(enum_field_value)s)\n"
ENUM_DECLARATION_END = "    .export_values()\n  ;\n\n"

FOOTER = "}"
