#include "DataHandler.h"

namespace qtool {
namespace data {

using memory::Memory;

DataHandler::DataHandler(Memory* memory) :
    memory(memory),
    parsingBoard(memory){
}

}
}
