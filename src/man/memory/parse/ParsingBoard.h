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
namespace memory {
namespace parse {
class ParsingBoard;
}
}

#include "memory/Memory.h"

namespace memory {
namespace parse {

typedef pair< const MObject*, Parser*> ObjectParserPair;
typedef pair< const MObject*, FDProvider*> ObjectFDProviderPair;

typedef map< const MObject*, Parser*> ObjectParserMap;
typedef map< const MObject*, FDProvider*> ObjectFDProviderMap;

class LoggingBoard {

public:
    LoggingBoard(const Memory* _memory);
    //TODO: make sure to delete all of the Parser objects
    //~LoggingBoard();

    void parse(const MObject* mobject);

    //const ImageParser* getImageParser(const MImage* mimage) const;
    const Parser* getParser(const MObject* mobject) const;

private:
    void initParsingObjects();

private:
    const Memory* memory;
    ObjectParserMap objectParserMap;
    ObjectFDProviderMap objectFDProviderMap;


};
}
}
