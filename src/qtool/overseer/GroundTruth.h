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
#include "data/QProtobufMessage.h"
#include "GroundTruth.pb.h"
#include "ClassHelper.h"

namespace qtool {
namespace overseer {

class GroundTruth : public data::QProtobufMessage {

    ADD_SHARED_PTR(GroundTruth)

public:
    typedef boost::shared_ptr<const proto::GroundTruth> ProtoGroundTruth_const_ptr;
    typedef boost::shared_ptr<proto::GroundTruth> ProtoGroundTruth_ptr;
    typedef std::vector<point<float> > fpoint_vector;

public:
    GroundTruth(ProtoGroundTruth_ptr data = ProtoGroundTruth_ptr(new proto::GroundTruth))
        : QProtobufMessage(data, "GroundTruth"), data(data) {}

    GroundTruth(point<float>* ballPosition,
                fpoint_vector* robotPositions,
                ProtoGroundTruth_ptr data = ProtoGroundTruth_ptr(new proto::GroundTruth))
        : QProtobufMessage(data, "GroundTruth"), data(data),
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

    virtual ProtoGroundTruth_const_ptr get() const {
        return data;
    }

protected:
    ProtoGroundTruth_ptr data;
    point<float>* ballPosition;
    fpoint_vector* robotPositions;

};

}
}
