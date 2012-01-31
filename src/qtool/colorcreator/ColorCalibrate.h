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
#include "image/BMPYUVImage.h"
#include "data/DataManager.h"
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
    enum Colors {Orange, Blue, Yellow, Green, White, Pink, Navy, Black,
        BlueGreen, BlueNavy, OrangeRed, NUM_COLORS};
    static const Colors STARTING_COLOR = Orange;

public:
    ColorCalibrate(qtool::data::DataManager::ptr dataManager,
            QWidget *parent = 0);
    ~ColorCalibrate() {}


private:
    qtool::data::DataManager::ptr dataManager;
    ColorSpace colorSpace[NUM_COLORS];
    ColorSpace* currentColorSpace;
    ColorSpaceWidget colorSpaceWidget;
    ColorWheel colorWheel;
    float zSlice;
    int currentColor;

};

}
}
