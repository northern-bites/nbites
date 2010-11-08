#ifndef _OfflineVision_h_DEFINED
#define _OfflineVision_h_DEFINED

#include <stdlib.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Vision.h"

class NaoPose;
class Sensors;
class Profiler;


class OfflineVision
{
public:
    OfflineVision();
    virtual ~OfflineVision();

    int runOnDirectory(std::string path, int first, int last);

private:
    Vision * vision;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
    boost::shared_ptr<Profiler> profiler;

};

#endif /* _OfflineVision_h_DEFINED */
