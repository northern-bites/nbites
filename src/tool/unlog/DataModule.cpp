#include "DataModule.h"
#include <iostream>

namespace tool{
namespace unlog {

DataModule::DataModule()
{
    ADD(JointAngles);
}

bool DataModule::addUnlogger(std::string path)
{
    UnlogModule<Header> check(path);
    check.openFile();
    Header head = check.readNextMessage<Header>();
    check.closeFile();

    if(typeMap.find(head.name()) == typeMap.end())
    {
        std::cout << "BAD" << std::endl;
        return false;
    }

    unloggers.push_back(typeMap[head.name()](path));
    diag.addModule(*unloggers.back());
    return true;
}

void DataModule::run_()
{
    diag.run();
}

}
}
