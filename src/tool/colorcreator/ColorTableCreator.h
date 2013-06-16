/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 * @withsomehelpfrom Octavian Neamtu
 *
 * @updated Lizzie Mamantov
 * @date April 2013
 *
 * @update Benjamin Mende
 * @date June 2013
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

struct colorChanges {
    byte y, u, v, color;
};

/**
 * @class BrushStroke
 *
 * Saves a color paint action (like defining or undefining a color
 * on a region in the image)
 *
 */
class BrushStroke {

public:
    BrushStroke(int x, int y, image::Color::ColorID color,
                int brushSize, bool define)
        : x(x),
          y(y),
          color(color),
          brushSize(brushSize),
          define(define) { }

    BrushStroke invert() const { return BrushStroke(x, y, color,
                                                    brushSize, !define); }

    int x, y;
    image::Color::ColorID color;
    int brushSize;
    bool define;
};

/*
 * @class ColorTableCreator
 *
 * Main widget for the "click on images" part of color table creation.
 * Has tabs to display the two images, and the mouse over these images
 * displays which color you're defining in a variable-size cursor box.
 * Clicking on the image causes the color table to make the pixels under
 * the cursor the desired color. The current state of the color table
 * is shown with a thresholded image display, which shows what the current
 * image looks like thresholded. Also clearly provides buttons for choosing
 * which color we're defining, loading a color table, etc.
 * Is both a QWidget (so that it can be a GUI) and a Module (so that we can
 * connect it to non-GUI modules underlying its functionality).
 */

class ColorTableCreator: public QWidget,
                          public portals::Module
{
    Q_OBJECT;

public:
    static const image::Color::ColorID STARTING_COLOR = image::Color::Orange;

    ColorTableCreator(QWidget *parent = 0);

    // In portals will hook up to unloggers in the ToolDiagram--this module
    // needs the two image inputs
    portals::InPortal<messages::YUVImage> bottomImageIn;
    portals::InPortal<messages::YUVImage> topImageIn;
    portals::InPortal<messages::ThresholdImage> topThrIn;
    portals::InPortal<messages::ThresholdImage> botThrIn;


signals:
    // Color table manipulation
    void tableChanges(std::vector<color::colorChanges> tableAdjustments);
    void tableUnChanges(std::vector<color::colorChanges> tableAdjustments);


protected slots:

    // Re-threshold the thresholded image
    void updateThresholdedImage();

    // Register a click on one of the images
    void canvasClicked(int x, int y, int brushSize, bool leftClick);

    // Undo an unwanted click
    void undo();

    // Listen to color selection by the user
    void updateColorSelection(int color);

    // Listen if the user switches which image she's working on
    void imageTabSwitched(int index);

protected:
    // Implements Module's pure virutal method
    virtual void run_();

    // Called when a click is made
    void paintStroke(const BrushStroke& brushStroke);


private:
    // For the tool, we have an actual ColorTable class that does a lot
    // of useful things
    // @see ColorTable.h
    ColorTable colorTable;

    QLabel* colorTableName;

    // Used to change, store which color user is currently working on
    QComboBox colorSelect;
    int currentColor;

    // Keep track of top vs bottom camera
    Camera::Type currentCamera;

    // Most recent brushstrokes--for undoing
    std::vector<BrushStroke> brushStrokes;

    // Main image display
    QTabWidget* imageTabs;

    // This module contains its own diagram! Trippy.
    portals::RoboGram subdiagram;

    // Modules
    image::ImageDisplayListener topDisplay;
    image::ImageDisplayListener bottomDisplay;
    image::ThresholdedImageDisplayModule thrDisplay;

    // So that multiple things in this module can connect to these--
    // There's no way to just wire up in portals to the outer module's
    // InPortals for Images
    portals::OutPortal<messages::YUVImage> bottomImage;
    portals::OutPortal<messages::YUVImage> topImage;
    portals::OutPortal<messages::ThresholdImage> topThrImage;
    portals::OutPortal<messages::ThresholdImage> botThrImage;
};

}
}
