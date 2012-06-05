/**
 * Renders a ThresholdedImage
 * with a combo box for selecting different color filters
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QComboBox>
#include "BMPImageViewer.h"
#include "image/ThresholdedImage.h"

namespace qtool {
namespace viewer {

class FilteredThresholdedViewer: public BMPImageViewer {
    Q_OBJECT;
public:

    FilteredThresholdedViewer(image::ThresholdedImage* image,
                QWidget *parent = NULL);

    virtual ~FilteredThresholdedViewer() {}

public slots:
    void selectionChanged(int i);

protected:
    void setupUI();

protected:
    image::ThresholdedImage* thresholdedImage;
    QComboBox filterSelect;



};

}
}
