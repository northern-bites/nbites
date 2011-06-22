/**
 *
 * @class ParsingBoard
 *
 * Keeps track of all parsers for all of the objects of one memory instance
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <map>

#include "include/io/FileFDProvider.h"

#include "MessageParser.h"
#include "ImageParser.h"
#include "memory/MObject.h"

//forward declaration
namespace man {
namespace memory {
namespace parse {
class ParsingBoard;
}
}
}

#include "memory/Memory.h"

namespace man {
namespace memory {
namespace parse {

typedef std::pair< const MObject*, Parser*> ObjectParserPair;
typedef std::pair< const MObject*, FDProvider*> ObjectFDProviderPair;

typedef std::map< const MObject*, Parser*> ObjectParserMap;
typedef std::map< const MObject*, FDProvider*> ObjectFDProviderMap;

class ParsingBoard {

public:
    ParsingBoard(Memory* _memory);
    //TODO: make sure to delete all of the Parser objects
    //~ParsingBoard();

    void parse(const MObject* mobject);
    void parseAll();

    //const ImageParser* getImageParser(const MImage* mimage) const;
//    const Parser* getParser(const MObject* mobject) const;

private:
    void initParsingObjects();

private:
    Memory* memory;
    ObjectParserMap objectParserMap;
    ObjectFDProviderMap objectFDProviderMap;


};
}
}
}
