#include "DataManager.h"

namespace qtool {
namespace data {

using memory::Memory;

DataManager::DataManager() :
    memory(new Memory()),
    dataHandler(new DataHandler(memory)){

}

}
}
