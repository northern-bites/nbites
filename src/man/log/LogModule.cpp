#include "LogModule.h"
#include <iostream>

using namespace portals;
using namespace std;

namespace man {
namespace log {

LogModule::LogModule() : Module()
{
}

LogModule::~LogModule()
{
}

void LogModule::run_()
{
    for(vector<LoggerBase*>::iterator i = logs.begin(); i != logs.end(); i++)
    {
        (*i)->signalToResume();
    }
}

}
}
