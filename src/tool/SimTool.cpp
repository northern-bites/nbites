#include "SimTool.h"

namespace tool {

SimulationTool::SimulationTool(const char* title) : EmptyTool(title),
                                                    sim()
{
    toolTabs->addTab(sim.getGUI(), tr("Vision Sim"));
}

void SimulationTool::setUpModules()
{
    // The sim module is completely self-contained at the moment so
    // this is a little silly. It shouldn't be so forever, though.
    diagram.addModule(sim);
}

}
