#pragma once

#include "boost/shared_ptr.hpp"

#include "pb/Vision.pb.h"
#include "Vision.h"
#include "MObject.hpp"

class MVision;

using boost::shared_ptr;

class MVision: public MObject {
    friend class Vision;

private:
    void update();

public:
    MVision(shared_ptr<Vision> vision_ptr);

private:
    shared_ptr<Vision> vision;
};
