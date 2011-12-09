#include "BallEKFViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

BallEKFViewer::BallEKFViewer(DataManager::ptr dataManager) :
                QMainWindow(),
                dataManager(dataManager),
                ekfDataViewer(new EKFDataViewer()) {

    this->setCentralWidget(ekfDataViewer);

}

}
}
