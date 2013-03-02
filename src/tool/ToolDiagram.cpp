#include "ToolDiagram.h"

#include "JointAngles.pb.h"

namespace tool{

ToolDiagram::ToolDiagram()
{
    ADD_MAPPED_TYPE(JointAngles);
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
    return true;
}

}
