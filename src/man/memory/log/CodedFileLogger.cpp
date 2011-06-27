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

#include "CodedFileLogger.h"

namespace man {
namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

CodedFileLogger::CodedFileLogger(const FDProvider* fdp,
		int logTypeID, const ProtoMessage* m) :
        FDLogger(fdp), logID(logTypeID), message(m) {

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
    this->writeHead();
}

void CodedFileLogger::writeHead() {
    coded_output->WriteLittleEndian32(logID);
    // this time stamps the log
    coded_output->WriteLittleEndian64(birth_time);
}

void CodedFileLogger::writeToLog() {

    //TODO: can we use cached size here?
    //cout << message->
    coded_output->WriteLittleEndian32(message->ByteSize());
    message->SerializeToCodedStream(coded_output);
}

CodedFileLogger::~CodedFileLogger() {
    //TODO: this might not be flushing properly
    raw_output->Close();
    delete coded_output;
    delete raw_output;
}
}
}
}
