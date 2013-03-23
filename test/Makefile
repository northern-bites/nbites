all: test_proto.so

test_proto.so: test.proto ../*.py
	protoc --cpp_out=. test.proto
	../wrapifier.py test.proto
	g++ -fPIC test.pb.cc test.boost.pb.cpp -shared -o test_proto.so -I/usr/include/python2.7 -lprotobuf -lboost_python -lpython2.7

clean:
	$(RM) *.pyc *.pb.cpp *.pb.h *.so
