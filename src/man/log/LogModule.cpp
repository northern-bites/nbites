#include "LogModule.h"
#include "VisionDef.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace portals;
using namespace std;

namespace man {
namespace log {

LogModule::LogModule() : Module(),
                         saved_frames(1)
{
}

void LogModule::run_()
{
    topImageIn.latch();
    writeFrame();
}

void LogModule::writeFrame()
{
    for(vector<LoggerBase*>::iterator i = logs.begin(); i != logs.end(); i++)
    {
        (*i)->signalToResume();
    }
}

}
}
