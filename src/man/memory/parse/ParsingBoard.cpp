#include "ParsingBoard.h"

#include <vector>
#include <iostream>

namespace man {
namespace memory {
namespace parse {

using boost::shared_ptr;
using namespace std;

ParsingBoard::ParsingBoard(Memory::ptr memory,
        IOProvider::const_ptr ioProvider) :
    memory(memory) {
    this->newIOProvider(ioProvider);
}

ParsingBoard::~ParsingBoard(){}

void ParsingBoard::newIOProvider(IOProvider::const_ptr ioProvider) {
    objectParserMap.clear();

    const IOProvider::FDProviderMap* fdmap = ioProvider->getMap();
    for (IOProvider::FDProviderMap::const_iterator i = fdmap->begin();
            i!= fdmap->end(); i++) {

        if (i->first == MIMAGE_ID) {
            shared_ptr<RoboImage> roboImage = memory->getMutableRoboImage();
            objectParserMap[MIMAGE_ID] = Parser::ptr(new ImageParser(i->second,
                    roboImage));
        } else {
            shared_ptr<ProtoMessage> mobject =
                    memory->getMutableProtoMessage(i->first);
            if (mobject != shared_ptr<ProtoMessage>()) {
                objectParserMap[i->first] = Parser::ptr(new MessageParser(i->second,
                        mobject));
            } else {
                std::cout<<"Could not read valid log ID from file descriptor: "
                        << "log ID: " << i->first << " "
                        << i->second->debugInfo() << std::endl;
            }
        }
    }
}

void ParsingBoard::parse(MObject_ID id) {

    ObjectParserMap::iterator it = objectParserMap.find(id);
    // if this is true, then we found a legitimate logger
    // corresponding to our mobject in the map
    if (it != objectParserMap.end()) {
        //it->second is the logger associated with the specified mobject
        it->second->getNext();
    }
}

void ParsingBoard::parseAll() {
    for (ObjectParserMap::iterator it = objectParserMap.begin();
            it != objectParserMap.end(); it++ ) {
        it->second->getNext();
    }

}

//const ImageParser* ParsingBoard::getImageLogger(const MImage* mimage) const {
//    return dynamic_cast<const ImageParser*>(this->getLogger(mimage));
//}

//const Parser* ParsingBoard::getLogger(const MObject* mobject) const {
//    ObjectFDLoggerMap::const_iterator it = objectFDLoggerMap.find(mobject);
//    // if this is true, then we found a legitimate logger
//    // corresponding to our mobject in the map
//    if (it != objectFDLoggerMap.end()) {
//        return it->second;
//    } else {
//        return NULL;
//    }
//}


}
}
}
