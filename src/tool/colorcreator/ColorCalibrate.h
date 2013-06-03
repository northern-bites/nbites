/**
 * @class ColorCalibrate
 *
 * Tool to define/calibrate a color table
 *
 * @author Eric Chown
 * @author EJ Googins
 * @author Octavian Neamtu
 * @author Daniel Zeller
 */


#pragma once

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QTabWidget>
#include <QComboBox>
#include <QSizePolicy>

//tool
#include "RoboGrams.h"
#include "image/ImageDisplayModule.h"
#include "image/ImageConverterModule.h"
#include "Camera.h"

#include "image/Color.h"

#include "PathConfig.h"

//colorcreator
#include "ColorEdit.h"
#include "ColorTable.h"
#include "ColorSpace.h"
#include "ColorSpaceWidget.h"
#include "ColorWheel.h"

namespace tool {
namespace colorcreator {

class ColorCalibrate : public QWidget,
                       public portals::Module
{
    Q_OBJECT

    public:
    static const image::Color::ColorID STARTING_COLOR = image::Color::Orange;

public:
    ColorCalibrate(QWidget *parent = 0);
    ~ColorCalibrate() {}

	//Where we actually get the images via the tool
    portals::InPortal<messages::YUVImage> topImageIn;
    portals::InPortal<messages::YUVImage> bottomImageIn;

protected slots:
    void selectColorSpace(int index);
    void updateThresholdedImage();
    void loadSlidersBtnPushed();     
    void saveSlidersBtnPushed();     
    void loadColorTableBtnPushed();
    void saveColorTableBtnPushed();   // Assuming ColorTable has the same functionality
    void imageTabSwitched(int i);    
    void setFullColors(bool state);
	void canvasClicked(int x, int y, int brushSize, bool leftClick);

protected:
    virtual void run_();
    void loadColorSpaces(QString filename);
    void writeColorSpaces(QString filename);

private:
    color::ColorTable colorTable;

    QTabWidget* imageTabs;
    Camera::Type currentCamera;

    portals::RoboGram subdiagram;

	//Display modules for the 4 images
    image::ImageDisplayListener topDisplay;
    image::ImageDisplayListener bottomDisplay;

	//Unfortunately we have to use these intermediary portals because we can't
	//get them directly from the tool
    portals::OutPortal<messages::YUVImage> topImage;
    portals::OutPortal<messages::YUVImage> bottomImage;

    color::ColorSpace colorSpace[image::Color::NUM_COLORS];
    color::ColorSpace* currentColorSpace;
    QComboBox colorSelect;
    ColorSpaceWidget colorSpaceWidget;
    ColorWheel colorWheel;
    QLabel thresholdedImagePlaceholder;
	QImage thresholdedImage;
    QPushButton loadSlidersBtn, saveSlidersBtn, loadColorTableBtn, saveColorTableBtn;

	QHBoxLayout* topImageLayout;
	QHBoxLayout* bottomImageLayout;
    QHBoxLayout* bottomLayout;
    QVBoxLayout* colorButtons;
    QVBoxLayout* leftJunk;
    QVBoxLayout* mainLayout;
    QHBoxLayout* topLayout;

    bool displayAllColors;
};

}
}
