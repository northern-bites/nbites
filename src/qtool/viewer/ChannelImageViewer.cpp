
#include "ChannelImageViewer.h"

namespace qtool {
namespace viewer {

using namespace image;
using namespace man::memory;

ChannelImageViewer::ChannelImageViewer(BMPYUVImage* image,
                                       QWidget *parent)
    : BMPImageViewerListener(image, parent), bmpyuvimage(image) {

    setupUI();
}

void ChannelImageViewer::selectionChanged(int i) {
    bmpyuvimage->setBitmapType((BMPYUVImage::ChannelType) i);
    this->updateView();
}

void ChannelImageViewer::setupUI() {
	this->updateView();
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
