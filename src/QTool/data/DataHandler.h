/**
 * @class DataHandler
 *
 * Abstract class whose purpose is to maintain a parsing board provide a way to
 * poll it for new data
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include "man/memory/parse/ParsingBoard.h"

namespace qtool {
namespace data {

class DataHandler {

public:
    DataHandler(memory::Memory* memory) : memory(memory) {}
    ~DataHandler();

    //virtual void updateMemory() {parsingBoard->parseAll();}

private:
    memory::Memory* memory;
    memory::parse::ParsingBoard* parsingBoard;

};

}
}
