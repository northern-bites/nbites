/*
 * FileLogger.cpp
 *
 *      Author: oneamtu
 */

#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include "FileLogger.hpp"

using namespace std;

FileLogger::FileLogger(char* fileName, ProtoMessage* m) : Logger(m) {
    int file_descriptor = open(fileName,
                               O_WRONLY | O_CREAT,
                               S_IRWXU | S_IRWXG | S_IRWXO);
    if (file_descriptor == -1) {
        cout << "Warning: failed to open " << fileName
                << " for logging" << endl;
    }
    raw_output = new FileOutputStream(file_descriptor);
    coded_output = new CodedOutputStream(raw_output);

    int magic_number = 1234;
    coded_output->WriteLittleEndian32(magic_number);
}

void FileLogger::write() {

    //TODO: can we use cached size here?
    coded_output->WriteVarint32(message->ByteSize());
    message->SerializeToCodedStream(coded_output);
}

FileLogger::~FileLogger() {
    raw_output->Close();
    delete coded_output;
    delete raw_output;
    close(file_descriptor);
}
