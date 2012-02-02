/**
 * @class ColorCalibrate
 *
 * Tool to define/calibrate a color table
 *
 * @author Eric Chown
 * @author EJ Googins
 * @author Octavian Neamtu
 */


#pragma once

#include <QWidget>

//qtool
#include "viewer/ChannelImageViewer.h"
#include "data/DataManager.h"
#include "image/BMPYUVImage.h"
//colorcreator
#include "ColorEdit.h"
#include "ColorTable.h"
#include "ColorSpace.h"
#include "ColorSpaceWidget.h"
#include "ColorWheel.h"

namespace qtool {
namespace colorcreator {

class ColorCalibrate : public QWidget
{
    Q_OBJECT

public:
    static const image::ColorID STARTING_COLOR = image::Orange;

public:
    ColorCalibrate(qtool::data::DataManager::ptr dataManager,
            QWidget *parent = 0);
    ~ColorCalibrate() {}

protected slots:
    void selectColorSpace(int index);
    void updateThresholdedImage();

private:
    data::DataManager::ptr dataManager;
    image::BMPYUVImage::ptr image;
    viewer::ChannelImageViewer channelImage;
    ColorSpace colorSpace[image::NUM_COLORS];
    ColorSpace* currentColorSpace;
    QComboBox colorSelect;
    ColorSpaceWidget colorSpaceWidget;
    ColorWheel colorWheel;
    QLabel thresholdedImagePlaceholder;
    QImage thresholdedImage;
    float zSlice;
    int currentColor;

};

}
}
