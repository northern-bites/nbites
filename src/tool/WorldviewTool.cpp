#include "WorldviewTool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

WorldviewTool::WorldviewTool(const char* title) :
    EmptyTool(title),
    worldView(this)
{
    toolTabs->addTab(&worldView, tr("World View"));
}

WorldviewTool::~WorldviewTool() {}

} //namespace tool
