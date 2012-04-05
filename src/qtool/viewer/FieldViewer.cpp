
#include "FieldViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;
using namespace image;

FieldViewer::FieldViewer(DataManager::ptr dataManager) :
        QMainWindow(), dataManager(dataManager) {

    paintField = new PaintField(this);
    fieldView = new BMPImageViewer(paintField, this);

    // Adds the Field
    this->setCentralWidget(fieldView);

    fieldView->updateView();
}

}
}
