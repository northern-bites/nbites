#pragma once

#include "unlog/UnlogModule.h"
#include "Header.pb.h"

namespace tool{

// Things for mapping names to types
typedef unlog::UnlogBase*(* Construct)(std::string);
typedef std::map<std::string, Construct> TypeMap;

#define ADD_MAPPED_TYPE(name) \
    std::string nameStr = std::string("messages.") + std::string(#name); \
    typeMap[nameStr] = &makeUnlogger<messages::name>;

template<class T> unlog::UnlogBase* makeUnlogger(std::string path)
{
    return new unlog::UnlogModule<T>(path);
}

// Actual class

class ToolDiagram
{
public:
    ToolDiagram();

    void addModule(portals::Module& mod);
    bool unlogFrom(std::string path);
    void run() { diagram.run(); }

protected:
    portals::RoboGram diagram;
    std::vector<unlog::UnlogBase*> unloggers;

    TypeMap typeMap;
};

}
