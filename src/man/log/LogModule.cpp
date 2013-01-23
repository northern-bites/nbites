#include "LogModule.h"
#include "VisionDef.h"
#include <iostream>

using namespace portals;
using namespace std;

namespace man {
namespace log {

LogModule::LogModule() : Module(),
                         saved_frames(1),
                         topFile((FILEPATH+"TopImage"+EXT).c_str(),
                                 fstream::out),
                         bottomFile((FILEPATH+"BottomImage"+EXT).c_str(),
                                 fstream::out)
{
    topFile.write(HEADER.c_str(), HEADER.size());
    bottomFile.write(HEADER.c_str(), HEADER.size());
    topFile.flush();
    bottomFile.flush();
}

LogModule::~LogModule()
{
    topFile.close();
    bottomFile.close();
}

void LogModule::run_()
{
    topImageIn.latch();
    bottomImageIn.latch();
    writeCurrentFrames();
}

void LogModule::writeCurrentFrames()
{
    int MAX_FRAMES = 5000;
    if (saved_frames > MAX_FRAMES)
        return;

    topFile.write(reinterpret_cast<char*>(topImageIn.message().get_image()),
                  NAO_IMAGE_BYTE_SIZE);

    bottomFile.write(reinterpret_cast<char*>(bottomImageIn.message().get_image()),
                     NAO_IMAGE_BYTE_SIZE);

    topFile.flush();
    bottomFile.flush();

    cout << "Saved frame #" << saved_frames++ << endl;
}

}
}
