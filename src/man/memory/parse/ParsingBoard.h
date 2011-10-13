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
#include "MObjectParser.h"
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
#include "memory/MemoryIOBoard.h"

namespace man {
namespace memory {
namespace parse {

class ParsingBoard : public MemoryIOBoard<Parser> {

public:
    ParsingBoard(Memory::ptr memory,
            IOProvider::const_ptr ioProvider = IOProvider::NullBulkIO());
    virtual ~ParsingBoard();

    void newIOProvider(IOProvider::const_ptr ioProvider);

    void parse(MObject_ID id);
    void parseAll();

    void rewind(MObject_ID id);
    void rewindAll();

private:
    Memory::ptr memory;
};
}
}
}
