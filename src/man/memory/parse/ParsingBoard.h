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

#include "io/InProvider.h"
#include "MObjectParser.h"
#include "memory/MObject.h"
#include "memory/Memory.h"
#include "memory/MemoryIOBoard.h"

namespace man {
namespace memory {
namespace parse {

class ParsingBoard : public MemoryIOBoard<MObjectParser> {

public:
    ParsingBoard(Memory::ptr memory);
    virtual ~ParsingBoard();

    void newInputProvider(common::io::InProvider::const_ptr inProvider);

    void parseNext(MObject_ID id);
    void parseNextAll();

    void rewind(MObject_ID id);
    void rewindAll();

private:
    Memory::ptr memory;
};
}
}
}
