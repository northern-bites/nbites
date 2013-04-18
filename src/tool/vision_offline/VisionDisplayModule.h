/**
 * @class VisionDisplayModule
 *
 * The Tool's Offline Vision component
 *
 * @author Benjamin Mende
 * @date: April 17, 2013
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QToolBar>


#include "RoboGrams.h"
#include "image/ImageConverterModule.h"
#include "image/ImageDisplayModule.h"
#include "colorcreator/ColorTable.h"
#include "Camera.h"
#include "Images.h"
#include "PathConfig.h"

#include "vision/VisionModule.h"

namespace tool {
namespace vision {

class VisionDisplayModule : public QWidget,
							public portals::Module

{
	
	Q_OBJECT;

public:
	
	VisionDisplayModule(QWidget *parent = 0);

    // These are just pointers to the converter modules' InPortals
	portals::InPortal<messages::YUVImage> bottomImageIn;
	portals::InPortal<messages::YUVImage> topImageIn;

protected slots:
	
	void loadColorTable();

protected:
	virtual void run_();

private:
	
	QTabWidget* imageTabs;
	Camera::Type currentCamera;
	
    // This module contains its own diagram! Trippy.
	portals::RoboGram subdiagram;
	
	
	// Modules
	man::image::ImageConverterModule topConverter;
	man::image::ImageConverterModule bottomConverter;
	image::ImageDisplayListener topDisplay;
    image::ImageDisplayListener bottomDisplay;
    image::ThresholdedImageDisplayModule topThrDisplay;
	image::ThresholdedImageDisplayModule botThrDisplay;
	man::vision::VisionModule visMod;
	color::ColorTable colorTable;

    // So that multiple things in this module can connect to these
    portals::OutPortal<messages::YUVImage> bottomImage;
    portals::OutPortal<messages::YUVImage> topImage;


};

}
}
