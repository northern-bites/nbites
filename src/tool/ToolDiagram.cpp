#include "ToolDiagram.h"

// @TODO: Make includes and map generation automagic
#include "AudioCommand.pb.h"
#include "BallModel.pb.h"
#include "BatteryState.pb.h"
#include "ButtonState.pb.h"
#include "FallStatus.pb.h"
#include "FeetOnGround.pb.h"
#include "FSR.pb.h"
#include "GameState.pb.h"
#include "GCResponse.pb.h"
#include "Images.h"
#include "InertialState.pb.h"
#include "SonarState.pb.h"
#include "StiffnessControl.pb.h"
#include "TeamPacket.pb.h"
#include "VisionField.pb.h"
#include "VisionRobot.pb.h"
#include "WorldModel.pb.h"

namespace tool{

ToolDiagram::ToolDiagram(QWidget* parent) : QObject(parent)
{
    ADD_MAPPED_TYPE(AudioCommand);
    ADD_MAPPED_TYPE(BatteryState);
    ADD_MAPPED_TYPE(ButtonState);
    ADD_MAPPED_TYPE(FallStatus);
    ADD_MAPPED_TYPE(FeetOnGround);
    ADD_MAPPED_TYPE(FSR);
    ADD_MAPPED_TYPE(GameState);
    ADD_MAPPED_TYPE(GCResponse);
    ADD_MAPPED_TYPE(InertialState);
    ADD_MAPPED_TYPE(SonarState);
    ADD_MAPPED_TYPE(StiffnessControl);
    ADD_MAPPED_TYPE(TeamPacket);
    ADD_MAPPED_TYPE(VisionBall);
    ADD_MAPPED_TYPE(VisionField);
    ADD_MAPPED_TYPE(VisionRobot);
    ADD_MAPPED_TYPE(WorldModel);
    ADD_MAPPED_TYPE(YUVImage);

    diagram = new portals::RoboGram; // on heap so we can delete when new log is loaded
}

bool ToolDiagram::unlogFrom(std::string path)
{
    unlog::UnlogModule<Header> check(path);
    check.openFile();
    Header head = check.readNextMessage();
    check.closeFile();

    if(typeMap.find(head.name()) == typeMap.end())
    {
        std::cout << head.name() << " is not a registered type." << std::endl;
        return false;
    }

    unloggers.push_back(typeMap[head.name()](path));
    diagram->addModule(*unloggers.back());

    unlog::GUI gui = unloggers.back()->makeMyGUI();
    diagram->addModule(*gui.module);
    displays.push_back(gui.module);

    if(head.name() == "messages.YUVImage")
    {
        if(head.has_top_camera() && head.top_camera())
        {
            emit signalNewDisplayWidget(gui.qwidget, "Top Image");
        }
        else
        {
            emit signalNewDisplayWidget(gui.qwidget, "Bottom Image");
        }
    }
    else
    {
        emit signalNewDisplayWidget(gui.qwidget, head.name());
    }

    return true;
}

template<>
bool ToolDiagram::connectToUnlogger(portals::InPortal<messages::YUVImage>& input, std::string name)
{
    for (std::vector<unlog::UnlogBase*>::iterator i = unloggers.begin();
         i != unloggers.end(); i++)
    {
        if((*i)->getType() == "messages.YUVImage")
        {
            if(name == getIdFromPath((*i)->getFilePath()) || name == "none")
            {
                unlog::UnlogModule<messages::YUVImage>* u =
                    dynamic_cast<unlog::UnlogModule<messages::YUVImage>*>(*i);
                input.wireTo(&u->output);
                std::cout << "Connected " << name << " camera input to "
                          << getIdFromPath((*i)->getFilePath())
                          << " camera unlogger." << std::endl;
                return true;
            }
        }
    }

    std::cout << "Tried to connect a module to nonexistent image unlogger!" <<
    std::endl;
    return false;
}

void ToolDiagram::runForward()
{
    unlog::UnlogBase::readBackward = false;
    diagram->run();
}

void ToolDiagram::runBackward()
{
    unlog::UnlogBase::readBackward = true;
    diagram->run();
}

// addUnloggers should only be called once for log folder
// IMPORTANT whenever this is called, all old unloggers and guis are deleted
// MUST follow the once per log folder rule for tool to behave correctly
void ToolDiagram::addUnloggers(std::vector<std::string> paths)
{
    // delete the last set of unloggers
    for (std::vector<unlog::UnlogBase*>::iterator i = unloggers.begin();
         i != unloggers.end(); i++)
    {
        delete *i;
    }
    unloggers.clear();

    // delete the last set of displays and remove all widgets
    for (std::vector<portals::Module*>::iterator i = displays.begin();
         i != displays.end(); i++)
    {
        delete *i;
    }
    displays.clear();
    emit signalDeleteDisplayWidgets();

    // delete old diagram, make new one
    delete diagram;
    diagram = new portals::RoboGram;

    for (std::vector<std::string>::iterator i = paths.begin();
         i != paths.end(); i++)
    {
        if(unlogFrom(*i))
        {
            std::cout << "Created Unlogger for file " <<  *i << std::endl;
        }
    }

    emit signalUnloggersReady();
}

}
