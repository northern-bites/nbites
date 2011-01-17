/*
 * FileLogger.hpp
 *
 * this class provides a way to serialize a proto message to a file
 *
 * the raw_output is a ZeroCopyOutputStream, an abstract I/O interface
 * which will minimize the amount of copying to the buffer
 *
 * the coded_output is a CodedOutputStream which warps the ZeroCopyOutputStream and
 * adds some nice features such as being able to ouput variable size integers
 *
 * read more:
 * http://code.google.com/apis/protocolbuffers/docs/reference/cpp/google.protobuf.io.coded_stream.html
 *
 * some of the code is inspired from the example provided in the link
 *
 *      Author: oneamtu
 */

#pragma once

#include <string>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "Logger.hpp"

using namespace google::protobuf::io;
using std::string;

class FileLogger;

class FileLogger : public Logger {

public:
    FileLogger(char* fileName, ProtoMessage* m);
    ~FileLogger();
    void write();

private:
    int file_descriptor;
    FileOutputStream* raw_output;
    CodedOutputStream* coded_output;

};


