#include "ParsingBoard.h"

#include <vector>
#include <iostream>

namespace man {
namespace memory {
namespace parse {

using boost::shared_ptr;
using namespace std;
using namespace common::io;

ParsingBoard::ParsingBoard(Memory::ptr memory) :
    memory(memory) {
}

ParsingBoard::~ParsingBoard(){}

void ParsingBoard::newInputProvider(InProvider::ptr inProvider, std::string name) {

    if (name == "find_it_out") {
        //warning - if target is a socket, then this might block (potentially
        //forever)
        //TODO: find some way around that (the tricky part is that we use
        // the id we get from reading the log to identify what memory object
        // it's going to be parsed to, so we need to wait on the open is some
        // way)
        try {
            inProvider->openCommunicationChannel();
            MessageHeader header = inProvider->peekAndGet<MessageHeader>();
            name = std::string(header.name);
        } catch (std::exception& e) {
            cerr << e.what() << endl;
            return ;
        }
    }

    try {
        ProtobufMessage::ptr object = memory->getByName(name);
        MessageParser::ptr mObjectParser(new MessageParser(inProvider, object));
        objectIOMap[name] = mObjectParser;
        mObjectParser->start();
    } catch(std::exception& e) {
        cerr << e.what() << endl;
        return ;
    }
}

void ParsingBoard::parseNext(std::string name) {

    ObjectIOMap::iterator it = objectIOMap.find(name);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        it->second->signalToParseNext();
    }
}

void ParsingBoard::parseNextAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->signalToParseNext();
    }

}

void ParsingBoard::parseNextAll(int skips) {
	for (ObjectIOMap::iterator it = objectIOMap.begin();
		 it != objectIOMap.end(); it++ ) {
		for (int i = 0; i < skips; i++)
			it->second->readNextMessage();
    }
}

void ParsingBoard::rewind(std::string name) {

    ObjectIOMap::iterator it = objectIOMap.find(name);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        //it->second is the parser associated with the specified mobject
        it->second->getPrev();
    }
}

void ParsingBoard::rewindAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->getPrev();
    }

}

void ParsingBoard::rewindAll(int skips) {
	for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->getPrev(skips);
    }
}

}
}
}
