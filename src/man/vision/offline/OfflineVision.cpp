#include "OfflineVision.h"

#include <iostream>
#include <vector>

#include "Sensors.h"
#include "NaoPose.h"
#include "Profiler.h"
#include "Common.h"
#include "ImageAcquisition.h"

using namespace std;
using boost::shared_ptr;

OfflineVision::OfflineVision(int _iterations, int _first, int _last) :
    numIterations(_iterations), first(_first), last(_last),
    table(new unsigned char[yLimit * uLimit * vLimit]),
    params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit)
{
    assert(last >= first);
    sensors = shared_ptr<Sensors>(new Sensors());
    pose = shared_ptr<NaoPose>(new NaoPose(sensors));
    profiler =
        shared_ptr<Profiler>(new Profiler(thread_micro_time));

    vision = new Vision(pose, profiler);

#ifdef USE_TIME_PROFILING
    profiler->profiling = true;
    profiler->profileFrames((last-first+1) * numIterations);
    profiler->printEmpty = false;
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
    // Allocate output image buffer
    uint16_t * image =
        reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE]);

    // Put a buffer between the last print statement and this one.
    cout << endl;

    for (int c=0; c < numIterations; ++c){
        for (int i = first; i <= last; ++i){
            stringstream framePath;
            framePath << path << "/" << i << ".frm";

            sensors->loadFrame(framePath.str());

            _acquire_image_fast(table, &params,
                                const_cast<uint8_t*>(sensors->getNaoImage()),
                                image);

            vision->notifyImage(image);
            PROF_NFRAME(profiler);
        }
    }
    PROF_NFRAME(profiler);
    cout << endl;

    delete[] image;

    return 0;
}

// @TODO: Create OfflineTranscriber to do this work for you.
void OfflineVision::initTable(string filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("initTable() FAILED to open filename: %s", filename.c_str());
#ifdef OFFLINE
        exit(0);
#else
        return;
#endif
    }

    // actually read the table into memory
    // Color table is in UVY ordering
    int rval;
    for(int u=0; u< uLimit; ++u){
        for(int v=0; v < vLimit; ++v){
            rval = fread(&table[u * vLimit * yLimit + v * yLimit],
                         sizeof(unsigned char), yLimit, fp);
        }
    }

    printf("Loaded colortable %s",filename.c_str());

    fclose(fp);
}

void printUsage()
{
    cout << "Usage: ./OfflineVision <path-to-directory>"
         << " <# first frame> <# last frame> "
         << "\n\tOPTIONAL: <# iterations> <color table>" << endl;
}

int main(int argv, char * argc[])
{
    if (argv < 4){
        printUsage();
        return 1;
    }

    int numIterations = 1;
    if (argv > OfflineVision::iterations){
        numIterations = atoi(argc[OfflineVision::iterations]);
    }

    OfflineVision * off =
        new OfflineVision(numIterations,
                          atoi(argc[OfflineVision::first_img]),
                          atoi(argc[OfflineVision::last_img]));

    if (argv == OfflineVision::table_name+1){
        off->initTable(argc[OfflineVision::table_name]);
    }

    off->runOnDirectory(argc[OfflineVision::directory]);
    delete off;
    return 0;
}
