#ifndef _OfflineVision_h_DEFINED
#define _OfflineVision_h_DEFINED

#include <stdlib.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Vision.h"
#include "ColorParams.h"

class NaoPose;
class Sensors;
class Profiler;


class OfflineVision
{
public:
    OfflineVision(int _iterations, int _first, int _last);
    virtual ~OfflineVision();

    int runOnDirectory(std::string path);
    void initTable(string filename);
private:

public:
    enum {
        directory = 1,
        first_img,
        last_img,
        iterations,
        table_name
    };

private:
    Vision * vision;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
    boost::shared_ptr<Profiler> profiler;

    int numIterations, first, last;
    unsigned char *table;
    ColorParams params;
    enum {
        y0 = 0,
        u0 = 0,
        v0 = 0,

        y1 = 256,
        u1 = 256,
        v1 = 256,

        yLimit = 128,
        uLimit = 128,
        vLimit = 128,

        tableByteSize = yLimit * uLimit * vLimit
    };
};

#endif /* _OfflineVision_h_DEFINED */
