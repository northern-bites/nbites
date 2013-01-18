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
    int MAX_FRAMES = 5000;
    if (saved_frames > MAX_FRAMES)
        return;

    stringstream num;
    num << saved_frames;
    string filename = FILEPATH + num.str() + EXT;

    fstream fout(filename.c_str(), fstream::out);
    fout.write(reinterpret_cast<const char*>(topImageIn.message()),
               NAO_IMAGE_BYTE_SIZE);

    fout.close();
    cout << "Saved frame #" << saved_frames++ << endl;
}

}
}
