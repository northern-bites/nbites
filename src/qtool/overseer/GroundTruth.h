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

#include "io/ProtobufMessage.h"
#include "Structs.h"
#include "GroundTruth.pb.h"
#include "ClassHelper.h"

namespace qtool {
namespace overseer {

class GroundTruth : public common::io::TemplatedProtobufMessage<proto::GroundTruth> {

    ADD_SHARED_PTR(GroundTruth)

public:
    typedef std::vector<point<float> > fpoint_vector;

public:
    GroundTruth()
        : TemplatedProtobufMessage<proto::GroundTruth>(class_name<GroundTruth>()) {}

    GroundTruth(point<float>* ballPosition,
                fpoint_vector* robotPositions)
        : TemplatedProtobufMessage<proto::GroundTruth>(class_name<GroundTruth>()),
          ballPosition(ballPosition), robotPositions(robotPositions) {}

    virtual ~GroundTruth() {}

    //TODO: move the updating to the server (much like robot objects update their own memory object

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
    point<float>* ballPosition;
    fpoint_vector* robotPositions;

};

}
}
