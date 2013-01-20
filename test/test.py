#!/usr/bin/python

import unittest

from test_pb2 import *

class TestSequenceFunctions(unittest.TestCase):

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

if __name__ == '__main__':
    unittest.main()