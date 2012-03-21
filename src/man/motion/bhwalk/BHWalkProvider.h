/**
 * @class BHWalkProvider
 *
 * A MotionProvider that uses the B-Human walk engine to compute the next body joints
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>

#include "Sensors.h"
#include "NaoPose.h"
#include "WalkCommand.h"
#include "StepCommand.h"
#include "DestinationCommand.h"
#include "MetaGait.h"
#include "BodyJointCommand.h"
#include "EKFStructs.h"
#include "MotionProvider.h"

//BH
#include "Robot/RobotDimensions.h"
#include "Robot/MassCalibration.h"
#include "Robot/RobotModel.h"
#include "Robot/TorsoMatrix.h"
#include "Robot/JointCalibration.h"
#include "WalkingEngine.h"
#include "TorsoMatrixProvider.h"

namespace man {
namespace motion {

class BHWalkProvider : public MotionProvider {

public:
    //TODO: naopose is for nothing, I just kept it here because the WalkProvider is the same
    //but it should be eliminated from both places (e.g. it is not used here, it is not used
    //in walkprovider)
    BHWalkProvider(boost::shared_ptr<Sensors> s, boost::shared_ptr<NaoPose> p);
    virtual ~BHWalkProvider() {}

    void requestStopFirstInstance() {
        setCommand(WalkCommand::ptr( new WalkCommand(0.0f, 0.0f, 0.0f) ));
    }
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void setCommand(const WalkCommand::ptr command);
    void setCommand(const Gait::ptr command) {}
    void setCommand(const StepCommand::ptr command);
    void setCommand(const DestinationCommand::ptr command);

    std::vector<BodyJointCommand::ptr> getGaitTransitionCommand() {
        return std::vector<BodyJointCommand::ptr>();
    }

    MotionModel getOdometryUpdate();

    virtual const SupportFoot getSupportFoot() const;

protected:
    void setActive() {}

private:
    boost::shared_ptr<Sensors> sensors;
    RobotModel robotModel;
    TorsoMatrix torsoMatrix;
    RobotDimensions robotDimensions;
    MassCalibration massCalibration;
    JointCalibration jointCalibration;
    TorsoMatrixProvider torsoMatrixProvider;
    WalkingEngine walkingEngine;
    WalkingEngineOutput walkOutput;
};

}
}
