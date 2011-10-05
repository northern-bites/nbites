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

//TODO: this could be moved to MemoryIOBoard, since it's very similar to
// the LoggingBoard method
void ParsingBoard::newIOProvider(IOProvider::const_ptr ioProvider) {
    objectIOMap.clear();

    const IOProvider::FDProviderMap* fdmap = ioProvider->getMap();
    for (IOProvider::FDProviderMap::const_iterator i = fdmap->begin();
            i!= fdmap->end(); i++) {
        MObject::ptr mobject =
                memory->getMutableMObject(i->first);

        if (mobject != MObject::ptr()) {
            objectIOMap[i->first] = Parser::ptr(
                    new MObjectParser(i->second, mobject));
        } else {
            std::cout<<"Could not read valid log ID from file descriptor: "
                    << "log ID: " << i->first << " "
                    << i->second->debugInfo() << std::endl;
        }
    }
}

void ParsingBoard::parse(MObject_ID id) {

    ObjectIOMap::iterator it = objectIOMap.find(id);
    // if this is true, then we found a legitimate parser
    // corresponding to our mobject in the map
    if (it != objectIOMap.end()) {
        //it->second is the parser associated with the specified mobject
        it->second->getNext();
    }
}

void ParsingBoard::parseAll() {
    for (ObjectIOMap::iterator it = objectIOMap.begin();
            it != objectIOMap.end(); it++ ) {
        it->second->getNext();
    }

}


}
}
}
