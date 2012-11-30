#pragma once

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "man/portals/RoboGrams.h"
#include "man/memory/parse/MessageParser.h"
#include "io/ProtobufMessage.h"
#include "io/FileInProvider.h"

template<class T>

class LogModule : public Module
{
public:
    LogModule(std::string fileName) :
        Module(),
        output(base()),
        incomingMessage(new common::io::TemplatedProtobufMessage<T>("input")),
        parser(boost::shared_ptr<common::io::FileInProvider>
               (new common::io::FileInProvider(fileName)), incomingMessage)
    { parser.openFile(); };

    OutPortal<T> output;

private:
    virtual void run_()
    {
        if(parser.readNextMessage() == false)
        {
            std::cout << "Couldn't read the message." << std::endl;
            return;
        }
        Message<T> outMessage(incomingMessage->get());
        output.setMessage(outMessage);
    };

    boost::shared_ptr<common::io::TemplatedProtobufMessage<T> > incomingMessage;
    man::memory::parse::MessageParser parser;
};
