#include "ColorCalibrate.h"

namespace qtool {
namespace colorcreator {

using namespace qtool::data;

ColorCalibrate::ColorCalibrate(DataManager::ptr dataManager, QWidget *parent) :
        QWidget(parent), dataManager(dataManager),
        currentColorSpace(&colorSpace[STARTING_COLOR]),
        colorSpaceWidget(currentColorSpace, this),
        colorWheel(currentColorSpace, this) {
    QHBoxLayout* mainLayout = new QHBoxLayout;

    mainLayout->addWidget(&colorWheel);
    mainLayout->addWidget(&colorSpaceWidget);

    this->setLayout(mainLayout);
}

}
}
