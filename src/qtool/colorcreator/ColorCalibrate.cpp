#include "ColorCalibrate.h"

namespace qtool {
namespace colorcreator {

using namespace qtool::data;

ColorCalibrate::ColorCalibrate(DataManager::ptr dataManager, QWidget *parent) :
        QWidget(parent), dataManager(dataManager),
        channelImage(dataManager->getMemory()->getMImage()),
        currentColorSpace(&colorSpace[STARTING_COLOR]),
        colorSpaceWidget(currentColorSpace, this),
        colorWheel(currentColorSpace, this) {

    QHBoxLayout* mainLayout = new QHBoxLayout;

    QVBoxLayout* leftLayout = new QVBoxLayout;

    leftLayout->addWidget(&channelImage);

    dataManager->connectSlotToMObject(&channelImage,
                 SLOT(updateView()), MIMAGE_ID);

    QVBoxLayout* rightLayout = new QVBoxLayout;

    //set up the color selection combo box
    for (int i = 0; i < image::NUM_COLORS; i++) {
        colorSelect.addItem(image::Color_label[i].c_str());
    }
    colorSelect.setCurrentIndex(STARTING_COLOR);
    connect(&colorSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectColor(int)));
    rightLayout->addWidget(&colorSelect);

    rightLayout->addWidget(&colorWheel);
    rightLayout->addWidget(&colorSpaceWidget);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    this->setLayout(mainLayout);
}

void ColorCalibrate::selectColor(int index) {
    currentColorSpace = &colorSpace[index];
    colorWheel.setColorSpace(currentColorSpace);
    colorSpaceWidget.setColorSpace(currentColorSpace);
}

}
}
