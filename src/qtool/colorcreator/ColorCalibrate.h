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
#include <QPushButton>

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
    void loadSlidersBtnPushed();
    void saveSlidersBtnPushed();
    void saveColorTableBtnPushed();
    void imageTabSwitched(int);
	void setFullColors(bool state);

protected:
    void loadColorSpaces(QString filename);
    void writeColorSpaces(QString filename);

private:
    data::DataManager::ptr dataManager;

    QTabWidget* imageTabs;
    man::corpus::Camera::Type currentImage;

    image::BMPYUVImage* topImage;
    viewer::ChannelImageViewer topChannelImage;

    image::BMPYUVImage* bottomImage;
    viewer::ChannelImageViewer bottomChannelImage;

    ColorSpace colorSpace[image::NUM_COLORS];
    ColorSpace* currentColorSpace;
    QComboBox colorSelect;
    ColorSpaceWidget colorSpaceWidget;
    ColorWheel colorWheel;
    QLabel thresholdedImagePlaceholder;
    QImage thresholdedImage;
    QPushButton loadSlidersBtn, saveSlidersBtn, saveColorTableBtn;

	bool displayAllColors;

};

}
}
