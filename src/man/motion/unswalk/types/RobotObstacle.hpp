/*
 * RobotObservation.hpp
 *
 *  Created on: 02/07/2014
 *      Author: jashmore
 */

#pragma once

#include "RobotInfo.hpp"
#include "AbsCoord.hpp"
#include "RRCoord.hpp"

struct RobotObstacle {
    RRCoord rr;
    RobotInfo::Type type;
    AbsCoord rrc;
    AbsCoord pos;
    //Used to determine if a heading would overlap the obstacle
    double tangentHeadingLeft;
    double tangentHeadingRight;

    //Used as a path if we want to go around the obstacle
    RRCoord evadeVectorLeft;
    RRCoord evadeVectorRight;

    virtual ~RobotObstacle () {}

    bool operator== (const RobotObstacle &other) const {
       return rr == other.rr;
    }

    template<class Archive>
    void serialize(Archive &ar, const unsigned int file_version) {
        ar & rr;
        ar & type;
        ar & rrc;
        ar & pos;
        ar & tangentHeadingLeft;
        ar & tangentHeadingRight;
        ar & evadeVectorLeft;
        ar & evadeVectorRight;
    }
};
