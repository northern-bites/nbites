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

//qtool
//#include "viewer/ChannelImageViewer.h"
/* #include "data/DataManager.h" */
/* #include "image/BMPYUVImage.h" */
//#include "viewer/BMPImageViewerListener.h"
//colorcreator
/* #include "ColorEdit.h" */
/* #include "ColorTable.h" */
/* #include "ColorSpace.h" */
/* #include "ColorSpaceWidget.h" */
/* #include "ColorWheel.h" */

namespace tool {
namespace colorcreator {

class ColorCalibrate : public QWidget
{
    Q_OBJECT

public:
  //    static const image::ColorID STARTING_COLOR = image::Orange;

public:
    /*   ColorCalibrate(qtool::data::DataManager::ptr dataManager, */
  ColorCalibrate(QWidget *parent = 0);
	~ColorCalibrate() {}

protected slots:
    //void selectColorSpace(int index);
    //void updateThresholdedImage();
    void loadSlidersBtnPushed();
    void saveSlidersBtnPushed();
    //void saveColorTableBtnPushed();
    //void imageTabSwitched(int);
	//void setFullColors(bool state);
    //void canvassClicked(int x, int y, int brushSize, bool leftClick);


protected:
    void loadColorSpaces(QString filename);
    void writeColorSpaces(QString filename);

private:
    /* data::DataManager::ptr dataManager; */

     QTabWidget* imageTabs;
    /* man::corpus::Camera::Type currentImage; */

    /* image::BMPYUVImage* topImage; */
    /* viewer::ChannelImageViewer topChannelImage; */
    /* viewer::BMPImageViewerListener* topImageViewer; */

    /* image::BMPYUVImage* bottomImage; */
    /* viewer::ChannelImageViewer bottomChannelImage; */
    /* viewer::BMPImageViewerListener* bottomImageViewer; */

    /* ColorSpace colorSpace[image::NUM_COLORS]; */
    /* ColorSpace* currentColorSpace; */
     QComboBox colorSelect;
    /* ColorSpaceWidget colorSpaceWidget; */
    /* ColorWheel colorWheel; */
    QLabel thresholdedImagePlaceholder;
    QImage thresholdedImage;
    QPushButton loadSlidersBtn, saveSlidersBtn, saveColorTableBtn;

	QHBoxLayout* bottomLayout;
	QVBoxLayout* colorButtons;
	QVBoxLayout* leftJunk;
	QVBoxLayout* mainLayout;
    QHBoxLayout* topLayout;

	//Sliders & stuff for tweaking the color table
	QSlider* hMin;
	QSlider* hMax;
	QSlider* sMin;
	QSlider* sMax;
	QSlider* zMin;
	QSlider* zMax;
	int hLow, hHigh, sLow, sHigh, zLow, zHigh;

	bool displayAllColors;
	int lastClickedX, lastClickedY;

};

}
}
