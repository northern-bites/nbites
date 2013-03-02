#pragma once

#include "UnlogModule.h"
#include "JointAngles.pb.h"
#include <vector>

namespace tool{
namespace unlog{

typedef UnlogBase*(* Construct)(std::string);
typedef std::map<std::string, Construct> TypeMap;

#define ADD(name) \
    std::string nameStr = std::string("messages.") + std::string(#name); \
    typeMap[nameStr] = &makeUnlogger<messages::name>;

template<class T> UnlogBase* makeUnlogger(std::string path)
{
    return new UnlogModule<T>(path);
}

class DataModule : public portals::Module
{
public:
    DataModule();

    bool addUnlogger(std::string path);

protected:
    virtual void run_();
    portals::RoboGram diag;
    TypeMap typeMap;
    std::vector<UnlogBase*> unloggers;
};

}
}
