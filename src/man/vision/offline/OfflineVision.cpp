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

/**
 * Run the vision processing a set of directories given the path to
 * the directory and the bounds of image numbers on which to run.
 *
 * @note Only works with VERSIONED Nao images and requires that the
 *       images be numbered continuously from first to last
 */
int OfflineVision::runOnDirectory(std::string path, int first, int last)
{
    assert(last >= first);
    cout << "Running vision processing on " << path << endl;

    long long int t0;
    long long int runTime = 0;
    for (int i = first; i <= last; ++i){
        stringstream framePath;
        framePath << path << "/" << i << ".NBFRM";
        sensors->loadFrame(framePath.str());

        // We only want to time the vision processing, not the frame loading
        t0 = micro_time();
        vision->notifyImage(sensors->getImage());
        runTime += micro_time() - t0;
    }
    cout << "Total Run time was: " << runTime << endl;
    // Runs from first -> last inclusive, so must add 1
    cout << "Average Run time was: " << runTime/(last-first+1) << endl;

    cout << endl;
    return 0;
}

void printUsage()
{
    cout << "Usage: ./OfflineVision <path-to-directory>"
         << " <# first frame> <# last frame>" << endl;
}

int main(int argv, char * argc[])
{
    if (argv != 4){
        printUsage();
        return 1;
    }

    OfflineVision * off = new OfflineVision();
    return off->runOnDirectory(argc[1], atoi(argc[2]), atoi(argc[3]));
}
