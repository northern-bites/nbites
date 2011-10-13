
#include "FieldViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

FieldViewer::FieldViewer(DataManager::ptr dataManager) :
		QMainWindow(),
		dataManager(dataManager){



}

}
}
