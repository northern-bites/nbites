#include "LocTool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

LocTool::LocTool(const char* title) :
    fieldView(this),
    EmptyTool(title)

{
    toolTabs->addTab(&fieldView, tr("FieldView"));

    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));
}

LocTool::~LocTool() {

}

void LocTool::setUpModules()
{
    /** FieldViewer Tab **/
    // Should add field view
    bool shouldAddFieldView = false;
    if(diagram.connectToUnlogger<messages::RobotLocation>(fieldView.locationIn,
                                                          "location"))
    {
        fieldView.confirmLocationLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: location wasn't logged in this file" << std::endl;
    }
    if(diagram.connectToUnlogger<messages::RobotLocation>(fieldView.odometryIn,
                                                          "odometry"))
    {
        fieldView.confirmOdometryLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: odometry wasn't logged in this file" << std::endl;
    }

    if(diagram.connectToUnlogger<messages::ParticleSwarm>(fieldView.particlesIn,
                                                          "particleSwarm"))
    {
        fieldView.confirmParticleLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: Particles were'nt logged in this file" << std::endl;
    }
    if(diagram.connectToUnlogger<messages::VisionField>(fieldView.observationsIn,
                                                        "observations"))
    {
        fieldView.confirmObsvLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: Observations were'nt logged in this file" << std::endl;
    }
    if(shouldAddFieldView)
        diagram.addModule(fieldView);


}

}
