/*
 * @class UnlogModule
 * @class UnlogBase
 *
 * The UnlogModule is a templated class that can read from a file and parse
 * the contents of that file into a protobuf of its template type. It is
 * only passed a filename, so another piece of code needs to try to match
 * the file with the correct type of protobuf in order to create the unlogger
 * appropriately.
 * Also provides an option to emit the new message every time we get one so
 * that a display can be updated.
 *
 * While we use AIO for logging, it wouldn't make sense to use the same for
 * the tool, so we use regular (blocking) IO.
 *
 * @author Lizzie Mamantov
 * @date February 2013
 * @author Octavian Neamtu
 * @date 2012  -- Much of the file stuff here was stolen from the previous
 *               version of the NBites logging system.
 */

#pragma once

#include "RoboGrams.h"
#include "LogDefinitions.h"
#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <google/protobuf/message.h>
#include <QObject>

namespace tool {
namespace unlog {

class GenericProviderModule : public QObject, public portals::Module
{
    Q_OBJECT;

public:
    GenericProviderModule(std::string t) : type(t) {}

    void setMessage(google::protobuf::Message* m) { msg = m; }
    google::protobuf::Message* getMessage() { return msg; }
    std::string getType() { return type; }

signals:
    void newMessage(const google::protobuf::Message*);

protected:
    virtual void run_() = 0;
    google::protobuf::Message* msg;
    std::string type;
};

template<class T>
class ProviderModule : public GenericProviderModule
{
public:
    ProviderModule(std::string t) : GenericProviderModule(t) {}

    portals::InPortal<T> input;

protected:
    virtual void run_()
    {
        input.latch();
        msg->CopyFrom(input.message());
        emit newMessage(msg);
    }
};

// Base Class
class UnlogBase : public portals::Module
{
public:
    // The path is expected to be a full path to the log file
    UnlogBase(std::string path, std::string type);
    virtual ~UnlogBase();

    std::string getType() { return typeName; }
    virtual GenericProviderModule* makeMeAProvider() = 0;

    // Reads the next sizeof(T) bytes and interprets them as a T
    template <class T>
    T readValue() {
        T value;
        readCharBuffer((char *)(&value), sizeof(value));
        return value;
    }

    bool& readDir() {
		static bool readDir = 1;
		return readDir;
	}

    // Basic file control
    void openFile() throw (file_exception);
    void closeFile();

protected:
    // Inheriting classes still need to implement this
    virtual void run_() = 0;

    uint32_t readCharBuffer(char* buffer, uint32_t size)
        const throw (file_read_exception);

    // Keeps track of whether the file is open/closed
    bool fileOpen;
    // Pointer to the file
    FILE* file;
    // Stores the full path of the file
    std::string fileName;
    std::string typeName;
    // Keeps track of the sizes of all the reads we've done
    std::vector<uint32_t> messageSizes;
};

// Template Class
template<class T>
class UnlogModule : public UnlogBase
{
public:
    // Takes a file path to parse from
    UnlogModule(std::string path) : UnlogBase(path, T().GetTypeName()),
                                    output(base()) {}

    // Where the output will be provided
    portals::OutPortal<T> output;

    GenericProviderModule* makeMeAProvider()
    {
        ProviderModule<T>* gpm = new ProviderModule<T>(typeName);
        // Have to actually initialize the generic message to correct type
        gpm->setMessage(new T());
        gpm->input.wireTo(&output);
        return gpm;
    }

    T readNextMessage()
    {
        // End of file
        if (feof(file)) {
            std::cout << "End of log file " << fileName << std::endl;
            return T();
        }

        // Read in the next message's size
        // @see LogModule.h for why this works
        uint32_t currentMessageSize = readValue<uint32_t>();

        // Keep track of the sizes we've read (to unwind)
        messageSizes.push_back(currentMessageSize);

       /*
        * Note: We can't deserialize directly from a file to a protobuf.
        * We have to read the file's contents into a buffer, then
        * deserialize from that buffer into the protobuf.
        */

        // To hold the data read, and the number of bytes read
        uint32_t bytes;
        char buffer[currentMessageSize];

        try {
            // Actual file reading call
            bytes = readCharBuffer(buffer, currentMessageSize);
        } catch (std::exception& read_exception) {
            std::cout << read_exception.what() << std::endl;
            return T();
        }

        // If we have actually read some bytes, treat them like a message
        T currentMessage;

        if (bytes) {
            // Parse into the message
            currentMessage.ParseFromString(std::string(buffer, bytes));
            return currentMessage;
        }

        // We read zero bytes at the end of a file w/o hitting feof
        std::cout << "End of log file " << fileName << std::endl;
        return currentMessage;
    }

	T readPrevMessage() {
		std::cout<<"Rewound to file location:"<<ftell(file)<<std::endl;
        if (ftell(file)==0) {
            std::cout << "Beginning of log file " << fileName << std::endl;
            return T();
        }

		////////////////////////
		//////////////TODO
		////////////////////////
	}

protected:
    // Implements the Module run_ method
    void run_()
    {
        // Makes sure the file is available to read
        if (!fileOpen)
        {
            openFile();
            readHeader();
        }

        // Reads the next message from the file and puts it on
        // the OutPortal
		portals::Message<T> msg(0);

        //switch the read direction based on a static bool
		if (readDir()){
			// Reads the next message from the file and puts it on
			// the OutPortal
			*msg.get() = readNextMessage();
			output.setMessage(msg);
		} else {
			// Reads the previous message from the file and puts it on
			// the OutPortal
			*msg.get() = readPrevMessage();
			output.setMessage(msg);
		}
        output.setMessage(msg);
    }

    // Reads in the header; called when the file is first opened
    void readHeader()
    {
        uint32_t currentMessageSize = readValue<uint32_t>();

        // Keep track of the sizes we've read (to unwind)
        messageSizes.push_back(currentMessageSize);

        // To hold the data read, and the number of bytes read
        uint32_t bytes;
        char buffer[currentMessageSize];

        try {
            // Actual file reading call
            bytes = readCharBuffer(buffer, currentMessageSize);
        } catch (std::exception& read_exception) {
            std::cout << read_exception.what() << std::endl;
            return;
        }

        // If we have actually read some bytes, treat them like a message
        Header header;

        if (bytes) {
            // Parse into the message
            header.ParseFromString(std::string(buffer, bytes));
        }
    }

};

}
}
