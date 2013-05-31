/**
 * The main VisionTool class!
 */

#pragma once

#include "EmptyTool.h"

#include "colorcreator/ColorTableCreator.h"

namespace tool {

class VisionTool : public EmptyTool {
    Q_OBJECT;

public:
    VisionTool(const char* title = "VisionTool");
    ~VisionTool();

public slots:
    void setUpModules();

 protected:
    color::ColorTableCreator tableCreator;

};
}
