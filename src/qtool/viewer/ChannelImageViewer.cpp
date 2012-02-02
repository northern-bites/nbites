
#include "ChannelImageViewer.h"

namespace qtool {
namespace viewer {

using namespace image;
using namespace man::memory;

ChannelImageViewer::ChannelImageViewer(BMPYUVImage::ptr image,
                                       QWidget *parent)
    : BMPImageViewer(image, parent), bmpyuvimage(image) {

    setupUI();
}

ChannelImageViewer::ChannelImageViewer(MImage::const_ptr mImage,
                                       QWidget *parent)
    : BMPImageViewer(BMPYUVImage::ptr(new BMPYUVImage(mImage)), parent) {

    //we're certain that image in BMPImageViewer is a BMPYUVImage
    //because we're passing it
    bmpyuvimage = BMPYUVImage::ptr(dynamic_cast<BMPYUVImage*>(image.get()));
    setupUI();
}

void ChannelImageViewer::selectionChanged(int i) {
    bmpyuvimage->setBitmapType((BMPYUVImage::ChannelType) i);
    this->updateView();
}

void ChannelImageViewer::setupUI() {
    for (int i = 0; i < image::BMPYUVImage::NUM_CHANNELS; i++) {
        channelSelect.addItem(image::ChannelType_label[i].c_str(), QVariant(i));
    }

    QLayout* layout = this->layout();
    layout->addWidget(&channelSelect);

    connect(&channelSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectionChanged(int)));
}

}
}
