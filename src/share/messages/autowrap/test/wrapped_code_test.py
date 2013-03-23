#!/usr/bin/python

import unittest

from test_proto import *

class TestBoostPyWrapping(unittest.TestCase):

    def setUp(self):
        self.test_message = TestMessage()

    def test_string(self):
        self.test_message.string_test = "string"
        self.assertEqual(self.test_message.string_test, "string")

        self.assertTrue(self.test_message.has_string_test())
        self.test_message.clear_string_test()
        self.assertFalse(self.test_message.has_string_test())

    def test_bytes(self):
        self.test_message.bytes_test = "bytes"
        self.assertEqual(self.test_message.bytes_test, "bytes")

        self.assertTrue(self.test_message.has_bytes_test())
        self.test_message.clear_bytes_test()
        self.assertFalse(self.test_message.has_bytes_test())

    def test_int32(self):
        self.test_message.int32_test = 17
        self.assertEqual(self.test_message.int32_test, 17)

        self.assertTrue(self.test_message.has_int32_test())
        self.test_message.clear_int32_test()
        self.assertFalse(self.test_message.has_int32_test())

    def test_float(self):
        self.test_message.float_test = 0.009999999776482582
        self.assertEqual(self.test_message.float_test, 0.009999999776482582)

        self.assertTrue(self.test_message.has_float_test())
        self.test_message.clear_float_test()
        self.assertFalse(self.test_message.has_float_test())

    def test_enum_declaration(self):
        enum = TestMessage.TestEnum()
        enum = TestMessage.TEST_ENUM_0
        self.assertEqual(enum, TestMessage.TEST_ENUM_0)
        self.assertEqual(enum, TestMessage.TestEnum(0))

    def test_enum_field(self):
        self.test_message.enum_test = TestMessage.TEST_ENUM_0
        self.assertEqual(self.test_message.enum_test, TestMessage.TEST_ENUM_0)

        self.assertTrue(self.test_message.has_enum_test())
        self.test_message.clear_enum_test()
        self.assertFalse(self.test_message.has_enum_test())

    def test_nested_message(self):
        nested_test_message = TestMessage.NestedTestMessage()

    def test_single_message_field(self):
        nested_message_test = self.test_message.nested_message_test
        nested_message_test.string_test = "string"
        self.assertEqual(nested_message_test.string_test, "string")
        self.assertEqual(self.test_message.nested_message_test.string_test, "string")

        self.assertTrue(self.test_message.has_nested_message_test())
        self.test_message.clear_nested_message_test()
        self.assertFalse(self.test_message.has_nested_message_test())

    def test_repeated_message_field(self):
        self.assertEqual(self.test_message.repeated_nested_message_test_size(), 0)

        nested_message_test = self.test_message.add_repeated_nested_message_test()
        nested_message_test.string_test = "string"
        self.assertEqual(nested_message_test.string_test, "string")
        self.assertEqual(self.test_message.repeated_nested_message_test(0).string_test, "string")

        self.assertEqual(self.test_message.repeated_nested_message_test_size(), 1)
        self.test_message.clear_repeated_nested_message_test()
        self.assertEqual(self.test_message.repeated_nested_message_test_size(), 0)

    def test_repeated_int32_field(self):
        self.assertEqual(self.test_message.repeated_int32_test_size(), 0)

        self.test_message.add_repeated_int32_test(17)
        self.assertEqual(self.test_message.repeated_int32_test(0), 17)

        self.test_message.set_repeated_int32_test(0, 23)
        self.assertEqual(self.test_message.repeated_int32_test(0), 23)

        self.assertEqual(self.test_message.repeated_int32_test_size(), 1)
        self.test_message.clear_repeated_int32_test()
        self.assertEqual(self.test_message.repeated_int32_test_size(), 0)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestBoostPyWrapping)
    unittest.TextTestRunner(verbosity=2).run(suite)
