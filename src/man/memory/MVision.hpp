#pragma once

#include "boost/shared_ptr.hpp"

#include "gen/Vision.pb.h"
#include "MObject.hpp"
#include "Vision.h"

class MVision;

class MVision: public Proto::PVision, public MObject {

public:
    MVision(boost::shared_ptr<Vision> v);
    ~MVision();
    void update();
    void log() const;

private:
    boost::shared_ptr<Vision> vision;
    FileLogger* fileLogger;

};
