#include "DataManager.h"

namespace qtool {
namespace data {

using man::memory::Memory;
using boost::shared_ptr;

DataManager::DataManager() :
    memory(shared_ptr<Memory>(new Memory())),
    dataHandler(new DataHandler(memory.get())){

}

}
}
