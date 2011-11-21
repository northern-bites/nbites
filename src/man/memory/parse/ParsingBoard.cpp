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

void ParsingBoard::newInputProvider(InProvider::const_ptr inProvider) {
    MObjectParser::ptr mObjectParser(new MObjectParser(inProvider));
    //TODO
    MObject_ID id = mObjectParser->getHeader().log_id;
    if (0 < id && id < LAST_OBJECT_ID) {
        mObjectParser->setObjectToParseTo(memory->getMutableMObject(id));
        objectIOMap[id] = mObjectParser;
    } else {
        cout<<"Could not read valid log ID from input: "
            << inProvider->debugInfo() << endl;
    }
}

void ParsingBoard::parseNext(MObject_ID id) {

    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        //it->second is the parser associated with the specified mobject
        it->second->getNext();
    }
}

void ParsingBoard::parseNextAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->getNext();
    }

}

void ParsingBoard::rewind(MObject_ID id) {

    ObjectIOMap::iterator it = objectIOMap.find(id);
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

}
}
}
