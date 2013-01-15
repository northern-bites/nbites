/*
 * LogModule : Reads a log and returns a PB message as the output of an
 *             OutPortal. Can be used to connect our logs to the new portals
 *             system.
 *
 * Usage : LogModule<PSomeMessage> logMod("filename");
 *         // someOtherModule has an InPortal<PSomeMessage> //
 *         someOtherModule.inPortal.wireTo(logMod.output);
 *         RoboGram aDiagram;
 *         aDiagram.addModule(logMod);
 *         aDiagram.addModule(someOtherModule);
 *         diagram.run()
 *
 * Notes : No guarantee of the behavior when it reaches the end of a file.
 *         Apparently if a message is invalid it doesn't crash though.
 *         Depends on the underlying MessageParser, which may do things of
 *         its own!
 */

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
            std::cout << "Couldn't read a message." << std::endl;
            return;
        }
        Message<T> outMessage(incomingMessage->get());
        output.setMessage(outMessage);
    };

    boost::shared_ptr<common::io::TemplatedProtobufMessage<T> > incomingMessage;
    man::memory::parse::MessageParser parser;
};
