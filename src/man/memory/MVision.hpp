#pragma once

#include <boost/shared_ptr.hpp>

#include "gen/Vision.pb.h"
#include "MObject.hpp"
#include "Vision.h"
#include "log/FileLogger.hpp"

//class MVision;

namespace memory {
class MVision: public proto::PVision, public MObject {

public:
    MVision(boost::shared_ptr<Vision> v);
    ~MVision();
    void update();
    void log() const;

private:
    boost::shared_ptr<Vision> vision;
    log::FileLogger* fileLogger;

};
}
