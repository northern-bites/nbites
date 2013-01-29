all:
	protoc --cpp_out=. test.proto
	g++ -fPIC test.pb.cc test_boost.pb.cpp -shared -o test_pb2.so -I/usr/include/python2.7 -lprotobuf -lboost_python -lpython2.7
