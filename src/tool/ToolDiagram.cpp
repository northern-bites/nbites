#include "ToolDiagram.h"

// @TODO: Make includes and map generation automagic
#include "AudioCommand.pb.h"
#include "BatteryState.pb.h"
#include "ButtonState.pb.h"
#include "FallStatus.pb.h"
#include "FeetOnGround.pb.h"
#include "FootBumperState.pb.h"
#include "FSR.pb.h"
#include "GameState.pb.h"
#include "GCResponse.pb.h"
#include "InertialState.pb.h"
#include "InitialState.pb.h"
#include "JointAngles.pb.h"
#include "SonarState.pb.h"
#include "StiffnessControl.pb.h"
#include "TeamPacket.pb.h"
#include "VisionBall.pb.h"
#include "VisionField.pb.h"
#include "VisionRobot.pb.h"
#include "WorldModel.pb.h"

namespace tool{

ToolDiagram::ToolDiagram(QWidget* parent) : QObject(parent),
                                            viewTab(parent)
{
    ADD_MAPPED_TYPE(AudioCommand);
    ADD_MAPPED_TYPE(BatteryState);
    ADD_MAPPED_TYPE(ButtonState);
    ADD_MAPPED_TYPE(FallStatus);
    ADD_MAPPED_TYPE(FeetOnGround);
    ADD_MAPPED_TYPE(FootBumperState);
    ADD_MAPPED_TYPE(FSR);
    ADD_MAPPED_TYPE(GameState);
    ADD_MAPPED_TYPE(GCResponse);
    ADD_MAPPED_TYPE(InertialState);
    ADD_MAPPED_TYPE(InitialState);
    ADD_MAPPED_TYPE(JointAngles);
    ADD_MAPPED_TYPE(SonarState);
    ADD_MAPPED_TYPE(StiffnessControl);
    ADD_MAPPED_TYPE(TeamPacket);
    ADD_MAPPED_TYPE(VisionBall);
    ADD_MAPPED_TYPE(VisionField);
    ADD_MAPPED_TYPE(VisionRobot);
    ADD_MAPPED_TYPE(WorldModel);
}

bool ToolDiagram::unlogFrom(std::string path)
{
    unlog::UnlogModule<Header> check(path);
    check.openFile();
    Header head = check.readNextMessage<Header>();
    check.closeFile();

    if(typeMap.find(head.name()) == typeMap.end())
    {
        std::cout << head.name() << " is not a registered type." << std::endl;
        return false;
    }

    unloggers.push_back(typeMap[head.name()](path));
    diagram.addModule(*unloggers.back());
    unloggers.back()->run();
    viewTab.addProtoViewer(unloggers.back());
    return true;
}

void ToolDiagram::addUnloggers(std::vector<std::string> paths)
{
    for (std::vector<std::string>::iterator i = paths.begin();
         i != paths.end(); i++)
    {
        if(unlogFrom(*i))
        {
            std::cout << "Created Unlogger for file " <<  *i << std::endl;
        }
    }
}
}
