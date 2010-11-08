#include "OfflineVision.h"

#include <iostream>
#include <vector>

#include "Sensors.h"
#include "NaoPose.h"
#include "Profiler.h"
#include "Common.h"

using namespace std;
using boost::shared_ptr;

OfflineVision::OfflineVision()
{
    sensors = shared_ptr<Sensors>(new Sensors());
    pose = shared_ptr<NaoPose>(new NaoPose(sensors));
    profiler =
        shared_ptr<Profiler>(new Profiler(micro_time));

    vision = new Vision(pose, profiler);
}

OfflineVision::~OfflineVision()
{
    delete vision;
}

int OfflineVision::runOnDirectory(std::string path)
{
    cout << "Running vision processing on " << path << endl;

    sensors->loadFrame(path);
    long long t0 = micro_time();
    vision->notifyImage(sensors->getImage());
    cout << "Run time was: " << micro_time() - t0 << endl;

    cout << endl;
    return 0;
}

void printUsage()
{
    cout << "Usage: ./OfflineVision <path-to-directory>" << endl;
}

int main(int argv, char * argc[])
{
    if (argv < 2){
        printUsage();
        return 1;
    }

    OfflineVision * off = new OfflineVision();
    return off->runOnDirectory(argc[1]);
}
