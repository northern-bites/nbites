#include "RobotField.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace image;
using namespace memory;

RobotField::RobotField(DataManager::ptr dataManager, QWidget* parent)
            : QWidget(parent), dataManager(dataManager) {


    PaintField* fieldImage = new PaintField(this);
    PaintRobotWorldState* worldStateOverlay = new PaintRobotWorldState(
            dataManager, this);

    OverlayedImage* combinedImage = new OverlayedImage(fieldImage, worldStateOverlay, this);

    BMPImageViewer* robotFieldViewer = new BMPImageViewer(combinedImage, this);

    //TODO: again double processing
    dataManager->connectSlot(robotFieldViewer, SLOT(updateView()), "MVision");
    dataManager->connectSlot(robotFieldViewer, SLOT(updateView()), "MLocalization");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(robotFieldViewer);
}



}
}
