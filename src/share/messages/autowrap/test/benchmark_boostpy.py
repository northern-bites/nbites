from test_pb2 import *

for i in range(1, 1000):
	test_message = TestMessage()
	test_message.string_test = "string"
	test_message.string_test == "string"