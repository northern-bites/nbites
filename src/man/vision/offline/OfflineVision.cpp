#include "OfflineVision.h"

#include <iostream>
#include <vector>

#include "Sensors.h"
#include "NaoPose.h"
#include "Profiler.h"
#include "Common.h"

using namespace std;
using boost::shared_ptr;

OfflineVision::OfflineVision(int _iterations, int _first, int _last) :
    numIterations(_iterations), first(_first), last(_last)
{
    assert(last >= first);
    sensors = shared_ptr<Sensors>(new Sensors());
    pose = shared_ptr<NaoPose>(new NaoPose(sensors));
    profiler =
        shared_ptr<Profiler>(new Profiler(micro_time));

    vision = new Vision(pose, profiler);

#ifdef USE_TIME_PROFILING
    profiler->profiling = true;
    profiler->profileFrames((last-first+1) * numIterations);
    profiler->maxPrintDepth = 2;
#endif
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
int OfflineVision::runOnDirectory(std::string path)
{
    for (int c=0; c < numIterations; ++c){
        for (int i = first; i <= last; ++i){
            stringstream framePath;
            framePath << path << "/" << i << ".frm";
            sensors->loadFrame(framePath.str());
            vision->notifyImage(sensors->getImage());
            PROF_NFRAME();
        }
    }
    PROF_NFRAME();
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
    if (argv < 4){
        printUsage();
        return 1;
    }

    int numIterations;
    if (argv == 4){
        numIterations = 1;
    } else {
        numIterations = atoi(argc[4]);
    }

    OfflineVision * off = new OfflineVision(numIterations,
                                            atoi(argc[2]), atoi(argc[3]));
    return off->runOnDirectory(argc[1]);
}
