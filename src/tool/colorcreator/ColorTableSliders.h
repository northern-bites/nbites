/**
 * @class ColorTableSliders
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 * @withsomehelpfrom Octavian Neamtu
 *
 * @updated Lizzie Mamantov
 * @date April 2013
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>

#include "RoboGrams.h"
#include "image/ImageConverterModule.h"
#include "image/ImageDisplayModule.h"
#include "Camera.h"
#include "Images.h"
#include "PathConfig.h"

#include "ColorTable.h"

namespace tool {
namespace color {

/*
 * @class ColorTableSliders
 *
 * Uses sliders to define a color table
 */

class ColorTableSliders: public QWidget,
                          public portals::Module
{
    Q_OBJECT;

public:
    static const image::Color::ColorID STARTING_COLOR = image::Color::Orange;

    ColorTableSliders(QWidget *parent = 0);

    // In portals will hook up to unloggers in the ToolDiagram--this module
    // needs the two image inputs
    portals::InPortal<messages::YUVImage> bottomImageIn;
    portals::InPortal<messages::YUVImage> topImageIn;

protected:
    // Implements Module's pure virutal method
    virtual void run_();

protected slots:
    // Color table manipulation
    void loadColorTable();
    void saveColorTable();

    // Listen to color selection by the user
    void updateColorSelection(int color);

    //Update the thresholded images with a color table
    void updateThresholdedImages();

private:
    //// Color Table pieces ////
    ColorTable colorTable;
    // Display stats, current table name (helpful)
    QLabel* colorStats;
    QLabel* colorTableName;

    // Used to change, store which color user is currently working on
    QComboBox colorSelect;
    int currentColor;

    QPushButton loadColorTableBtn, saveColorTableBtn;



    // Keep track of top vs bottom camera
    Camera::Type currentCamera;

    // Main image display
    QTabWidget* imageTabs;

    // This module contains its own diagram! Trippy.
    portals::RoboGram subdiagram;

    // Modules
    man::image::ImageConverterModule topConverter;
    man::image::ImageConverterModule bottomConverter;
    image::ImageDisplayModule topDisplay;
    image::ImageDisplayModule bottomDisplay;
    image::ThresholdedImageDisplayModule topThrDisplay;
    image::ThresholdedImageDisplayModule botThrDisplay;

    // So that multiple things in this module can connect to these--
    // There's no way to just wire up in portals to the outer module's
    // InPortals for Images
    portals::OutPortal<messages::YUVImage> bottomImage;
    portals::OutPortal<messages::YUVImage> topImage;

};

} // namespace color
} // namespace tool
