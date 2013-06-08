/**
 * The main VisionTool class!
 */

#pragma once

#include "EmptyTool.h"

#include "colorcreator/ColorTable.h"
#include "vision_offline/VisionDisplayModule.h"
#include "colorcreator/ColorTableCreator.h"

#include "image/ImageConverterModule.h"
#include "colorcreator/ColorCalibrate.h"

namespace tool {

class VisionTool : public EmptyTool {
    Q_OBJECT;

public:
    VisionTool(const char* title = "VisionTool");
    ~VisionTool();

public slots:
    void setUpModules();
    void loadColorTable();

 protected:
    color::ColorTableCreator tableCreator;
    vision::VisionDisplayModule visDispMod;

    man::image::ImageConverterModule topConverter;
    man::image::ImageConverterModule bottomConverter;

    colorcreator::ColorCalibrate colorCalibrate;

    color::ColorTable globalColorTable;


};
}
