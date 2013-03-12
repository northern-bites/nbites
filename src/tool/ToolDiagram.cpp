#include "ToolDiagram.h"

// @TODO: Make includes and map generation automagic
#include "JointAngles.pb.h"
#include "ButtonState.pb.h"
#include "GameState.pb.h"
#include "SonarState.pb.h"
#include "WorldModel.pb.h"
#include "GCResponse.pb.h"
#include "InertialState.pb.h"
#include "TeamPacket.pb.h"
#include "FootBumperState.pb.h"

namespace tool{

ToolDiagram::ToolDiagram(QWidget* parent) : QObject(parent)
{
    ADD_MAPPED_TYPE(JointAngles);
    ADD_MAPPED_TYPE(ButtonState);
    ADD_MAPPED_TYPE(GameState);
    ADD_MAPPED_TYPE(SonarState);
    ADD_MAPPED_TYPE(WorldModel);
    ADD_MAPPED_TYPE(GCResponse);
    ADD_MAPPED_TYPE(InertialState);
    ADD_MAPPED_TYPE(TeamPacket);
    ADD_MAPPED_TYPE(FootBumperState);
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
