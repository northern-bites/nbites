/**
 * @class GroundTruth
 *
 * Should store ground truth positions of robots and ball
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Structs.h"
#include "io/ProtobufMessage.h"
#include "GroundTruth.pb.h"

namespace qtool {
namespace overseer {

class GroundTruth : public common::io::ProtobufMessage {

public:
    typedef boost::shared_ptr<proto::GroundTruth> ProtoGroundTruth_ptr;
    typedef std::vector<point<float> > fpoint_vector;

public:
    GroundTruth(point<float>* ballPosition,
                fpoint_vector* robotPositions,
                ProtoGroundTruth_ptr data = ProtoGroundTruth_ptr(new proto::GroundTruth))
        : ProtobufMessage(data, "GroundTruth"), data(data),
          ballPosition(ballPosition), robotPositions(robotPositions) {}

    virtual ~GroundTruth() {}

    virtual void updateData() {

        data->set_timestamp(time_stamp());

        proto::GroundTruth::Point* ball_position = data->mutable_ball();
        ball_position->set_x(ballPosition->x);
        ball_position->set_y(ballPosition->y);

        data->clear_robots();
        for(fpoint_vector::iterator it = robotPositions->begin(); it != robotPositions->end(); it++) {
            proto::GroundTruth::Point* robot_position = data->add_robots();
            robot_position->set_x(it->x);
            robot_position->set_y(it->y);
        }

    }

protected:
    ProtoGroundTruth_ptr data;
    point<float>* ballPosition;
    fpoint_vector* robotPositions;

};

}
}
