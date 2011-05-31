/**
 * ZeroCopyFileLogger.cpp
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

#include "Common.h"
#include "ZeroCopyFileLogger.h"

namespace memory {

namespace log {

extern long long birth_time;

using namespace std;

ZeroCopyFileLogger::ZeroCopyFileLogger(string output_file_descriptor,
                                       int logTypeID, ProtoMessage* m) :
        FDLogger(output_file_descriptor.data(), (m)),
        current_buffer(new (void*)),
        current_buffer_size(1),
        bytes_written(0),
        logID(logTypeID)
        {
    raw_output = new FileOutputStream(file_descriptor, m->ByteSize() + 4);
    cout << raw_output->GetErrno() << endl;
    this->getNextBuffer();
    writeHead();
}

void ZeroCopyFileLogger::writeHead() {
//    // this helps us ID the log
//    *((int*) (*current_buffer)) = logID;
//    // this time stamps the log
//    *((long long*) (*current_buffer) + sizeof(int)) = birth_time;
//
//    raw_output->BackUp(current_buffer_size - sizeof(int) - sizeof(long long));
}

void ZeroCopyFileLogger::write() {
//
//    this->getNextBuffer();
//    *((int*) (*current_buffer)) = message->GetCachedSize();
//    message->SerializeToArray(current_buffer + sizeof(int), current_buffer_size - sizeof(int));

}

void ZeroCopyFileLogger::getNextBuffer() {

    raw_output->Next(current_buffer, &current_buffer_size);
    //we're not guaranteed a non-empty buffer
    //but we're guaranteed one eventually
    while (current_buffer_size == 0) {
        raw_output->Next(current_buffer, &current_buffer_size);
    }


}

ZeroCopyFileLogger::~ZeroCopyFileLogger() {

    raw_output->Close();
    delete raw_output;
    close(file_descriptor);
}
}
}
