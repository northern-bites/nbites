#!/usr/bin/python

import unittest

import parser

class TestProtoParser(unittest.TestCase):

    def test_field(self):
        parse_result = parser.field.parseString("repeated int32 test_field = 1;")

        self.assertEqual(parse_result['flavor'], 'repeated')
        self.assertEqual(parse_result['type'], 'int32')
        self.assertEqual(parse_result['name'], 'test_field')
        self.assertEqual(parse_result['id'], '1')

    def test_field_with_non_primitive_type(self):
        parse_result = parser.field.parseString("required TestMessage test_message = 15;")

        self.assertEqual(parse_result['flavor'], 'required')
        self.assertEqual(parse_result['type'], 'TestMessage')
        self.assertEqual(parse_result['name'], 'test_message')
        self.assertEqual(parse_result['id'], '15')

    def test_default(self):
        self.assertNotEqual(len(parser.default.parseString("[default=3]")), 0)
        self.assertNotEqual(len(parser.default.parseString("[default = 3.32]")), 0)
        self.assertNotEqual(len(parser.default.parseString("[default = \"\\0 poo\"]")), 0)

    def test_field_with_default(self):
        parse_result = parser.field.parseString("optional sint32 test_field = 5 [default=3];")

        self.assertEqual(parse_result['flavor'], 'optional')
        self.assertEqual(parse_result['type'], 'sint32')
        self.assertEqual(parse_result['name'], 'test_field')
        self.assertEqual(parse_result['id'], '5')

    def test_enum_item(self):
        parse_result = parser.enum_item.parseString(" TEST_ENUM = 1; ")

        self.assertEqual(parse_result['name'], 'TEST_ENUM')
        self.assertEqual(parse_result['value'], '1')

    def test_enum(self):
        parse_result = parser.enum.parseString("enum TestEnum { TEST_ENUM_1 = 1; TEST_ENUM_2 = 2; }")

        self.assertEqual(parse_result['enum'], 'enum')
        self.assertEqual(parse_result['name'], 'TestEnum')
        self.assertEqual(parse_result.elements[0]['name'], 'TEST_ENUM_1')
        self.assertEqual(parse_result.elements[0]['value'], '1')
        self.assertEqual(parse_result.elements[1]['name'], 'TEST_ENUM_2')
        self.assertEqual(parse_result.elements[1]['value'], '2')

    def test_package_declaration(self):
        parse_result = parser.package_declaration.parseString("package foo.bar;")

        self.assertEqual(parse_result.scopes[0], 'foo')
        self.assertEqual(parse_result.scopes[1], 'bar')

    def test_recursive_message(self):
        parse_result = parser.message.parseString("message Parent { message Child { } } ")

        self.assertEqual(parse_result['message'], 'message')
        self.assertEqual(parse_result['name'], 'Parent')
        self.assertEqual(parse_result.elements[0]['message'], 'message')
        self.assertEqual(parse_result.elements[0]['name'], 'Child')

    def test_hefty_message(self):
        test_string = (
            "message M {"
                "optional float f = 1;"
                "required int i = 2;"
                "message M2 {}"
                "repeated M2 m2=3;"
                "enum E {}"
                "enum E2 {}"
            "}"
        )

        parse_result = parser.message.parseString(test_string)

        self.assertEqual(parse_result['message'], 'message')
        self.assertEqual(parse_result['name'], 'M')

        float_field = parse_result.elements[0]
        self.assertEqual(float_field['flavor'], 'optional')
        self.assertEqual(float_field['type'], 'float')
        self.assertEqual(float_field['name'], 'f')
        self.assertEqual(float_field['id'], '1')

        int_field = parse_result.elements[1]
        self.assertEqual(int_field['flavor'], 'required')
        self.assertEqual(int_field['type'], 'int')
        self.assertEqual(int_field['name'], 'i')
        self.assertEqual(int_field['id'], '2')

        message = parse_result.elements[2]
        self.assertEqual(message['message'], 'message')
        self.assertEqual(message['name'], 'M2')

        message_field = parse_result.elements[3]
        self.assertEqual(message_field['flavor'], 'repeated')
        self.assertEqual(message_field['type'], 'M2')
        self.assertEqual(message_field['name'], 'm2')
        self.assertEqual(message_field['id'], '3')

        enum = parse_result.elements[4]
        self.assertEqual(enum['enum'], 'enum')
        self.assertEqual(enum['name'], 'E')

        enum2 = parse_result.elements[5]
        self.assertEqual(enum2['enum'], 'enum')
        self.assertEqual(enum2['name'], 'E2')

    def test_messages(self):
        parse_result = parser.proto_file.parseString("message M1 {} message M2 {}")

        self.assertEqual(parse_result[0]['message'], 'message')
        self.assertEqual(parse_result[0]['name'], 'M1')
        self.assertEqual(parse_result[1]['message'], 'message')
        self.assertEqual(parse_result[1]['name'], 'M2')

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestProtoParser)
    unittest.TextTestRunner(verbosity=2).run(suite)
