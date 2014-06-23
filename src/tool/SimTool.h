#pragma once

#include "EmptyTool.h"

#include "sim/VisionSimModule.h"

namespace tool {

class SimulationTool : public EmptyTool
{
    Q_OBJECT;

public:
    SimulationTool(const char* title = "The Vision Simulator");

public slots:
    void setUpModules();

protected:
    visionsim::VisionSimModule sim;
};

}
