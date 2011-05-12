/**
 * CodedFileLogger.cpp
 *
 *  The structure for a log file:
 *  -- ID number for the type of object logged
 *  -- the birth_time timestamp of the man process that parents the logger
 *
 *  -- for each message
 *  ---- size of serialized message
 *  ---- serialized message
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <zlib.h>

#include <google/protobuf/io/gzip_stream.h>

#include "CodedFileLogger.hpp"

namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

CodedFileLogger::CodedFileLogger(string fileName, int logTypeID, ProtoMessage* m) :
        FileLogger(fileName.data(), m) {
//    int file_descriptor = open(fileName.data(),
//                               O_WRONLY | O_CREAT,
//                               S_IRWXU | S_IRWXG | S_IRWXO);
//    if (file_descriptor == -1) {
//        cout << "Warning: failed to open " << fileName
//                << " for logging" << endl;
//    }
    raw_output = new FileOutputStream(file_descriptor);
    //TODO: put the gzip code in a gzip file logger
//    GzipOutputStream::Options opts;
//    opts.format = GzipOutputStream::ZLIB;
//    opts.compression_level = 1;
//    opts.compression_strategy = Z_RLE;
//    opts.buffer_size = 614400;
//    GzipOutputStream* gzip_output = new GzipOutputStream(raw_output, opts);
    coded_output = new CodedOutputStream(raw_output);
    // this helps us ID the log
    coded_output->WriteLittleEndian32(logTypeID);
    // this time stamps the log
    coded_output->WriteLittleEndian64(birth_time);
}

void CodedFileLogger::write() {

    //TODO: can we use cached size here?
    coded_output->WriteVarint32(message->ByteSize());
    message->SerializeToCodedStream(coded_output);
}

CodedFileLogger::~CodedFileLogger() {
    raw_output->Close();
    delete coded_output;
    delete raw_output;
    close(file_descriptor);
}
}
}
