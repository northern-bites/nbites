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
#include "MessageParser.h"

#include "memory/Memory.h"
#include "memory/MemoryCommon.h"

namespace man {
namespace memory {
namespace parse {

class ParsingBoard {

	typedef common::io::InProvider InProvider;
	typedef std::pair< std::string, MessageParser::ptr > ObjectIOPair;
	typedef std::map< std::string, MessageParser::ptr > ObjectIOMap;

public:
    ParsingBoard(Memory::ptr memory);
    virtual ~ParsingBoard();

    // warning: this function may block in trying to find out the MObject_ID
    // associated with this input if the name is "find_it_out"
    void newInputProvider(common::io::InProvider::ptr inProvider,
                          std::string name = "find_it_out");

    void parseNext(std::string name);
    void parseNextAll();

    void rewind(std::string name);
    void rewindAll();

    void reset() { objectIOMap.clear(); }

private:
    Memory::ptr memory;
    ObjectIOMap objectIOMap;
};
}
}
}
