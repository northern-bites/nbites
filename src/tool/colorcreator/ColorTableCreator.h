/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 * @withsomehelpfrom Octavian Neamtu
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

/**
 * @class BrushStroke
 *
 * Saves a color paint action (like defining or undefining a color on a region in the image)
 *
 */
class BrushStroke {

public:
    BrushStroke(int x, int y, image::Color::ColorID color, int brushSize, bool define)
        : x(x), y(y), color(color), brushSize(brushSize), define(define) { }

    BrushStroke invert() const { return BrushStroke(x, y, color, brushSize, !define); }

    int x, y;
    image::Color::ColorID color;
    int brushSize;
    bool define;

};

class ColorTableCreator: public QWidget,
                         public portals::Module
{
    Q_OBJECT;

public:
    static const image::Color::ColorID STARTING_COLOR = image::Color::Orange;

    ColorTableCreator(QWidget *parent = 0);

    void paintStroke(const BrushStroke& brushStroke);

    // These are just pointers to the converter modules' InPortals
    portals::InPortal<messages::YUVImage> bottomImageIn;
    portals::InPortal<messages::YUVImage> topImageIn;

protected slots:
    void loadColorTable();
    void saveColorTable();
    void updateThresholdedImage();
    void canvasClicked(int x, int y, int brushSize, bool leftClick);
    void undo();
    void updateColorSelection(int color);
    void updateColorStats();
    void imageTabSwitched(int);

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

    // So that multiple things in this module can connect to these
    portals::OutPortal<messages::YUVImage> bottomImage;
    portals::OutPortal<messages::YUVImage> topImage;

    QLabel* colorStats;
    QComboBox colorSelect;
    int currentColor;

    ColorTable colorTable;

    std::vector<BrushStroke> brushStrokes;
};

class FixedLayout: public QVBoxLayout{
	Q_OBJECT

public:
	QSize sizeHint() const {return QSize(350,500);}
};


}
}
