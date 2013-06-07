
#include "FilteredThresholdedViewer.h"

namespace qtool {
namespace viewer {

using namespace image;
using namespace man::memory;

FilteredThresholdedViewer::FilteredThresholdedViewer(ThresholdedImage* image,
                                           QWidget *parent)
    : BMPImageViewer(image, parent), thresholdedImage(image) {

    setupUI();
}

void FilteredThresholdedViewer::selectionChanged(int i) {
    if (i != image::NUM_COLORS) {
        thresholdedImage->setFilter(image::Color_bits[i]);
    } else {
        thresholdedImage->setFilter(image::ALL_COLORS);
    }
}

void FilteredThresholdedViewer::setupUI() {
    for (int i = 0; i < image::NUM_COLORS; i++) {
        filterSelect.addItem(image::Color_label[i].c_str(), QVariant(i));
    }

    filterSelect.addItem("All Colors", QVariant(NUM_COLORS));

    QLayout* layout = this->layout();
    layout->addWidget(&filterSelect);

    connect(&filterSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectionChanged(int)));
}

}
}
