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

#include "include/io/BulkIO.h"
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

typedef std::pair< MObject_ID, Parser::ptr> ObjectParserPair;
typedef std::map< MObject_ID, Parser::ptr> ObjectParserMap;

class ParsingBoard {

private:
    typedef include::io::BulkIO<MObject_ID> IOProvider;

public:
    ParsingBoard(Memory::ptr memory,
            IOProvider::const_ptr ioProvider = IOProvider::NullBulkIO());
    virtual ~ParsingBoard();

    void newIOProvider(IOProvider::const_ptr ioProvider);

    void parse(MObject_ID);
    void parseAll();

protected:
    void initParsingObjects(const IOProvider::FDProviderMap* fdmap);

private:
    Memory::ptr memory;
    ObjectParserMap objectParserMap;

};
}
}
}
