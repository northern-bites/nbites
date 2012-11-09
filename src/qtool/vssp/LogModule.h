#pragma once

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "man/portals/RoboGrams.h"
#include "man/memory/parse/MessageParser.h"
#include "io/ProtobufMessage.h"
#include "io/FileInProvider.h"
#include "memory/protos/Vision.pb.h"

using man::memory::parse::MessageParser;
using common::io::FileInProvider;
using common::io::TemplatedProtobufMessage;
using man::memory::proto::PVision;
using boost::shared_ptr;

class LogModule : public Module
{
public:
    LogModule(std::string fileName) : Module(),
        output(base()),
        incomingMessage(new TemplatedProtobufMessage<PVision>("input")),
        parser(shared_ptr<FileInProvider>(new FileInProvider(fileName)),
               incomingMessage)
    { parser.openFile(); };

    OutPortal<PVision> output;

private:
    virtual void run_()
    {
        if(parser.readNextMessage() == false)
        {
            std::cout << "Couldn't read the message." << std::endl;
            return;
        }
        Message<PVision> outMessage(incomingMessage->get());
        output.setMessage(outMessage);
        outMessage.get()->PrintDebugString();
    };

    shared_ptr<TemplatedProtobufMessage<PVision> > incomingMessage;
    MessageParser parser;
};
