#include "ColorCalibrate.h"

namespace qtool {
namespace colorcreator {

using namespace qtool::data;
using namespace qtool::image;

ColorCalibrate::ColorCalibrate(DataManager::ptr dataManager, QWidget *parent) :
        QWidget(parent), dataManager(dataManager),
        image(new BMPYUVImage(dataManager->getMemory()->getMImage())),
        channelImage(image),
        currentColorSpace(&colorSpace[STARTING_COLOR]),
        colorSpaceWidget(currentColorSpace, this),
        colorWheel(currentColorSpace, this) {

    QHBoxLayout* mainLayout = new QHBoxLayout;

    QVBoxLayout* leftLayout = new QVBoxLayout;

    leftLayout->addWidget(&thresholdedImagePlaceholder);
    connect(currentColorSpace, SIGNAL(parametersChanged()),
            this, SLOT(updateThresholdedImage()));
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
            this, SLOT(selectColorSpace(int)));
    rightLayout->addWidget(&colorSelect);

    rightLayout->addWidget(&colorWheel);
    rightLayout->addWidget(&colorSpaceWidget);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    this->setLayout(mainLayout);
}

void ColorCalibrate::selectColorSpace(int index) {
    currentColorSpace = &colorSpace[index];
    colorWheel.setColorSpace(currentColorSpace);
    colorSpaceWidget.setColorSpace(currentColorSpace);
}

// TODO: Ideally we'd want to have this in a separate class
// or unify this with our regular thresholding process (maybe
// by converting the color space parameters to a table continuously?
void ColorCalibrate::updateThresholdedImage() {

    //check for size changes and make sure
    //the thresholded image is the same size as the image
    if (thresholdedImage.width() != image->getWidth()
            || thresholdedImage.height() != image->getHeight()) {
        thresholdedImage = QImage(image->getWidth(),
                                  image->getHeight(),
                                  QImage::Format_RGB32);
    }

    //threshold the image
    thresholdedImage.fill(image::Color_RGB[image::Grey]);
    for (int j = 0; j < thresholdedImage.height(); j++) {
        QRgb* qImageLine = (QRgb*) (thresholdedImage.scanLine(j));
            for (int i = 0; i < thresholdedImage.width(); i++) {
            QRgb rgb = image->getBitmap().pixel(i, j);
            Color color;
            color.setRgb(rgb);
            for (int t = 0; t < image::NUM_COLORS; t++) {
                if (colorSpace[t].contains(color)) {
                    qImageLine[i] = image::Color_RGB[t];
                }
            }
        }
    }
    //set it
    thresholdedImagePlaceholder.setPixmap(QPixmap::fromImage(thresholdedImage));
}


}
}
