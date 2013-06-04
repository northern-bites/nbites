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
#include <QtGui>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QToolBar>


#include "RoboGrams.h"
#include "image/ImageDisplayModule.h"
#include "colorcreator/ColorTable.h"
#include "Camera.h"
#include "Images.h"
#include "logview/ProtoViewer.h"
#include "PathConfig.h"

#include "vision/VisionModule.h"

namespace tool {
namespace vision {

class VisionDisplayModule : public QMainWindow,
							public portals::Module

{
	
	Q_OBJECT;

public:
	
	VisionDisplayModule(QWidget *parent = 0);

    // These are just pointers to the converter modules' InPortals
	portals::InPortal<messages::YUVImage> bottomImageIn;
	portals::InPortal<messages::YUVImage> topImageIn;
	
	portals::InPortal<messages::ThresholdImage> tTImage_in;
	portals::InPortal<messages::PackedImage16> tYImage_in;
	portals::InPortal<messages::PackedImage16> tUImage_in;
	portals::InPortal<messages::PackedImage16> tVImage_in;

    portals::InPortal<messages::ThresholdImage> bTImage_in;
	portals::InPortal<messages::PackedImage16> bYImage_in;
	portals::InPortal<messages::PackedImage16> bUImage_in;
	portals::InPortal<messages::PackedImage16> bVImage_in;
													

protected slots:
	

protected:
	virtual void run_();

private:
	
	QImage makeOverlay(Camera::Type which);

	
	QTabWidget* imageTabs;
	Camera::Type currentCamera;
	
    // This module contains its own diagram! Trippy.
	portals::RoboGram subdiagram;
	
	
	// Modules
	image::OverlayDisplayModule topDisplay;
    image::OverlayDisplayModule bottomDisplay;
    image::ThresholdedImageDisplayModule topThrDisplay;
	image::ThresholdedImageDisplayModule botThrDisplay;
	man::vision::VisionModule visMod;
	color::ColorTable colorTable;


	// to view vision data
	logview::TypedProtoViewer<messages::VisionField>* field_viewer;
	logview::TypedProtoViewer<messages::VisionRobot>* robot_viewer;
	logview::TypedProtoViewer<messages::VisionBall>* ball_viewer;
	logview::TypedProtoViewer<messages::VisionObstacle>* obstacle_viewer;

    // So that multiple things in this module can connect to these
    portals::OutPortal<messages::YUVImage> bottomImage;
    portals::OutPortal<messages::YUVImage> topImage;
	portals::OutPortal<messages::ThresholdImage> tTImage;
	portals::OutPortal<messages::PackedImage16> tYImage;
	portals::OutPortal<messages::PackedImage16> tUImage;
	portals::OutPortal<messages::PackedImage16> tVImage;

    portals::OutPortal<messages::ThresholdImage> bTImage;
	portals::OutPortal<messages::PackedImage16> bYImage;
	portals::OutPortal<messages::PackedImage16> bUImage;
	portals::OutPortal<messages::PackedImage16> bVImage;


};

}
}
