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
#include "memory/MemoryCommon.h"

namespace man {
namespace memory {
namespace parse {

class ParsingBoard {

	typedef common::io::InProvider InProvider;
	typedef std::pair< MObject_ID, MObjectParser::ptr > ObjectIOPair;
	typedef std::map< MObject_ID, MObjectParser::ptr > ObjectIOMap;

public:
    ParsingBoard(Memory::ptr memory);
    virtual ~ParsingBoard();

    // warning: this function may block in trying to find out the MObject_ID
    // associated with this input if the MObject_ID is UNKNOWN_OBJECT
    void newInputProvider(common::io::InProvider::ptr inProvider,
                          MObject_ID id = UNKNOWN_OBJECT);

    void parseNext(MObject_ID id);
    void parseNextAll();

    void rewind(MObject_ID id);
    void rewindAll();

    void reset() { objectIOMap.clear(); }

private:
    Memory::ptr memory;
    ObjectIOMap objectIOMap;
};
}
}
}
