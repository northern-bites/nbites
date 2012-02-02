
#include "ChannelImageViewer.h"

namespace qtool {
namespace viewer {

ChannelImageViewer::ChannelImageViewer(image::BMPYUVImage::ptr image,
                                       QWidget *parent)
    : BMPImageViewer(image, parent), bmpyuvimage(image) {

    for (int i = 0; i < image::NUM_CHANNELS; i++) {
        channelSelect.addItem(image::ChannelType_label[i].c_str(), QVariant(i));
    }

    QLayout* layout = this->layout();
    layout->addWidget(&channelSelect);

    connect(&channelSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectionChanged(int)));
}

void ChannelImageViewer::selectionChanged(int i) {
    bmpyuvimage->setBitmapType((image::ChannelType) i);
    this->updateView();
}

}
}
