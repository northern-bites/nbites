/**
 * @file WalkingEngine.cpp
 * Implementation of a module that creates the walking motions
 * @author Colin Graf
 */

#include "WalkingEngine.h"
#include "Tools/Debugging/DebugDrawings.h" // PLOT
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Tools/InverseKinematic.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Tools/Streams/InStreams.h"
#include "Tools/Settings.h"
#include <algorithm>
#include <iostream>

using namespace std;

MAKE_MODULE(WalkingEngine, Motion Control)

    PROCESS_WIDE_STORAGE(WalkingEngine) WalkingEngine::theInstance = 0;

WalkingEngine::WalkingEngine() : WalkingEngineBase(), optimizeStarted(false)
{
    theInstance = this;

    naoProvider = new NaoProvider();
    jointFilter = new JointFilter();
    robotModelProvider = new RobotModelProvider();
    groundContactDetector = new GroundContactDetector();
    inertiaSensorCalibrator = new InertiaSensorCalibrator();
    inertiaSensorFilter = new InertiaSensorFilter();
    sensorFilter = new SensorFilter();
    fallDownStateDetector = new FallDownStateDetector();
    torsoMatrixProvider = new TorsoMatrixProvider();
    motionSelector = new MotionSelector();

    init();

#ifdef TARGET_SIM
    observerMeasurementDelay = 60.f;
#endif

    // load walking engine kicks
    kicks.load(ModuleBase::config_path);

    // reset internal state
    reset();
}

WalkingEngine::~WalkingEngine()
{
    delete naoProvider;
    delete jointFilter;
    delete robotModelProvider;
    delete groundContactDetector;
    delete inertiaSensorCalibrator;
    delete inertiaSensorFilter;
    delete sensorFilter;
    delete fallDownStateDetector;
    delete torsoMatrixProvider;
    delete motionSelector;

    theInstance = 0;
}

bool WalkingEngine::handleMessage(InMessage& message)
{
    WalkingEngine* engine = theInstance;
    if(engine && message.getMessageID() == idWalkingEngineKick)
    {
        unsigned int id, size;
        message.bin >> id >> size;
        ASSERT(id < WalkRequest::numOfKickTypes);
        char* buffer = new char[size + 1];
        message.bin.read(buffer, size);
        buffer[size] = '\0';
        engine->kicks.load(ModuleBase::config_path, WalkRequest::KickType(id), buffer);
        delete[] buffer;
        return true;
    }
    return false;
}

void WalkingEngine::serialize(In* in, Out* out)
{
    STREAM_REGISTER_BEGIN
        STREAM_BASE(WalkingEngineBase)
        STREAM_REGISTER_FINISH

        if(in)
            init();
}

void WalkingEngine::init()
{
    InMapFile hardnessStream(ModuleBase::config_path + "hardnessSettings.cfg");
    if (hardnessStream.exists()) {
        hardnessStream >> theHardnessSettingsBH;
    } else {
        cout << "Could not find hardnessSettings.cfg!" << endl;
    }

    InMapFile sensorStream(ModuleBase::config_path + "sensorCalibration.cfg");
    if (sensorStream.exists()) {
        sensorStream >> theSensorCalibrationBH;
    } else {
        cout << "Could not find sensorCalibration.cfg!" << endl;
    }

    InMapFile jointCalibrateStream(ModuleBase::config_path + "jointCalibration.cfg");
    if (jointCalibrateStream.exists()) {
        jointCalibrateStream >> theJointCalibrationBH;
    } else {
        cout << "Could not find jointCalibration.cfg!" << endl;
    }

#ifdef V5_ROBOT
    InMapFile massCalibrationStream(ModuleBase::config_path + "massCalibrationV5.cfg");
#else
    InMapFile massCalibrationStream(ModuleBase::config_path + "massCalibrationV4.cfg");
#endif
    if (massCalibrationStream.exists()) {
        massCalibrationStream >> theMassCalibrationBH;
    } else {
        cout << "Could not find massCalibration.cfg!" << endl;
    }

    InMapFile robotDimensionsStream(ModuleBase::config_path + "robotDimensions.cfg");
    if (robotDimensionsStream.exists()) {
        robotDimensionsStream >> theRobotDimensionsBH;
    } else {
        cout << "Could not find robotDimensions.cfg!" << endl;
    }

    InMapFile damageStream(ModuleBase::config_path + "damageConfiguration.cfg");
    if (damageStream.exists()) {
        damageStream >> theDamageConfigurationBH;
    } else {
        cout << "Could not find damageConfiguration.cfg!" << endl;
    }

    // pre compute constants
    const float g = 9806.65f;
    walkK.x = sqrt(g / walkHeight.x);
    walkK.y = sqrt(g / walkHeight.y);
    standBodyRotation = RotationMatrixBH(Vector3BH<>(0.f, standBodyTilt, 0.f));
    walkPhaseDuration = walkStepDuration * (0.001f * 0.5f);
    walkPhaseDurationAtFullSpeedX = walkStepDurationAtFullSpeedX * (0.001f * 0.5f);
    walkPhaseDurationAtFullSpeedY = walkStepDurationAtFullSpeedY * (0.001f * 0.5f);

    // compute walkXvdXSoftLimit and walkXvdXHardLimit

    // td = tt * 0.5f
    // xv0 * sinh(k * td) / k = ns * 0.5f
    // xvd = xv0 * cosh(k * td)

    // xv0 = ns * 0.5f * k / sinh(k * td)

    const float k = walkK.x;
    const float tt = walkPhaseDurationAtFullSpeedX;
    const float td = tt * 0.5f;
    const float coshKTd = cosh(k * td);
    const float sinhKTd = sinh(k * td);
    float ns, xv0;

    ns = walkStepSizeXPlanningLimit.max;
    xv0 = ns * 0.5f * k / sinhKTd;
    walkXvdXPlanningLimit.max = xv0 * coshKTd;
    ASSERT(fabs(xv0 * sinh(k * td) / k - ns * 0.5f) < 0.1f);

    ns = walkStepSizeXPlanningLimit.min;
    xv0 = ns * 0.5f * k / sinhKTd;
    walkXvdXPlanningLimit.min = xv0 * coshKTd;
    ASSERT(fabs(xv0 * sinh(k * td) / k - ns * 0.5f) < 0.1f);

    ns = walkStepSizeXLimit.max;
    xv0 = ns * 0.5f * k / sinhKTd;
    walkXvdXLimit.max = xv0 * coshKTd;
    ASSERT(fabs(xv0 * sinh(k * td) / k - ns * 0.5f) < 0.1f);

    ns = walkStepSizeXLimit.min;
    xv0 = ns * 0.5f * k / sinhKTd;
    walkXvdXLimit.min = xv0 * coshKTd;
    ASSERT(fabs(xv0 * sinh(k * td) / k - ns * 0.5f) < 0.1f);
}

void WalkingEngine::reset()
{
    currentMotionType = stand;
    requestedMotionType = stand;

    pendulumPlayer.engine = this;
    generateFirstPendulumPhase(pendulumPlayer.phase);
    generateNextPendulumPhase(pendulumPlayer.phase, pendulumPlayer.nextPhase);
    pendulumPlayer.seek(0.f);
    lastRequestedSpeedRel = Pose2DBH();

    lastExpectedLeftToCom = Vector3BH<>();
    lastExpectedRightToCom = Vector3BH<>();

    lastGyroErrorY = 0.0f;
    lastSmoothedGyroY = InertiaSensorDataBH::off;
}

void WalkingEngine::update(WalkingEngineOutputBH& walkingEngineOutput)
{
    DECLARE_PLOT("module:WalkingEngine:torsoAngularVelocityY");
    DECLARE_PLOT("module:WalkingEngine:torsoAngularVelocityCorrectionY");
    DECLARE_PLOT("module:WalkingEngine:gyroY");
    DECLARE_PLOT("module:WalkingEngine:smoothedGyroY");
    MODIFY("module:WalkingEngine:optimizeBestParameters", optimizeBestParameters);

    theMotionInfoBH.motion = theMotionRequestBH.motion;
    theMotionInfoBH.kickRequest = theMotionRequestBH.kickRequest;
    theMotionInfoBH.specialActionRequest = theMotionRequestBH.specialActionRequest;
    theMotionInfoBH.walkRequest = theMotionRequestBH.walkRequest;
    theMotionInfoBH.isMotionStable = true;

    naoProvider->update(theJointDataBH, theSensorDataBH);
    naoProvider->update(theFrameInfoBH);
    jointFilter->update(theFilteredJointDataBH);
    robotModelProvider->update(theRobotModelBH);
    groundContactDetector->update(theGroundContactStateBH);
    inertiaSensorCalibrator->update(theInertiaSensorDataBH);
    inertiaSensorFilter->update(theOrientationDataBH);
    sensorFilter->update(theFilteredSensorDataBH);
    fallDownStateDetector->update(theFallDownStateBH);
    torsoMatrixProvider->update(theOdometryDataBH);
    torsoMatrixProvider->update(theTorsoMatrixBH);
    motionSelector->update(theMotionSelectionBH);

    // if(theMotionSelectionBH.ratios[MotionRequestBH::walk] > 0.9f && countParams <= 50)
    // {
    //     if(!optimizeOptimizer.isRunning())
    //     {
    //         countParams = 0;
    //         optimizeOptimizer.addDimension(walkLiftOffset.y, 0.f, walkLiftOffset.y + 20.f, 1.f);
    //         optimizeOptimizer.addDimension(walkLiftOffset.z, 0.f, walkLiftOffset.z + 20.f, 1.f);
    //         optimizeOptimizer.start();
    //         (WalkingEngineBase&) optimizeBestParameters = *this;
    //     }
    //     if(optimizeStarted && theFrameInfoBH.getTimeSince(optimizeStartTime) > 8000)
    //     {
    //         const float rating = optimizeFitness.getAverage();
    //         optimizeFitness.init();
    //         optimizeOptimizer.setRating(rating);
    //         if(rating == optimizeOptimizer.getBestRating())
    //         {
    //             std::cout << "Best params!" << std::endl;
    //             std::cout << rating << std::endl;
    //             OUTPUT(idText, text, "optimize: rating=" << rating << " (new optimum)");
    //             (WalkingEngineBase&) optimizeBestParameters = *this;
    //         }
    //         else
    //         {
    //             std::cout << "Not best params!" << std::endl;
    //             std::cout << rating << std::endl;
    //             OUTPUT(idText, text, "optimize: rating=" << rating);
    //         }
    //         optimizeStarted = false;
    //     }

    //     if(!optimizeStarted)
    //     {
    //         optimizeStarted = true;
    //         optimizeStartTime = theFrameInfoBH.time;

    //         optimizeOptimizer.next();
    //         init();
    //         std::cout << "Params:" << std::endl;
    //         std::cout << countParams++ << std::endl;
    //         std::cout << walkLiftOffset.y << std::endl;
    //         std::cout << walkLiftOffset.z << std::endl;
    //     }
    // }
    // if (countParams == 50)
    // {
    //     if(optimizeStarted)
    //     {
    //         optimizeStarted = false;
    //         (WalkingEngineBase&) *this = optimizeBestParameters;
    //         std::cout << "Best params:" << std::endl;
    //         std::cout << walkLiftOffset.y << std::endl;
    //         std::cout << walkLiftOffset.z << std::endl;
    //     }
    // }

    if(theMotionSelectionBH.ratios[MotionRequestBH::walk] > 0.f || theMotionSelectionBH.ratios[MotionRequestBH::stand] > 0.f)
    {
        updateMotionRequest();
        updatePendulumPlayer();
        computeMeasuredPosture();
        computeExpectedPosture();
        computeEstimatedPosture();
        computeError();
        correctPendulumPlayer();
        updatePredictedPendulumPlayer();
        generateTargetPosture();
        generateJointRequest();
        computeOdometryOffset();
        generateOutput(walkingEngineOutput);
    }
    else
    {
        reset();
        generateDummyOutput(walkingEngineOutput);
    }

    for(int i = 0; i < FilteredJointDataBH::numOfJoints; ++i)
    {
        if (walkingEngineOutput.jointHardness.hardness[i] == HardnessSettingsBH::useDefault)
        {
            walkingEngineOutput.jointHardness.hardness[i] = theHardnessSettingsBH.hardness[i];
        }
    }

    DEBUG_RESPONSE("module:WalkingEngine:initialPosition",
            for(int i = 0; i < JointDataBH::numOfJoints; ++i)
            jointRequest.angles[i] = walkingEngineOutput.angles[i] = 0.f;
              );

    DECLARE_DEBUG_DRAWING3D("module:WalkingEngine:zmp", "field", drawZmp(); );

#ifdef TARGET_SIM
    declareDrawings(walkingEngineOutput);
#endif

    // Northern Bites hack for hand speed info
    updateHandSpeeds();
}

void WalkingEngine::updateMotionRequest()
{
    if(theMotionRequestBH.motion == MotionRequestBH::walk || theMotionRequestBH.motion == MotionRequestBH::stand)
    {
        if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
        {
            if(theMotionRequestBH.walkRequest.target != Pose2DBH() && theMotionRequestBH.walkRequest.target != lastCopiedWalkTarget)
                lastCopiedWalkTarget = requestedWalkTarget = theMotionRequestBH.walkRequest.target;
        }
    }
    else if(theMotionRequestBH.walkRequest.target != Pose2DBH() && theMotionRequestBH.walkRequest.target != lastCopiedWalkTarget)
        lastCopiedWalkTarget = requestedWalkTarget = theMotionRequestBH.walkRequest.target;

    // get requested motion state
    requestedMotionType = stand;
    if(theInertiaSensorDataBH.calibrated && /*theGroundContactStateBH.contact &&*/ theMotionSelectionBH.ratios[MotionRequestBH::walk] >= 1.f)
    {
        if(theMotionRequestBH.motion == MotionRequestBH::walk)
        {
            if(currentMotionType == stand)
            {
                Vector3BH<> angleDiff = ((const RotationMatrixBH&)(standBodyRotation.invert() * theTorsoMatrixBH.rotation)).getAngleAxis();
                if(angleDiff.squareAbs() < 0.0001f) // don't start walking until the robot stopped fluctuating
                {
                    requestedMotionType = stepping;
                }
            }
            else
            {
                requestedMotionType = stepping;
            }
        }
    }
}

void WalkingEngine::updatePendulumPlayer()
{
    // motion update
    if(pendulumPlayer.phase.type != standPhase || pendulumPlayer.nextPhase.type != standPhase)
    {
        pendulumPlayer.seek(theFrameInfoBH.cycleTime);
        if(pendulumPlayer.phase.type == standPhase && pendulumPlayer.nextPhase.type == standPhase)
            currentMotionType = stand;
    }

    // change motion type from stand to stepping
    if(currentMotionType==stand && requestedMotionType == stepping)
    {
        generateFirstPendulumPhase(pendulumPlayer.phase);
        generateNextPendulumPhase(pendulumPlayer.phase, pendulumPlayer.nextPhase);
        updatePendulumPhase(pendulumPlayer.phase, pendulumPlayer.nextPhase, true);
        currentMotionType = stepping;
    }
}

void WalkingEngine::computeMeasuredPosture()
{
    measuredLeftToCom = -Pose3DBH(theTorsoMatrixBH.rotation).translate(-theRobotModelBH.centerOfMass).conc(theRobotModelBH.limbs[MassCalibrationBH::footLeft]).translate(0.f, 0.f, -theRobotDimensionsBH.heightLeg5Joint).translation;
    measuredRightToCom = -Pose3DBH(theTorsoMatrixBH.rotation).translate(-theRobotModelBH.centerOfMass).conc(theRobotModelBH.limbs[MassCalibrationBH::footRight]).translate(0.f, 0.f, -theRobotDimensionsBH.heightLeg5Joint).translation;
}

void WalkingEngine::computeExpectedPosture()
{
    LegPosture expectedPosture;
    pendulumPlayer.getPosture(expectedPosture, 0, 0, &observedStepOffset);

    expectedLeftToCom = expectedPosture.leftOriginToCom - expectedPosture.leftOriginToFoot.translation;
    expectedRightToCom = expectedPosture.rightOriginToCom - expectedPosture.rightOriginToFoot.translation;
    expectedComVelocity = pendulumPlayer.phase.xv0;
}

void WalkingEngine::computeEstimatedPosture()
{
    if(lastExpectedLeftToCom.z == 0.f)
    {
        estimatedLeftToCom = expectedLeftToCom;
        estimatedRightToCom = expectedRightToCom;
        estimatedComVelocity = expectedComVelocity;

        covX = Matrix3x3f(Vector3f(sqrBH(observerProcessDeviation[0]), 0, observerProcessDeviation[0] * observerProcessDeviation[2]),
                Vector3f(0, sqrBH(observerProcessDeviation[0]), observerProcessDeviation[0] * observerProcessDeviation[2]),
                Vector3f(observerProcessDeviation[0] * observerProcessDeviation[2], observerProcessDeviation[0] * observerProcessDeviation[2], sqrBH(observerProcessDeviation[2])));
        covY = Matrix3x3f(Vector3f(sqrBH(observerProcessDeviation[1]), 0, observerProcessDeviation[1] * observerProcessDeviation[3]),
                Vector3f(0, sqrBH(observerProcessDeviation[1]), observerProcessDeviation[1] * observerProcessDeviation[3]),
                Vector3f(observerProcessDeviation[1] * observerProcessDeviation[3], observerProcessDeviation[1] * observerProcessDeviation[3], sqrBH(observerProcessDeviation[3])));
    }
    else
    {
        estimatedLeftToCom += expectedLeftToCom - lastExpectedLeftToCom;
        estimatedRightToCom += expectedRightToCom - lastExpectedRightToCom;
        estimatedComVelocity = expectedComVelocity;
    }

    static const Matrix2x3f c(Vector2f(1, 0), Vector2f(0, 1), Vector2f());
    static const Matrix3x2f cTransposed = c.transpose();
    const Matrix3x3f a(Vector3f(1, 0, 0), Vector3f(0, 1, 0),
            Vector3f(pendulumPlayer.phase.type == leftSupportPhase ? theFrameInfoBH.cycleTime : 0,
                pendulumPlayer.phase.type == rightSupportPhase ? theFrameInfoBH.cycleTime : 0,
                1));
    covX = a * covX * a.transpose();
    covY = a * covY * a.transpose();

    covX[0][0] += sqrBH(observerProcessDeviation[0]);
    covX[1][1] += sqrBH(observerProcessDeviation[0]);
    covX[2][2] += sqrBH(observerProcessDeviation[2]);
    covY[0][0] += sqrBH(observerProcessDeviation[1]);
    covY[1][1] += sqrBH(observerProcessDeviation[1]);
    covY[2][2] += sqrBH(observerProcessDeviation[3]);

    Matrix2x2f covXPlusSensorCov = c * covX * cTransposed;
    covXPlusSensorCov[0][0] += sqrBH(observerMeasurementDeviation[0]);
    covXPlusSensorCov[1][1] += sqrBH(observerMeasurementDeviation[0]);

    Matrix2x2f covYPlusSensorCov = c * covY * cTransposed;
    covYPlusSensorCov[0][0] += sqrBH(observerMeasurementDeviation[1]);
    covYPlusSensorCov[1][1] += sqrBH(observerMeasurementDeviation[1]);

    Matrix3x2f kalmanGainX = covX * cTransposed * covXPlusSensorCov.invert();
    covX -= kalmanGainX * c * covX;
    Vector3f correctionX = kalmanGainX * Vector2f(measuredLeftToCom.x - estimatedLeftToCom.x, measuredRightToCom.x - estimatedRightToCom.x);

    Matrix3x2f kalmanGainY = covY * cTransposed * covYPlusSensorCov.invert();
    covY -= kalmanGainY * c * covY;
    Vector3f correctionY = kalmanGainY * Vector2f(measuredLeftToCom.y - estimatedLeftToCom.y, measuredRightToCom.y - estimatedRightToCom.y);

    estimatedLeftToCom.x += correctionX[0];
    estimatedRightToCom.x += correctionX[1];
    estimatedComVelocity.x += correctionX[2];

    estimatedLeftToCom.y += correctionY[0];
    estimatedRightToCom.y += correctionY[1];
    estimatedComVelocity.y += correctionY[2];

    lastExpectedLeftToCom = expectedLeftToCom;
    lastExpectedRightToCom = expectedRightToCom;
    lastExpectedComVelocity = expectedComVelocity;
}

void WalkingEngine::computeError()
{
    if(theGroundContactStateBH.contact && theFallDownStateBH.state == FallDownStateBH::upright)
    {
        errorLeft = estimatedLeftToCom - expectedLeftToCom;
        errorRight = estimatedRightToCom - expectedRightToCom;
        errorVelocity = estimatedComVelocity - expectedComVelocity;
        instability.add(sqrBH((errorLeft.y + errorRight.y) * 0.5f));
    }
    else
    {
        errorLeft = errorRight = Vector3BH<>();
        errorVelocity = Vector2BH<>();
    }

    // if(theGroundContactStateBH.contact && currentMotionType == stepping)
    //     optimizeFitness.add(sqrBH((errorLeft.y + errorRight.y) * 0.5f));
    // else
    //     optimizeFitness.add(sqrBH((20.f + 20.f) * 0.5f));
}

void WalkingEngine::correctPendulumPlayer()
{
    if(balance && pendulumPlayer.phase.type != standPhase)
        applyCorrection(pendulumPlayer.phase, pendulumPlayer.nextPhase);
}

void WalkingEngine::updatePredictedPendulumPlayer()
{
    predictedPendulumPlayer = pendulumPlayer;

    // predict future pendulum state
    if(pendulumPlayer.phase.type != standPhase || pendulumPlayer.nextPhase.type != standPhase)
    {
        predictedPendulumPlayer.seek(observerMeasurementDelay * 0.001f);
    }
}

void WalkingEngine::generateTargetPosture()
{
    predictedPendulumPlayer.getPosture(targetPosture);
}

void WalkingEngine::generateJointRequest()
{
    // use head and arm joint angles
    jointRequest.angles[JointDataBH::HeadYaw] = targetPosture.headJointAngles[0];
    jointRequest.angles[JointDataBH::HeadPitch] = targetPosture.headJointAngles[1];
    jointRequest.angles[JointDataBH::LShoulderPitch] = targetPosture.leftArmJointAngles[0];
    jointRequest.angles[JointDataBH::LShoulderRoll] = targetPosture.leftArmJointAngles[1];
    jointRequest.angles[JointDataBH::LElbowYaw] = targetPosture.leftArmJointAngles[2];
    jointRequest.angles[JointDataBH::LElbowRoll] = targetPosture.leftArmJointAngles[3];
    jointRequest.angles[JointDataBH::RShoulderPitch] = targetPosture.rightArmJointAngles[0];
    jointRequest.angles[JointDataBH::RShoulderRoll] = targetPosture.rightArmJointAngles[1];
    jointRequest.angles[JointDataBH::RElbowYaw] = targetPosture.rightArmJointAngles[2];
    jointRequest.angles[JointDataBH::RElbowRoll] = targetPosture.rightArmJointAngles[3];

    // compute torso orientation
    bool transition = theMotionSelectionBH.ratios[MotionRequestBH::specialAction] > 0 || theMotionSelectionBH.ratios[MotionRequestBH::getUp] > 0 || theMotionSelectionBH.ratios[MotionRequestBH::kick] > 0;
    float additionalBodyRotation = (((targetPosture.rightOriginToCom.y - targetPosture.rightOriginToFoot.translation.y) - standComPosition.y) + ((targetPosture.leftOriginToCom.y - targetPosture.leftOriginToFoot.translation.y) + standComPosition.y)) * 0.5f;
    additionalBodyRotation *= 1.f / (22.5f - 50.f);
    additionalBodyRotation *= walkComBodyRotation;
    RotationMatrixBH bodyRotation(Vector3BH<>(additionalBodyRotation, 0.0f, 0.0f));
    bodyRotation *= standBodyRotation;

    float angularVelocityCorrection = 0.0f;
    if(!transition && /*theGroundContactStateBH.contact &&*/ theInertiaSensorDataBH.calibrated && theInertiaSensorDataBH.gyro.y != InertiaSensorDataBH::off)
    {
        // Buffer the relative rotations of the torso around its y-axis.
        const RotationMatrixBH relativeRotation = bodyRotation.invert() * lastBodyRotationMatrix;
        const float relativeRotationY = atan2(relativeRotation.c2.x, relativeRotation.c2.z);
        relativeRotations.add(relativeRotationY);

        // Calculate the moving average of the gyro measurements.
        if(lastSmoothedGyroY == InertiaSensorDataBH::off)
        {
            lastSmoothedGyroY = theInertiaSensorDataBH.gyro.y;
        }
        const float smoothedGyro = theInertiaSensorDataBH.gyro.y * gyroSmoothing + lastSmoothedGyroY * (1.0f - gyroSmoothing);

        // Use the difference between the buffered and measured angular velocities of the torso to calculate a
        // relative y-axis angle offset to control the torso's angular velocity.
        const int frameDelay = static_cast<int>(observerMeasurementDelay / (theFrameInfoBH.cycleTime * 1000.0f));
        ASSERT(frameDelay < 10);
        if(relativeRotations.getNumberOfEntries() == 10)
        {
            const float angularVelocityY = (relativeRotations.getEntry(frameDelay - 1) - relativeRotations.getEntry(frameDelay)) / theFrameInfoBH.cycleTime;
            PLOT("module:WalkingEngine:torsoAngularVelocityY", toDegrees(angularVelocityY));
            PLOT("module:WalkingEngine:gyroY", toDegrees(theInertiaSensorDataBH.gyro.y));
            PLOT("module:WalkingEngine:smoothedGyroY", toDegrees(smoothedGyro));
            const float errorY = angularVelocityY - smoothedGyro;
            const float approxDerivative = (errorY - lastGyroErrorY) / theFrameInfoBH.cycleTime;
            angularVelocityCorrection = gyroStateGain * errorY + gyroDerivativeGain * approxDerivative;
            PLOT("module:WalkingEngine:torsoAngularVelocityCorrectionY", toDegrees(angularVelocityCorrection));
        }
        lastSmoothedGyroY = smoothedGyro;
    }
    else
    {
        //std::cout << "IN TRANSITION" << std::endl;
        lastSmoothedGyroY = InertiaSensorDataBH::off;
        relativeRotations.init();
    }
    lastBodyRotationMatrix = bodyRotation;

    // compute foot position relative to the center of mass
    const Pose3DBH comToLeftOrigin = Pose3DBH(bodyRotation, targetPosture.leftOriginToCom).invert(); // TODO: optimize this by calculating the inverted left/rightOriginToCom pose directly
    const Pose3DBH comToRightOrigin = Pose3DBH(bodyRotation, targetPosture.rightOriginToCom).invert();
    const Pose3DBH comToLeftAnkle = Pose3DBH(comToLeftOrigin).conc(targetPosture.leftOriginToFoot).translate(0.f, 0.f, theRobotDimensionsBH.heightLeg5Joint);
    const Pose3DBH comToRightAnkle = Pose3DBH(comToRightOrigin).conc(targetPosture.rightOriginToFoot).translate(0.f, 0.f, theRobotDimensionsBH.heightLeg5Joint);

    // try to guess bodyToCom
    const Vector3BH<> averageComToAnkle = (comToLeftAnkle.translation + comToRightAnkle.translation) * 0.5f;
    Vector3BH<> bodyToComOffset = lastAverageComToAnkle != Vector3BH<>() ? (averageComToAnkle - lastAverageComToAnkle) * 0.4f : Vector3BH<>();
    lastAverageComToAnkle = averageComToAnkle;
    bodyToCom += bodyToComOffset;

    RobotModelBH robotModel;
    // find bodyToCom
    for(int i = 0; i < 7; ++i)
    {
        Pose3DBH bodyToLeftAnkle = Pose3DBH(bodyToCom).conc(comToLeftAnkle);
        Pose3DBH bodyToRightAnkle = Pose3DBH(bodyToCom).conc(comToRightAnkle);
        InverseKinematic::calcLegJoints(bodyToLeftAnkle, bodyToRightAnkle, jointRequest, theRobotDimensionsBH, 0.5f);
        robotModel.setJointData(jointRequest, theRobotDimensionsBH, theMassCalibrationBH); // TODO: improve this by not calculating the whole limb/mass model in each iteration

        Vector3BH<> delta = (robotModel.centerOfMass - bodyToCom) * 1.3f;
        bodyToCom += delta;
        if((abs(delta.x) < 0.05 && abs(delta.y) < 0.05 && abs(delta.z) < 0.05))
        {
            bodyToCom = robotModel.centerOfMass;
            break;
        }
    }

    // Correct the torso's angular velocity.
    Pose3DBH torsoInLeftFoot = robotModel.limbs[MassCalibrationBH::footLeft].invert();
    Pose3DBH torsoInRightFoot = robotModel.limbs[MassCalibrationBH::footRight].invert();
    torsoInLeftFoot.rotateY(angularVelocityCorrection);
    torsoInRightFoot.rotateY(angularVelocityCorrection);
    const Pose3DBH leftFootInTorso = torsoInLeftFoot.invert();
    const Pose3DBH rightFootInTorso = torsoInRightFoot.invert();
    InverseKinematic::calcLegJoints(leftFootInTorso, rightFootInTorso, jointRequest, theRobotDimensionsBH, 0.5f);

    // turn head joints off?
    if(theHeadJointRequestBH.pan == JointDataBH::off)
        jointRequest.angles[JointDataBH::HeadYaw] = JointDataBH::off;
    if(theHeadJointRequestBH.tilt == JointDataBH::off)
        jointRequest.angles[JointDataBH::HeadPitch] = JointDataBH::off;

    // set hardness
    jointRequest.jointHardness.hardness[JointDataBH::LAnklePitch] = theDamageConfigurationBH.weakLeftLeg ? 100 : standHardnessAnklePitch;
    jointRequest.jointHardness.hardness[JointDataBH::LAnkleRoll] = standHardnessAnkleRoll;
    jointRequest.jointHardness.hardness[JointDataBH::RAnklePitch] = theDamageConfigurationBH.weakRightLeg ? 100 : standHardnessAnklePitch;
    jointRequest.jointHardness.hardness[JointDataBH::RAnkleRoll] = standHardnessAnkleRoll;
}

void WalkingEngine::generateOutput(WalkingEngineOutputBH& walkingEngineOutput)
{
    if(pendulumPlayer.phase.type != standPhase)
    {
        const float stepDuration = (pendulumPlayer.phase.td - pendulumPlayer.nextPhase.tu) * 2.f;
        walkingEngineOutput.speed.translation = Vector2BH<>(pendulumPlayer.phase.s.translation.x + pendulumPlayer.nextPhase.s.translation.x, pendulumPlayer.phase.s.translation.y + pendulumPlayer.nextPhase.s.translation.y) / stepDuration;
        walkingEngineOutput.speed.rotation = (pendulumPlayer.phase.s.rotation + pendulumPlayer.nextPhase.s.rotation) / stepDuration;
    }
    else
        walkingEngineOutput.speed = Pose2DBH();
    walkingEngineOutput.odometryOffset = odometryOffset;

    walkingEngineOutput.upcomingOdometryOffset = upcomingOdometryOffset;
    walkingEngineOutput.upcomingOdometryOffsetValid = true;
    walkingEngineOutput.isLeavingPossible = currentMotionType == stand;
    if(pendulumPlayer.phase.type == standPhase)
        walkingEngineOutput.positionInWalkCycle = 0.f;
    else
    {
        const float duration = pendulumPlayer.phase.td - pendulumPlayer.phase.tu;
        walkingEngineOutput.positionInWalkCycle = 0.5f * (-pendulumPlayer.phase.tu / duration) + (pendulumPlayer.phase.type == leftSupportPhase ? 0.5f : 0.f);
    }

    walkingEngineOutput.standing = currentMotionType == stand;
    walkingEngineOutput.instability = 0.f;
    walkingEngineOutput.executedWalk = theMotionRequestBH.walkRequest;
    walkingEngineOutput.executedWalk.kickType = predictedPendulumPlayer.phase.kickType; //kickPlayer.isActive() ? kickPlayer.getType() : WalkRequest::none;
    (JointRequestBH&)walkingEngineOutput = jointRequest;
}

void WalkingEngine::generateDummyOutput(WalkingEngineOutputBH& walkingEngineOutput)
{
    walkingEngineOutput.standing = false;
    walkingEngineOutput.speed = Pose2DBH();
    walkingEngineOutput.odometryOffset = Pose2DBH();
    walkingEngineOutput.upcomingOdometryOffset = Pose2DBH();
    walkingEngineOutput.upcomingOdometryOffsetValid = true;
    walkingEngineOutput.isLeavingPossible = true;
    walkingEngineOutput.positionInWalkCycle = 0.f;
    walkingEngineOutput.instability = 0.f;
    walkingEngineOutput.executedWalk = WalkRequest();
    // leaving joint data untouched
}

void WalkingEngine::generateFirstPendulumPhase(WalkingEngine::PendulumPhase& phase)
{
    phase.type = standPhase;
    phase.k = walkK;
    phase.td = observerMeasurementDelay * (0.001f * 0.5f);
    phase.tu = -phase.td;
    phase.l = Vector3BH<>();
    phase.cl = Vector3BH<>();
    phase.s = StepSize();
    phase.rOpt = phase.rRef = phase.r = Vector2BH<>(walkRef.x, 0.f);
    phase.x0 = Vector2BH<>();
    phase.xv0 = Vector2BH<>();
    phase.toStand = false;
    phase.fromStand = false;
    phase.kickType = WalkRequest::none;
    phase.id = phaseBuffer.getNumberOfEntries() == 0 ? 1 : (phaseBuffer[0].id + 1);

    phaseBuffer.add(phase);
}

void WalkingEngine::generateNextPendulumPhase(const WalkingEngine::PendulumPhase& phase, WalkingEngine::PendulumPhase& nextPhase)
{
    // phase may have already been generated, if so do not generate it again
    unsigned int nextPhaseId = phase.id + 1;
    ASSERT(phaseBuffer.getNumberOfEntries() > 0);
    if(nextPhaseId <= phaseBuffer[0].id)
    {
        for(int i = 0; i < phaseBuffer.getNumberOfEntries(); ++i)
            if(phaseBuffer[i].id == nextPhaseId)
            {
                nextPhase = phaseBuffer[i];
                return;
            }
        ASSERT(false);
    }

    // get next phase type
    if(requestedMotionType == stepping && phase.type == standPhase)
    {
        nextPhase.type = ((theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode ? requestedWalkTarget.translation.y : theMotionRequestBH.walkRequest.speed.translation.y) > 0.f) ? rightSupportPhase : leftSupportPhase;
        // TODO: check sign
    }
    else if((requestedMotionType == stepping && !phase.toStand) || (phase.type != standPhase && requestedMotionType == stand && !phase.toStand))
    {
        ASSERT(phase.type == leftSupportPhase || phase.type == rightSupportPhase);
        nextPhase.type = phase.type == leftSupportPhase ? rightSupportPhase : leftSupportPhase;
    }
    else
        nextPhase.type = standPhase;

    // stand?
    if(nextPhase.type == standPhase)
    {
        ASSERT(phase.toStand || phase.type == standPhase);
        generateFirstPendulumPhase(nextPhase);
        ASSERT(nextPhase.type == standPhase);
        ASSERT(nextPhase.id == nextPhaseId);
        ASSERT(phaseBuffer[0].id == nextPhaseId);
        return;
    }

    // leftSupportPhase or rightSupportPhase
    nextPhase.id = nextPhaseId;
    const float sign = nextPhase.type == leftSupportPhase ? 1.f : -1.f;
    nextPhase.r = Vector2BH<>(walkRef.x, walkRef.y * sign);
    nextPhase.k = walkK;
    nextPhase.toStand = requestedMotionType == stand && !phase.fromStand;
    nextPhase.fromStand = phase.type == standPhase;
    nextPhase.kickType = WalkRequest::none;
    nextPhase.l = Vector3BH<>(walkLiftOffset.x, walkLiftOffset.y * sign, walkLiftOffset.z);
    nextPhase.cl = Vector3BH<>(walkComLiftOffset.x, walkComLiftOffset.y * sign, walkComLiftOffset.z);

    if(nextPhase.toStand && (abs(phase.s.translation.x) < walkStepSizeXPlanningLimit.max * 0.5f || theFallDownStateBH.state != FallDownStateBH::upright))
    {
        nextPhase.td = 0;
        nextPhase.tu = walkPhaseDuration * -0.5f;
        nextPhase.s = StepSize();
        nextPhase.x0.x = 0;
        nextPhase.x0.y = -nextPhase.r.y;
        nextPhase.xv0 = Vector2BH<>();
    }
    else
    {
        nextPhase.toStand = false;
        nextPhase.td = walkPhaseDuration * 0.5f;
        nextPhase.tu = walkPhaseDuration * -0.5f;

        float kickPhaseDuration = 0.f;
        if(nextPhase.fromStand || phase.fromStand)
        {
            nextPhase.l = Vector3BH<>();
            nextPhase.s = StepSize();
        }
        else if(phase.kickType != WalkRequest::none)
        {
            //if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
            //generateNextStepSize(nextPhase.type, nextPhase.s);
            //else
            nextPhase.s = StepSize();

            float additionRotation;
            Vector3BH<> additionalTranslation;
            kicks.getKickStepSize(phase.kickType, additionRotation, additionalTranslation);
            nextPhase.s.rotation += additionRotation;
            nextPhase.s.translation += additionalTranslation;
        }
        else if(theMotionRequestBH.walkRequest.kickType != WalkRequest::none && kicks.isKickMirrored(theMotionRequestBH.walkRequest.kickType) == (nextPhase.type == leftSupportPhase))
        {
            nextPhase.kickType = theMotionRequestBH.walkRequest.kickType;
            kickPhaseDuration = kicks.getKickStepDuration(nextPhase.kickType) * 0.5f;

            //if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
            //generateNextStepSize(nextPhase.type, nextPhase.s);
            //else
            nextPhase.s = StepSize();

            float additionRotation;
            Vector3BH<> additionalTranslation;
            kicks.getKickPreStepSize(nextPhase.kickType, additionRotation, additionalTranslation);
            nextPhase.r.x += kicks.getKickRefX(nextPhase.kickType, 0.f);
            nextPhase.s.rotation += additionRotation;
            nextPhase.s.translation += additionalTranslation;
        }
        else
        {
            generateNextStepSize(nextPhase.type, nextPhase.s);
        }

        nextPhase.r.x += abs(nextPhase.s.translation.x) * (walkRefAtFullSpeedX.x - walkRef.x) / (speedMax.translation.x * 0.5f);
        float rYSpeedFix = (abs(nextPhase.s.translation.x) * (walkRefAtFullSpeedX.y - walkRef.y) / (speedMax.translation.x * 0.5f));
        nextPhase.r.y += sign * rYSpeedFix;

        nextPhase.l.x += abs(nextPhase.s.translation.x) * (walkLiftOffsetAtFullSpeedX.x - walkLiftOffset.x) / (speedMax.translation.x * 0.5f);
        nextPhase.l.y += sign * (abs(nextPhase.s.translation.x) * (walkLiftOffsetAtFullSpeedX.y - walkLiftOffset.y) / (speedMax.translation.x * 0.5f));
        nextPhase.l.z += abs(nextPhase.s.translation.x) * (walkLiftOffsetAtFullSpeedX.z - walkLiftOffset.z) / (speedMax.translation.x * 0.5f);

        nextPhase.l.x += abs(nextPhase.s.translation.y) * (walkLiftOffsetAtFullSpeedY.x - walkLiftOffset.x) / (speedMax.translation.y);
        nextPhase.l.y += sign * (abs(nextPhase.s.translation.y) * (walkLiftOffsetAtFullSpeedY.y - walkLiftOffset.y) / (speedMax.translation.y));
        nextPhase.l.z += abs(nextPhase.s.translation.y) * (walkLiftOffsetAtFullSpeedY.z - walkLiftOffset.z) / (speedMax.translation.y);

        if(nextPhase.l.z > 0.f)
            nextPhase.lRotation = Vector3BH<>(
                    walkLiftRotation.x * sign * fabs(nextPhase.s.translation.y) / speedMax.translation.y,
                    nextPhase.s.translation.x > 0.f ? (walkLiftRotation.y * nextPhase.s.translation.x / (speedMax.translation.x * 0.5f)) : 0,
                    walkLiftRotation.z * sign);

        const float walkPhaseDurationX = walkPhaseDuration + abs(nextPhase.s.translation.x) * (walkPhaseDurationAtFullSpeedX - walkPhaseDuration) / (speedMax.translation.x * 0.5f);
        const float walkPhaseDurationY = walkPhaseDurationX + abs(nextPhase.s.translation.y) * (walkPhaseDurationAtFullSpeedY - walkPhaseDuration) / speedMax.translation.y;
        computeNextPendulumParamtersY(nextPhase, walkPhaseDurationX, kickPhaseDuration != 0.f ? kickPhaseDuration : walkPhaseDurationY);
        computeNextPendulumParamtersX(nextPhase);
    }

    nextPhase.rOpt = nextPhase.rRef = nextPhase.r;
    phaseBuffer.add(nextPhase);
}

void WalkingEngine::updatePendulumPhase(PendulumPhase& phase, PendulumPhase& nextPhase, bool init) const
{
    if(phase.type == standPhase)
    {
        ASSERT(nextPhase.type == standPhase || nextPhase.fromStand);
        ASSERT(fabs(phase.xv0.x) < 0.1f && fabs(phase.xv0.y) < 0.1f);
        ASSERT(nextPhase.s.translation == Vector3BH<>());
        return;
    }

    //
#ifndef NDEBUG
    Vector2BH<> px = phase.r + phase.x0;
    Vector2BH<> pxv = phase.xv0;
#endif

    updatePendulumParametersY(phase, nextPhase);
    updatePendulumParametersX(phase, nextPhase, init);

    ASSERT(nextPhase.tu < 0.f);
    ASSERT(nextPhase.x0.x == 0.f || nextPhase.toStand);
    ASSERT(nextPhase.xv0.y == 0.f);

    ASSERT(fabs(phase.r.y + phase.x0.y * cosh(phase.k.y * phase.td) + phase.xv0.y * sinh(phase.k.y * phase.td) / phase.k.y -
                (nextPhase.s.translation.y + nextPhase.r.y + nextPhase.x0.y * cosh(nextPhase.k.y * nextPhase.tu))) < (phase.td > 0.8f ? 1.f : 0.1f));
    ASSERT(fabs(phase.x0.y * phase.k.y * sinh(phase.k.y * phase.td) + phase.xv0.y * cosh(phase.k.y * phase.td) -
                nextPhase.x0.y * nextPhase.k.y * sinh(nextPhase.k.y * nextPhase.tu)) < (phase.td > 0.8f ? 1.f : 0.1f));

    //ASSERT(fabs(phase.r.x + phase.x0.x * cosh(phase.k.x * phase.td) + phase.xv0.x * sinh(phase.k.x * phase.td) / phase.k.x -
    //  (nextPhase.s.translation.x + nextPhase.r.x + nextPhase.x0.x * cosh(nextPhase.k.x * nextPhase.tu) + nextPhase.xv0.x * sinh(nextPhase.k.x * nextPhase.tu) / nextPhase.k.x)) < (phase.td > 0.8f ? 1.f : 0.1f));
    //ASSERT(fabs(phase.x0.x * phase.k.x * sinh(phase.k.x * phase.td) + phase.xv0.x * cosh(phase.k.x * phase.td) -
    //  (nextPhase.x0.x * nextPhase.k.x * sinh(nextPhase.k.x * nextPhase.tu) + nextPhase.xv0.x * cosh(nextPhase.k.x * nextPhase.tu))) < (phase.td > 0.8f ? 1.f : 0.1f));

    ASSERT(fabs(phase.r.x + phase.x0.x - px.x) < 0.1f || phase.toStand);
    ASSERT(fabs(phase.r.y + phase.x0.y - px.y) < 0.1f || phase.toStand);
    ASSERT(fabs(phase.xv0.x - pxv.x) < 0.1f || phase.toStand);
    ASSERT(fabs(phase.xv0.y - pxv.y) < 0.1f || phase.toStand);

}

void WalkingEngine::computeNextPendulumParamtersY(PendulumPhase& nextPhase, float walkPhaseDurationX, float walkPhaseDurationY) const
{
    // compute tu and td using walkPhaseDurationX
    nextPhase.td = walkPhaseDurationX * 0.5f;
    nextPhase.tu = -nextPhase.td;

    // compute x0 using walkPhaseDurationY
    const float td = walkPhaseDurationY * 0.5f;
    const float r = nextPhase.r.y;
    const float k = nextPhase.k.y;
    ASSERT(cosh(k * td) != 0.f);
    const float x0  = -r / cosh(k * td);
    nextPhase.x0.y = x0;
    nextPhase.xv0.y = 0.f;
    ASSERT(fabs(r + x0 * cosh(k * td)) < 0.1f);
}

void WalkingEngine::computeNextPendulumParamtersX(PendulumPhase& nextPhase) const
{
    const float td = nextPhase.td;
    const float ns =  nextPhase.s.translation.x;
    const float k = nextPhase.k.x;
    ASSERT(sinh(k * td) != 0.f);
    const float xv0 = ns * 0.5f * k / sinh(k * td);
    nextPhase.x0.x = 0.f;
    nextPhase.xv0.x = xv0;
    ASSERT(fabs((xv0 * sinh(k * td) / k) - (ns * 0.5f)) < 0.1f);
}

/*
   void WalkingEngine::generateNextStepSize(PhaseType nextSupportLeg, WalkingEngine::StepSize& nextStepSize)
   {
   ASSERT(nextSupportLeg == leftSupportPhase || nextSupportLeg == rightSupportPhase);

   if(requestedMotionType != stepping)
   {
   nextStepSize = StepSize();
   return;
   }

// get requested walk target and speed
Pose2DBH walkTarget = requestedWalkTarget;
Pose2DBH requestedSpeed = theMotionRequestBH.walkRequest.speed;
if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode) // remove upcoming odometry offset
{
walkTarget -= upcomingOdometryOffset;

requestedSpeed = Pose2DBH(walkTarget.rotation * 2.f / odometryScale.rotation, walkTarget.translation.x * 2.f / odometryScale.translation.x, walkTarget.translation.y * 2.f / odometryScale.translation.y);

if(theMotionRequestBH.walkRequest.speed.translation.x == 0.f)
requestedSpeed.translation.x = 0.f;
if(theMotionRequestBH.walkRequest.speed.translation.y == 0.f)
requestedSpeed.translation.y = 0.f;
if(theMotionRequestBH.walkRequest.speed.rotation == 0.f)
requestedSpeed.rotation = 0.f;
}
else if(theMotionRequestBH.walkRequest.mode == WalkRequest::percentageSpeedMode)
{
requestedSpeed.rotation *= speedMax.rotation;
requestedSpeed.translation.x *= (theMotionRequestBH.walkRequest.speed.translation.x >= 0.f ? speedMax.translation.x : speedMaxBackwards);
requestedSpeed.translation.y *= speedMax.translation.y;
}

// reduce speed for target walks near the target to handle limited deceleration
if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
{
float maxSpeedForTargetX = sqrt(2.f * abs(requestedSpeed.translation.x) * speedMaxChange.translation.x);
if(abs(requestedSpeed.translation.x) > maxSpeedForTargetX)
requestedSpeed.translation.x = requestedSpeed.translation.x >= 0.f ? maxSpeedForTargetX : -maxSpeedForTargetX;

float maxSpeedForTargetY = sqrt(2.f * abs(requestedSpeed.translation.y) * speedMaxChange.translation.y);
if(abs(requestedSpeed.translation.y) > maxSpeedForTargetY)
requestedSpeed.translation.y = requestedSpeed.translation.y >= 0.f ? maxSpeedForTargetY : -maxSpeedForTargetY;

float maxSpeedForTargetR = sqrt(2.f * abs(requestedSpeed.rotation) * speedMaxChange.rotation);
if(abs(requestedSpeed.rotation) > maxSpeedForTargetR)
requestedSpeed.rotation = requestedSpeed.rotation >= 0.f ? maxSpeedForTargetR : -maxSpeedForTargetR;
}

//
float skalarSpeed = 1.f;
const Pose2DBH maxSpeed(speedMax.rotation, requestedSpeed.translation.x < 0.f ? speedMaxBackwards : speedMax.translation.x, speedMax.translation.y);

//
ASSERT(skalarSpeed > 0.f);
Pose2DBH vRel(requestedSpeed.rotation / maxSpeed.rotation, requestedSpeed.translation.x / maxSpeed.translation.x, requestedSpeed.translation.y / maxSpeed.translation.y);
if(fabs(vRel.rotation) > skalarSpeed)
vRel.rotation /= fabs(vRel.rotation);
float maxTransRel = std::min(std::max(std::max(skalarSpeed, minRotationToReduceStepSize) - fabs(vRel.rotation), 0.f), 1.f);
if(vRel.translation.abs() > maxTransRel)
vRel.translation.normalizeBH(maxTransRel);

// max rotation speed change clipping (rotation-only)
vRel.rotation = RangeBH<>(lastRequestedSpeedRel.rotation - speedMaxChange.rotation / maxSpeed.rotation, lastRequestedSpeedRel.rotation + speedMaxChange.rotation / maxSpeed.rotation).limit(vRel.rotation);

//
const Pose2DBH& vRelOld = lastRequestedSpeedRel;
Pose2DBH vRelLimitedWithOld = vRel;
maxTransRel = std::min(std::max(std::max(1.f, minRotationToReduceStepSize) - fabs(vRelOld.rotation), 0.f), 1.f);
if(vRelLimitedWithOld.translation.abs() > maxTransRel)
vRelLimitedWithOld.translation.normalizeBH(maxTransRel);
float maxRotRel = 1 - vRelOld.translation.abs();
if(fabs(vRelLimitedWithOld.rotation) > maxRotRel)
    vRelLimitedWithOld.rotation *= vRelLimitedWithOld.rotation == 0.f ? 0.f  : maxRotRel / fabs(vRelLimitedWithOld.rotation);
    vRelLimitedWithOld.translation.x = min(fabs(vRelLimitedWithOld.translation.x), sqrt(1.f - sqrBH(vRelOld.translation.y))) * sgnBH(vRelLimitedWithOld.translation.x);
    vRelLimitedWithOld.translation.y = min(fabs(vRelLimitedWithOld.translation.y), sqrt(1.f - sqrBH(vRelOld.translation.x))) * sgnBH(vRelLimitedWithOld.translation.y);
    lastRequestedSpeedRel = vRel;

    //
    requestedSpeed = Pose2DBH(vRelLimitedWithOld.rotation * maxSpeed.rotation, vRelLimitedWithOld.translation.x * maxSpeed.translation.x, vRelLimitedWithOld.translation.y * maxSpeed.translation.y);

    // generate step size from requested walk speed
    nextStepSize = StepSize(requestedSpeed.rotation, requestedSpeed.translation.x * 0.5f, requestedSpeed.translation.y);

    // just move the outer foot, when walking sidewards or when rotating
if((nextStepSize.translation.y < 0.f && nextSupportLeg == leftSupportPhase) || (nextStepSize.translation.y > 0.f && nextSupportLeg != leftSupportPhase))
    nextStepSize.translation.y = 0.f;
if((nextStepSize.rotation < 0.f && nextSupportLeg == leftSupportPhase) || (nextStepSize.rotation > 0.f && nextSupportLeg != leftSupportPhase))
    nextStepSize.rotation = 0.f;

    // clip to walk target
if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
{
    if((nextStepSize.translation.x > 0.f && walkTarget.translation.x > 0.f && nextStepSize.translation.x * odometryScale.translation.x > walkTarget.translation.x) || (nextStepSize.translation.x < 0.f && walkTarget.translation.x < 0.f && nextStepSize.translation.x * odometryScale.translation.x < walkTarget.translation.x))
        nextStepSize.translation.x = walkTarget.translation.x / odometryScale.translation.x;
    if((nextStepSize.translation.y > 0.f && walkTarget.translation.y > 0.f && nextStepSize.translation.y * odometryScale.translation.y > walkTarget.translation.y) || (nextStepSize.translation.y < 0.f && walkTarget.translation.y < 0.f && nextStepSize.translation.y * odometryScale.translation.y < walkTarget.translation.y))
        nextStepSize.translation.y = walkTarget.translation.y / odometryScale.translation.y;
    if((nextStepSize.rotation > 0.f && walkTarget.rotation > 0.f && nextStepSize.rotation * odometryScale.rotation > walkTarget.rotation) || (nextStepSize.rotation < 0.f && walkTarget.rotation < 0.f && nextStepSize.rotation * odometryScale.rotation < walkTarget.rotation))
        nextStepSize.rotation = walkTarget.rotation / odometryScale.rotation;
}
}
    */
void WalkingEngine::generateNextStepSize(PhaseType nextSupportLeg, WalkingEngine::StepSize& nextStepSize)
{
    ASSERT(nextSupportLeg == leftSupportPhase || nextSupportLeg == rightSupportPhase);

    const Pose2DBH speedMaxMin(0.2f, 20.f, 0.f);

    if(requestedMotionType != stepping)
    {
        nextStepSize = StepSize();
        return;
    }

    // get requested walk target and speed
    Pose2DBH walkTarget = requestedWalkTarget;
    Pose2DBH requestedSpeed = theMotionRequestBH.walkRequest.speed;
    if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode) // remove upcoming odometry offset
    {
        walkTarget -= upcomingOdometryOffset;

        if ((fabs(walkTarget.translation.x) < 30) && (fabs(walkTarget.translation.y) < 30)) {
                    finishedWithTarget = true;
                }

        requestedSpeed = Pose2DBH(walkTarget.rotation * 2.f / odometryScale.rotation, walkTarget.translation.x * 2.f / odometryScale.translation.x, walkTarget.translation.y * 2.f / odometryScale.translation.y);

        if(theMotionRequestBH.walkRequest.speed.translation.x == 0.f)
            requestedSpeed.translation.x = 0.f;
        if(theMotionRequestBH.walkRequest.speed.translation.y == 0.f)
            requestedSpeed.translation.y = 0.f;
        if(theMotionRequestBH.walkRequest.speed.rotation == 0.f)
            requestedSpeed.rotation = 0.f;
    }
    else if(theMotionRequestBH.walkRequest.mode == WalkRequest::percentageSpeedMode)
    {
        requestedSpeed.rotation *= speedMax.rotation;
        requestedSpeed.translation.x *= (theMotionRequestBH.walkRequest.speed.translation.x >= 0.f ? speedMax.translation.x : speedMaxBackwards);
        requestedSpeed.translation.y *= speedMax.translation.y;
    }

    // reduce speed for target walks near the target to handle limited deceleration
    if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
    {
        float maxSpeedForTargetX = sqrt(2.f * abs(requestedSpeed.translation.x) * speedMaxChange.translation.x);
        if(abs(requestedSpeed.translation.x) > maxSpeedForTargetX)
            requestedSpeed.translation.x = requestedSpeed.translation.x >= 0.f ? maxSpeedForTargetX : -maxSpeedForTargetX;

        float maxSpeedForTargetY = sqrt(2.f * abs(requestedSpeed.translation.y) * speedMaxChange.translation.y);
        if(abs(requestedSpeed.translation.y) > maxSpeedForTargetY)
            requestedSpeed.translation.y = requestedSpeed.translation.y >= 0.f ? maxSpeedForTargetY : -maxSpeedForTargetY;

        float maxSpeedForTargetR = sqrt(2.f * abs(requestedSpeed.rotation) * speedMaxChange.rotation);
        if(abs(requestedSpeed.rotation) > maxSpeedForTargetR)
            requestedSpeed.rotation = requestedSpeed.rotation >= 0.f ? maxSpeedForTargetR : -maxSpeedForTargetR;
    }

    // normalizeBH x-y-speed
    const Pose2DBH maxSpeed(speedMax.rotation, requestedSpeed.translation.x < 0.f ? speedMaxBackwards : speedMax.translation.x, speedMax.translation.y);
    {
        Vector2BH<> tmpSpeed(
                requestedSpeed.translation.x / (speedMaxMin.translation.x + maxSpeed.translation.x),
                requestedSpeed.translation.y / (speedMaxMin.translation.y + maxSpeed.translation.y));
        const float tmpSpeedAbs = tmpSpeed.abs();
        if(tmpSpeedAbs > 1.f)
        {
            tmpSpeed /= tmpSpeedAbs;
            tmpSpeed.x *= (speedMaxMin.translation.x + maxSpeed.translation.x);
            tmpSpeed.y *= (speedMaxMin.translation.y + maxSpeed.translation.y);
            requestedSpeed.translation.x = RangeBH<>(-maxSpeed.translation.x, maxSpeed.translation.x).limit(tmpSpeed.x);
            requestedSpeed.translation.y = RangeBH<>(-maxSpeed.translation.y, maxSpeed.translation.y).limit(tmpSpeed.y);
        }
    }

    // normalizeBH speed (including rotation)
    {
        Vector3BH<> tmpSpeed(
                requestedSpeed.translation.x / (speedMaxMin.translation.x + maxSpeed.translation.x),
                requestedSpeed.translation.y / (speedMaxMin.translation.y + maxSpeed.translation.y),
                requestedSpeed.rotation / (speedMaxMin.rotation + maxSpeed.rotation));
        const float tmpSpeedAbs = tmpSpeed.abs();
        if(tmpSpeedAbs > 1.f)
        {
            tmpSpeed /= tmpSpeedAbs;
            tmpSpeed.x *= (speedMaxMin.translation.x + maxSpeed.translation.x);
            tmpSpeed.y *= (speedMaxMin.translation.y + maxSpeed.translation.y);
            tmpSpeed.z *= (speedMaxMin.rotation + maxSpeed.rotation);
            requestedSpeed.translation.x = RangeBH<>(-maxSpeed.translation.x, maxSpeed.translation.x).limit(tmpSpeed.x);
            requestedSpeed.translation.y = RangeBH<>(-maxSpeed.translation.y, maxSpeed.translation.y).limit(tmpSpeed.y);
            requestedSpeed.rotation = RangeBH<>(-maxSpeed.rotation, maxSpeed.rotation).limit(tmpSpeed.z);
        }
    }

    // max rotation speed change clipping (rotation-only)
    requestedSpeed.rotation = RangeBH<>(lastSelectedSpeed.rotation - speedMaxChange.rotation, lastSelectedSpeed.rotation + speedMaxChange.rotation).limit(requestedSpeed.rotation);
    lastSelectedSpeed = requestedSpeed;

    // clip requested walk speed to a target walk speed limit
    if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
    {
        requestedSpeed.translation.x = RangeBH<>(-speedMax.translation.x * theMotionRequestBH.walkRequest.speed.translation.x, speedMax.translation.x * theMotionRequestBH.walkRequest.speed.translation.x).limit(requestedSpeed.translation.x);
        requestedSpeed.translation.y = RangeBH<>(-speedMax.translation.y * theMotionRequestBH.walkRequest.speed.translation.y, speedMax.translation.y * theMotionRequestBH.walkRequest.speed.translation.y).limit(requestedSpeed.translation.y);
        requestedSpeed.rotation = RangeBH<>(-speedMax.rotation * theMotionRequestBH.walkRequest.speed.rotation, speedMax.rotation * theMotionRequestBH.walkRequest.speed.rotation).limit(requestedSpeed.rotation);
    }

    // generate step size from requested walk speed
    nextStepSize = StepSize(requestedSpeed.rotation, requestedSpeed.translation.x * 0.5f, requestedSpeed.translation.y);

    // just move the outer foot, when walking sidewards or when rotating
    if((nextStepSize.translation.y < 0.f && nextSupportLeg == leftSupportPhase) || (nextStepSize.translation.y > 0.f && nextSupportLeg != leftSupportPhase))
        nextStepSize.translation.y = 0.f;
    if((nextStepSize.rotation < 0.f && nextSupportLeg == leftSupportPhase) || (nextStepSize.rotation > 0.f && nextSupportLeg != leftSupportPhase))
        nextStepSize.rotation = 0.f;

    // clip to walk target
    if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
    {
        if((nextStepSize.translation.x > 0.f && walkTarget.translation.x > 0.f && nextStepSize.translation.x * odometryScale.translation.x > walkTarget.translation.x) || (nextStepSize.translation.x < 0.f && walkTarget.translation.x < 0.f && nextStepSize.translation.x * odometryScale.translation.x < walkTarget.translation.x))
            nextStepSize.translation.x = walkTarget.translation.x / odometryScale.translation.x;
        if((nextStepSize.translation.y > 0.f && walkTarget.translation.y > 0.f && nextStepSize.translation.y * odometryScale.translation.y > walkTarget.translation.y) || (nextStepSize.translation.y < 0.f && walkTarget.translation.y < 0.f && nextStepSize.translation.y * odometryScale.translation.y < walkTarget.translation.y))
            nextStepSize.translation.y = walkTarget.translation.y / odometryScale.translation.y;
        if((nextStepSize.rotation > 0.f && walkTarget.rotation > 0.f && nextStepSize.rotation * odometryScale.rotation > walkTarget.rotation) || (nextStepSize.rotation < 0.f && walkTarget.rotation < 0.f && nextStepSize.rotation * odometryScale.rotation < walkTarget.rotation))
            nextStepSize.rotation = walkTarget.rotation / odometryScale.rotation;
    }
}


void WalkingEngine::computeOdometryOffset()
{
    {
        Pose3DBH footLeft = Pose3DBH(theRobotModelBH.limbs[MassCalibrationBH::footLeft]).translate(0.f, 0.f, -theRobotDimensionsBH.heightLeg5Joint);
        Pose3DBH footRight = Pose3DBH(theRobotModelBH.limbs[MassCalibrationBH::footRight]).translate(0.f, 0.f, -theRobotDimensionsBH.heightLeg5Joint);
        Vector3BH<> odometryOrigin = (footLeft.translation + footRight.translation) * 0.5f;
        if(lastOdometryOrigin.z != 0.f)
        {
            Pose3DBH& footSupport = pendulumPlayer.phase.type == leftSupportPhase ? footLeft : footRight;
            Pose3DBH& lastFootSupport = pendulumPlayer.phase.type == leftSupportPhase ? lastFootLeft : lastFootRight;
            Pose3DBH odometryOffset3DinP = (Pose3DBH(-odometryOrigin).conc(footSupport).conc(lastFootSupport.invert()).conc(lastOdometryOrigin)).invert();
            Pose3DBH odometryOffset3D = Pose3DBH(theTorsoMatrixBH).conc(odometryOffset3DinP).conc(theTorsoMatrixBH.invert());
            odometryOffset.rotation = odometryOffset3D.rotation.getZAngle() * odometryScale.rotation;
            odometryOffset.translation.x = odometryOffset3D.translation.x * odometryScale.translation.x;
            odometryOffset.translation.y = odometryOffset3D.translation.y * odometryScale.translation.y;
        }
        else
            odometryOffset = Pose2DBH();
        lastFootLeft = footLeft;
        lastFootRight = footRight;
        lastOdometryOrigin = odometryOrigin;
    }

    // compute upcoming odometry offset
    upcomingOdometryOffset = Pose2DBH((pendulumPlayer.nextPhase.s.rotation - observedStepOffset.rotation) * 0.5f * odometryScale.rotation,
            (pendulumPlayer.nextPhase.s.translation.x - observedStepOffset.translation.x) * 0.5f * odometryScale.translation.x,
            (pendulumPlayer.nextPhase.s.translation.y - observedStepOffset.translation.y) * 0.5f * odometryScale.translation.y);
    upcomingOdometryOffset += Pose2DBH(pendulumPlayer.nextPhase.s.rotation * 0.5f * odometryScale.rotation,
            pendulumPlayer.nextPhase.s.translation.x * 0.5f * odometryScale.translation.x,
            pendulumPlayer.nextPhase.s.translation.y * 0.5f * odometryScale.translation.y);
    if(predictedPendulumPlayer.nextPhase.id > pendulumPlayer.nextPhase.id)
    {
        Pose2DBH upcomingOdometryOffsetNextPhase(predictedPendulumPlayer.nextPhase.s.rotation * odometryScale.rotation,
                predictedPendulumPlayer.nextPhase.s.translation.x * odometryScale.translation.x,
                predictedPendulumPlayer.nextPhase.s.translation.y * odometryScale.translation.y);
        upcomingOdometryOffset += upcomingOdometryOffsetNextPhase;
    }

    // remove odometry offset from requested walk target
    if(theMotionRequestBH.walkRequest.mode == WalkRequest::targetMode)
        requestedWalkTarget -= odometryOffset;
}

void WalkingEngine::applyCorrection(PendulumPhase& phase, PendulumPhase& nextPhase)
{
    Vector3BH<>* directError, * indirectError;
    if(phase.type == leftSupportPhase)
    {
        directError = &errorLeft;
        indirectError = &errorRight;
    }
    else
    {
        ASSERT(phase.type == rightSupportPhase);
        directError = &errorRight;
        indirectError = &errorLeft;
    }

    if(phase.type != standPhase)
    {
        const Vector2BH<> px = phase.r + phase.x0;
        const Vector2BH<> xv = phase.xv0;
        const Vector2BH<> xa(phase.x0.x * phase.k.x * phase.k.x, phase.x0.y * phase.k.y * phase.k.y);

        measuredPx = Vector2BH<>(px.x + directError->x, px.y + directError->y);
        const Vector2BH<> measuredPx = Vector2BH<>(px.x + directError->x * balanceCom.x, px.y + directError->y * balanceCom.y);
        Vector2BH<> measuredXv = xv + (measuredPx - px) / theFrameInfoBH.cycleTime;
        const Vector2BH<> measuredXa = xa + (measuredXv - xv) / theFrameInfoBH.cycleTime;

        const Vector2BH<> measuredX(measuredXa.x / (phase.k.x * phase.k.x), measuredXa.y / (phase.k.y * phase.k.y));
        measuredR = measuredPx - measuredX;

        measuredXv = Vector2BH<>(xv.x + errorVelocity.x * balanceComVelocity.x, xv.y + errorVelocity.y * balanceComVelocity.y);


        const float rYSign = phase.type == leftSupportPhase ? 1.f : -1.f;
        RangeBH<> rYLimit = RangeBH<>(phase.rOpt.y + walkRefYLimit.min * rYSign).add(phase.rOpt.y + walkRefYLimit.max * rYSign);

        Vector2BH<> newR = phase.r;

        // method #1: use measured ZMP as new ref
        //newR = measuredR; // ???
        //newR.y = rYLimit.limit(newR.y);

        // method #2: p-control
        newR.x = phase.rRef.x - (measuredR.x - phase.rRef.x) * balanceRef.x; // ????
        newR.y = phase.rRef.y - (measuredR.y - phase.rRef.y) * balanceRef.y; // ????
        newR.y = rYLimit.limit(newR.y);

        /*
        // method #3: i-control
        newR.x -= (measuredR.x - phase.rRef.x) * balanceRef.x; // ????
        newR.y -= (measuredR.y - phase.rRef.y) * balanceRef.x; // ????
        newR.y = rYLimit.limit(newR.y);


        // method #4:
        // find r, x0, ntu in
        // r + x0 = px
        // r + x0 * cosh(k * td) + xv0 * sinh(k * td) / k = ns + nr + nx0 * cosh(nk * ntu)
        // x0 * k * sinh(k * td) + xv0 * cosh(k * td) = nx0 * nk * sinh(nk * ntu)

        struct Data : public FunctionMinimizer
        {
        float px;
        float xv0;
        float k;
        float nx0;
        float nk;
        float nr;
        float ns;
        float coshKTd;
        float sinhKTd;

        virtual float func(float r) const
        {
        const float x = px - r;
        const float xvd = x * k * sinhKTd + xv0 * coshKTd;
        const float nkNtu = asinh(xvd / (nx0 * nk));
        if(nkNtu < 0.f)
        return fabs(r + x * coshKTd + xv0 * sinhKTd / k - (ns + nr + nx0 * cosh(nkNtu)));
        else
        return fabs(r + x * coshKTd + xv0 * sinhKTd / k - (ns + nr + nx0)) + nkNtu;
        }
        } d;

        d.px = measuredPx.y;
        d.xv0 = measuredXv.y;
        d.k = phase.k.y;
        d.nx0 = nextPhase.x0.y;
        d.nk = nextPhase.k.y;
        d.nr = nextPhase.r.y;
        d.ns = nextPhase.s.translation.y;
        d.coshKTd = cosh(d.k * phase.td);
        d.sinhKTd = sinh(d.k * phase.td);

        bool clipped;
        newR.y = d.minimize(rYLimit.min, rYLimit.max, phase.r.y, 0.1f, 0.05f, clipped, "applyCorrection");
         */

        Vector2BH<> newX = measuredPx - newR;
        Vector2BH<> newXv = measuredXv;

        phase.x0 = newX;
        phase.xv0 = newXv;
        phase.r = newR;
    }

    if(nextPhase.type != standPhase && !nextPhase.toStand)
    {
        //nextPhase.r.x += indirectError->x * balanceNextRef.x;
        //nextPhase.r.y += indirectError->y * balanceNextRef.y;

        nextPhase.r.x = nextPhase.rOpt.x + indirectError->x * balanceNextRef.x;
        nextPhase.r.y = nextPhase.rOpt.y + indirectError->y * balanceNextRef.y;

        RangeBH<> nrXLimit(nextPhase.rOpt.x + walkRefXLimit.min, nextPhase.rOpt.x + walkRefXLimit.max);
        const float nrYSign = nextPhase.type == leftSupportPhase ? 1.f : -1.f;
        RangeBH<> nrYLimit = RangeBH<>(nextPhase.rOpt.y + walkRefYLimit.min * nrYSign).add(nextPhase.rOpt.y + walkRefYLimit.max * nrYSign);

        nextPhase.r.x = nrXLimit.limit(nextPhase.r.x);
        nextPhase.r.y = nrYLimit.limit(nextPhase.r.y);


        //nextPhase.s.translation.x += indirectError->x * balanceStepSize.x;
        float nsxapprox = 2.f * nextPhase.xv0.x * cosh(nextPhase.k.x * nextPhase.tu);
        nsxapprox += indirectError->x * balanceStepSize.x;
        nextPhase.xv0.x = nsxapprox / (2.f * cosh(nextPhase.k.x * nextPhase.tu));


        /*
           float nsu = nextPhase.xv0.x * sinh(nextPhase.k.x * nextPhase.tu) / nextPhase.k.x;
           nsu += indirectError->x * balanceStepSize.x * 0.5f;
           nextPhase.xv0.x = nsu  / (sinh(nextPhase.k.x * nextPhase.tu) / nextPhase.k.x);
         */
        /*
           float nsu = nextPhase.r.x + nextPhase.xv0.x * sinh(nextPhase.k.x * nextPhase.tu) / nextPhase.k.x;
           float ns = phase.r.x + phase.x0.x * cosh(phase.k.x * phase.td) + phase.xv0.x * sinh(phase.k.x * phase.td) / phase.k.x - nsu;
        //nextPhase.s.translation.x = ns + indirectError->x * balanceStepSize.x;
        nextPhase.s.translation.x = ns + directError->x * balanceStepSize.x;
         */

        nextPhase.s.translation.y += indirectError->y * balanceStepSize.y;
    }

    repairPendulumParametersY(phase, nextPhase);
    updatePendulumPhase(phase, nextPhase, false);
}

void WalkingEngine::PendulumPlayer::seek(float deltaTime)
{
    for(;;)
    {
        float dt = deltaTime;
        if(dt > phase.td)
            dt = phase.td;
        /*
           if(dt > 0.005f)
           dt = 0.005f;
         */
        deltaTime -= dt;

        phase.td -= dt;
        phase.tu -= dt;
        switch(phase.type)
        {
            case standPhase:
                ASSERT(fabs(phase.xv0.x) < 0.1f);
                ASSERT(fabs(phase.xv0.y) < 0.1f);
                ASSERT(fabs(phase.x0.x) < 0.1f);
                ASSERT(fabs(phase.x0.y) < 0.1f);
                break;

            case leftSupportPhase:
            case rightSupportPhase:

                /*
                   if(!phase.toStand && !nextPhase.toStand)
                   {
                   float rotationOffset;
                   {
                   const float ratioNew = -phase.tu / (phase.td - phase.tu);
                   const float ratioOld = (-phase.tu - dt) / (phase.td - phase.tu);
                   const float swingMoveFadeInNew = ratioNew < engine->walkMovePhase.start ? 0.f : ratioNew > engine->walkMovePhase.start + engine->walkMovePhase.duration ? 1.f : blend((ratioNew - engine->walkMovePhase.start) / engine->walkMovePhase.duration);
                   const float swingMoveFadeInOld = ratioOld < engine->walkMovePhase.start ? 0.f : ratioOld > engine->walkMovePhase.start + engine->walkMovePhase.duration ? 1.f : blend((ratioOld - engine->walkMovePhase.start) / engine->walkMovePhase.duration);
                   const float swingMoveFadeOutNew = (1.f - swingMoveFadeInNew);
                   const float swingMoveFadeOutOld = (1.f - swingMoveFadeInOld);
                   rotationOffset = (nextPhase.s.rotation * (swingMoveFadeInNew - swingMoveFadeInOld) - phase.s.rotation * (swingMoveFadeOutNew - swingMoveFadeOutOld)) * -0.5f;
                   }

                //rotationOffset *= 0.5f; // ?? falsch!?

                {
                phase.xv0.rotate(rotationOffset);
                Vector2BH<> p(0.f, phase.type == leftSupportPhase ? engine->standComPosition.y : -engine->standComPosition.y);
                Vector2BH<> np(0.f, -p.y);
                phase.x0 = (-p + phase.r + phase.x0).rotate(rotationOffset) + p - phase.r;

                float tdOld = phase.td + dt;
                float xvd = phase.x0.x * phase.k.x * sinh(phase.k.x * tdOld) + phase.xv0.x * cosh(phase.k.x * tdOld);
                nextPhase.xv0.x = xvd / cosh(nextPhase.k.x * nextPhase.tu);
                }
                }
                 */

                // y
                {
                    const float k = phase.k.y;
                    const float kDt = k * dt;
                    const float coshKDt = cosh(kDt);
                    const float sinhKDt = sinh(kDt);
                    const float x0 = phase.x0.y;
                    const float xv0 = phase.xv0.y;
                    phase.x0.y = x0 * coshKDt + xv0 / k * sinhKDt;
                    phase.xv0.y = x0 * k * sinhKDt + xv0 * coshKDt;
                }

                // x
                {
                    const float k = phase.k.x;
                    const float kDt = k * dt;
                    const float coshKDt = cosh(kDt);
                    const float sinhKDt = sinh(kDt);
                    const float x0 = phase.x0.x;
                    const float xv0 = phase.xv0.x;
                    phase.x0.x = x0 * coshKDt + xv0 / k * sinhKDt;
                    phase.xv0.x = x0 * k * sinhKDt + xv0 * coshKDt;
                }

                if(kickPlayer.isActive() && deltaTime == 0)
                    kickPlayer.seekTo(-phase.tu / (phase.td - phase.tu) * kickPlayer.getLength());

                break;

            default:
                ASSERT(false);
                break;
        }

        if(deltaTime == 0.f)
            break;
        if(phase.td > 0.f)
            continue;

        // phase transition
        kickPlayer.stop();
        Vector2BH<> px = phase.r + phase.x0;
        Vector2BH<> pxv = phase.xv0;
        phase = nextPhase;
        engine->generateNextPendulumPhase(phase, nextPhase);
        phase.x0 = (px - Vector2BH<>(phase.s.translation.x, phase.s.translation.y)) - phase.r;
        phase.xv0 = pxv;
        phase.td -= phase.tu;
        phase.tu = 0.f;
        engine->updatePendulumPhase(phase, nextPhase, true);
        if(phase.kickType != WalkRequest::none)
        {
            kickPlayer.start(*engine->kicks.getKick(phase.kickType), engine->kicks.isKickMirrored(phase.kickType));
        }
    }
}

void WalkingEngine::PendulumPlayer::getPosture(LegPosture& stance, float* leftArmAngle, float* rightArmAngle, Pose2DBH* stepOffset)
{
    const WalkingEngine& p = *engine;
    switch(phase.type)
    {
        case standPhase:
            ASSERT(fabs(phase.xv0.x) < 0.1f && fabs(phase.xv0.y) < 0.1f);
            stance.leftOriginToCom = stance.rightOriginToCom = Vector3BH<>(phase.r.x + phase.x0.x, phase.r.y + phase.x0.y, p.standComPosition.z);
            stance.leftOriginToFoot = Pose3DBH(Vector3BH<>(0.f, p.standComPosition.y, 0.f));
            stance.rightOriginToFoot = Pose3DBH(Vector3BH<>(0.f, -p.standComPosition.y, 0.f));
            // TODO: nextPhase.s?

            if(stepOffset)
                *stepOffset = Pose2DBH();

            break;

        default:
            {
                const float ratio = -phase.tu / (phase.td - phase.tu);
                const float swingMoveFadeIn = ratio < p.walkMovePhase.start ? 0.f : ratio > p.walkMovePhase.start + p.walkMovePhase.duration ? 1.f : blend((ratio - p.walkMovePhase.start) / p.walkMovePhase.duration);
                const float swingMoveFadeOut = 1.f - swingMoveFadeIn;
                const float swingLift = ratio < p.walkLiftPhase.start || ratio > p.walkLiftPhase.start + p.walkLiftPhase.duration ? 0.f : blend((ratio - p.walkLiftPhase.start) / p.walkLiftPhase.duration * 2.f);

                Vector3BH<> comLift;
                if(phase.fromStand || phase.toStand)
                {
                    ASSERT(phase.fromStand != phase.toStand);
                    if(phase.fromStand)
                        comLift = nextPhase.cl * (1.f - blend(1.f + ratio));
                    else
                        comLift = phase.cl * (1.f - blend(ratio));
                }
                else
                    comLift = (ratio <= 0.5f ? phase.cl : nextPhase.cl) * (1.f - blend(ratio * 2.f));

                switch(phase.type)
                {
                    case leftSupportPhase:
                        {
                            /*
                               Vector3BH<> rightStepOffsetRotation(0.f, 0.f, nextPhase.s.rotation * swingMoveFadeIn);
                               Vector3BH<> leftStepOffsetRotation(0.f, swingLift * (phase.rRef.x - phase.rOpt.x) * p.walkSupportRotation, phase.s.rotation * swingMoveFadeOut);
                               rightStepOffsetRotation.z = (rightStepOffsetRotation.z - leftStepOffsetRotation.z) * 0.5f;
                               leftStepOffsetRotation.z = -rightStepOffsetRotation.z;
                               rightStepOffsetRotation += nextPhase.lRotation * swingLift;
                             */
                            stance.leftOriginToCom = Vector3BH<>(phase.r.x + phase.x0.x, phase.r.y + phase.x0.y, p.standComPosition.z) - comLift;
                            const Vector3BH<>& leftOriginToRightOrigin = nextPhase.s.translation;
                            stance.rightOriginToCom = stance.leftOriginToCom - leftOriginToRightOrigin;

                            Vector3BH<> rightStepOffsetRotation = nextPhase.lRotation * swingLift;
                            rightStepOffsetRotation.z += nextPhase.s.rotation * swingMoveFadeIn;
                            const Vector3BH<> leftStepOffsetRotation(0.f, swingLift * (phase.rRef.x - phase.rOpt.x) * p.walkSupportRotation, phase.s.rotation * swingMoveFadeOut);

                            stance.leftOriginToFoot = Pose3DBH(RotationMatrixBH(leftStepOffsetRotation), Vector3BH<>(0.f, p.standComPosition.y, 0.f));
                            stance.rightOriginToFoot = Pose3DBH(RotationMatrixBH(rightStepOffsetRotation), Vector3BH<>(0.f, -p.standComPosition.y, 0.f) + nextPhase.l * swingLift - (nextPhase.s.translation + phase.s.translation) * swingMoveFadeOut);

                            if(leftArmAngle)
                                *leftArmAngle = (nextPhase.s.translation.x * swingMoveFadeIn - phase.s.translation.x * swingMoveFadeOut) / p.speedMax.translation.x * p.walkArmRotationAtFullSpeedX;
                            if(rightArmAngle)
                                *rightArmAngle = (phase.s.translation.x * swingMoveFadeOut - nextPhase.s.translation.x * swingMoveFadeIn) / p.speedMax.translation.x * p.walkArmRotationAtFullSpeedX;
                        }
                        break;

                    case rightSupportPhase:
                        {
                            /*
                               Vector3BH<> leftStepOffsetRotation(0.f, 0.f, nextPhase.s.rotation * swingMoveFadeIn);
                               Vector3BH<> rightStepOffsetRotation(0.f, swingLift * (phase.rRef.x - phase.rOpt.x) * p.walkSupportRotation, phase.s.rotation * swingMoveFadeOut);
                               rightStepOffsetRotation.z = (rightStepOffsetRotation.z - leftStepOffsetRotation.z) * 0.5f;
                               leftStepOffsetRotation.z = -rightStepOffsetRotation.z;
                               leftStepOffsetRotation += nextPhase.lRotation * swingLift;
                             */

                            stance.rightOriginToCom = Vector3BH<>(phase.r.x + phase.x0.x, phase.r.y + phase.x0.y, p.standComPosition.z) - comLift;
                            const Vector3BH<>& rightOriginToLeftOrigin = nextPhase.s.translation;
                            stance.leftOriginToCom = stance.rightOriginToCom - rightOriginToLeftOrigin;

                            Vector3BH<> leftStepOffsetRotation = nextPhase.lRotation * swingLift;
                            leftStepOffsetRotation.z += nextPhase.s.rotation * swingMoveFadeIn;
                            const Vector3BH<> rightStepOffsetRotation(0.f, swingLift * (phase.rRef.x - phase.rOpt.x) * p.walkSupportRotation, phase.s.rotation * swingMoveFadeOut);

                            stance.leftOriginToFoot = Pose3DBH(RotationMatrixBH(leftStepOffsetRotation), Vector3BH<>(0.f, p.standComPosition.y, 0.f) + nextPhase.l * swingLift - (nextPhase.s.translation + phase.s.translation) * swingMoveFadeOut);
                            stance.rightOriginToFoot = Pose3DBH(RotationMatrixBH(rightStepOffsetRotation), Vector3BH<>(0.f, -p.standComPosition.y, 0.f)) ;

                            if(rightArmAngle)
                                *rightArmAngle = (nextPhase.s.translation.x * swingMoveFadeIn - phase.s.translation.x * swingMoveFadeOut) / p.speedMax.translation.x * p.walkArmRotationAtFullSpeedX;
                            if(leftArmAngle)
                                *leftArmAngle = (phase.s.translation.x * swingMoveFadeOut - nextPhase.s.translation.x * swingMoveFadeIn) / p.speedMax.translation.x * p.walkArmRotationAtFullSpeedX;
                        }
                        break;

                    default:
                        ASSERT(false);
                        break;
                }

                if(stepOffset)
                {
                    stepOffset->translation.x = nextPhase.s.translation.x * swingMoveFadeIn - phase.s.translation.x * swingMoveFadeOut;
                    stepOffset->translation.y = nextPhase.s.translation.y * swingMoveFadeIn - phase.s.translation.y * swingMoveFadeOut;
                    stepOffset->rotation = nextPhase.s.rotation * swingMoveFadeIn - phase.s.rotation * swingMoveFadeOut;
                }

                if(kickPlayer.isActive())
                    kickPlayer.applyFoot(stance.leftOriginToFoot, stance.rightOriginToFoot);
            }
    }
}

void WalkingEngine::PendulumPlayer::getPosture(Posture& stance)
{
    const WalkingEngine& p = *engine;

    // head
    stance.headJointAngles[0] = engine->theHeadJointRequestBH.pan == JointDataBH::off ? 0 : engine->theHeadJointRequestBH.pan;
    stance.headJointAngles[1] = engine->theHeadJointRequestBH.tilt == JointDataBH::off ? 0 : engine->theHeadJointRequestBH.tilt;

    // legs
    float leftArmAngle = 0.f, rightArmAngle = 0.f;
    getPosture((LegPosture&)stance, &leftArmAngle, &rightArmAngle, 0);

    // arms
    float halfArmRotation = p.walkArmRotationAtFullSpeedX * 0.5f;

    // northern bites don't use BH's ArmMotionEngine
    // if(engine->theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::left].move)
    // { // ARME arm movement
    // 	for(int i = 0; i < 4; ++i)
    // 		stance.leftArmJointAngles[i] = engine->theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::left].angles[i];
    // }
    // else
    // { // normal WalkingEngine arm movement
    stance.leftArmJointAngles[0] = -pi_2 + p.standArmJointAngles.y + leftArmAngle;
    stance.leftArmJointAngles[1] = p.standArmJointAngles.x;
    stance.leftArmJointAngles[2] = -pi_2;
    stance.leftArmJointAngles[3] = -p.standArmJointAngles.y - leftArmAngle - halfArmRotation;
    // }

    // northern bites don't use BH's ArmMotionEngine
    // if(engine->theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::right].move)
    // { // ARME arm movement
    // 	for(int i = 0; i < 4; ++i)
    // 	    stance.rightArmJointAngles[i] = engine->theArmMotionEngineOutputBH.arms[ArmMotionRequestBH::right].angles[i];
    // }
    // else
    // { // normal WalkingEngine arm movement
    stance.rightArmJointAngles[0] = -pi_2 + p.standArmJointAngles.y + rightArmAngle;
    stance.rightArmJointAngles[1] = p.standArmJointAngles.x;
    stance.rightArmJointAngles[2] = -pi_2;
    stance.rightArmJointAngles[3] = -p.standArmJointAngles.y - rightArmAngle - halfArmRotation;
    // }

    // kick mutations
    if(kickPlayer.isActive())
        kickPlayer.applyHeadAndArms(stance.headJointAngles, stance.leftArmJointAngles, stance.rightArmJointAngles);
}

void WalkingEngine::updateHandSpeeds()
{
    const Vector3BH<>& leftHandPos3D = theRobotModelBH.limbs[MassCalibrationBH::foreArmLeft].translation;
    Vector2BH<> leftHandPos(leftHandPos3D.x, leftHandPos3D.y);
    Vector2BH<> leftHandSpeedVec = (odometryOffset + Pose2DBH(leftHandPos) - Pose2DBH(lastLeftHandPos)).translation / theFrameInfoBH.cycleTime;
    leftHandSpeed = leftHandSpeedVec.abs();
    lastLeftHandPos = leftHandPos;

    const Vector3BH<>& rightHandPos3D = theRobotModelBH.limbs[MassCalibrationBH::foreArmRight].translation;
    Vector2BH<> rightHandPos(rightHandPos3D.x, rightHandPos3D.y);
    Vector2BH<> rightHandSpeedVec = (odometryOffset + Pose2DBH(rightHandPos) - Pose2DBH(lastRightHandPos)).translation / theFrameInfoBH.cycleTime;
    rightHandSpeed = rightHandSpeedVec.abs();
    lastRightHandPos = rightHandPos;
}

float WalkingEngine::asinh(float xf)
{
    const double x = xf; // yes, we need double here
#ifdef _MSC_VER
    const double y = log(x + sqrt(x * x + 1.));
#else
    const double y = ::asinh(x);
#endif
    return float(y);
}

float WalkingEngine::atanh(float xf)
{
    const double x = xf; // yes, we need double here
    const double almostInf = 1000000000000000.; // avoid returning inf or -inf
    if(x >= 1.)
        return float(almostInf);
    if(x <= -1.)
        return float(-almostInf);
#ifdef _MSC_VER
    const double y = 0.5f * log((1. + x) / (1. - x));
#else
    const double y = ::atanh(x);
#endif
    return float(y);
}

float WalkingEngine::blend(float r)
{
    return 0.5f - cos(r * pi) * 0.5f;
}

// equation (4.60):
// rx + x0x * cosh(k * td) + xv0x * sinh(k * td) / k = nsx + nrx                         + nxv0x * sinh(nk * ntu) / nk
// ry + x0y * cosh(k * td) + xv0y * sinh(k * td) / k = nsy + nry + nx0y * cosh(nk * ntu)

// equation (4.61):
// x0x * k * sinh(k * td) + xv0x * cosh(k * td) =                              nxv0x * cosh(nk * ntu)
// x0y * k * sinh(k * td) + xv0y * cosh(k * td) = nx0y * nk * sinh(nk * ntu)

void WalkingEngine::repairPendulumParametersY(PendulumPhase& phase, const PendulumPhase& nextPhase) const
{ // this function ensures that phase.r, phase.x0 and phase.xv0 can be used to solve equation (4.60) and (4.61)

    float sign = phase.type == leftSupportPhase ? 1.f : -1.f;
    float x = phase.x0.y;
    float xv = phase.xv0.y;
    const float k = phase.k.y;
    float r = phase.r.y;
    float px = r + x;

    // check #1: ensure pendulum pivot point is atleast 1mm away from pendulum deflection
    if(x * -sign < 1.f)
    {
        x = 1.f * -sign;
        r = px - x;
    }
    ASSERT(x * -sign >= 1.f);

    // check #2: ensure there is a value for t that solves:
    // x * k * sinh(k * t) + xv * cosh(k * t) = 0

    // x * k * sinh(k * t) + xv * cosh(k * t) = 0
    // x * k * sinh(k * t) / cosh(k * t) + xv  = 0
    // x * k * tanh(k * t) + xv  = 0
    // x * k * tanh(k * t) = -xv
    // tanh(k * t) = -xv / (x * k)
    // t = atanh(-xv / (x * k)) / k

    const float tanhKTl1 = -xv / (x * k);
    float x0 = 0.f;
    float t0;
    if(tanhKTl1 > -1.f && tanhKTl1 < 1.f)
    {
        const float kT = atanh(tanhKTl1);
        t0 = -kT / k;
        x0 = x * cosh(kT) + xv / k * sinh(kT);
    }
    if(x0 * -sign < 1.f)
    { // ensure pendulum pivot point is atleast 1mm away from pendulum peak
        x0 = 1.f * -sign;

        // r + x0 * cosh(k * t0) = px
        // x0 * k * sinh(k * t0) = pxv

        float kT = asinh(xv / (x0 * k));
        t0 = kT / k;
        r = px - x0 * cosh(kT);
        x = px - r;
    }
    ASSERT(x0 * -sign >= 1.f);
    ASSERT(fabs(r + x0 * cosh(k * t0) - px) < 0.1f);
    ASSERT(fabs(x0 * k * sinh(k * t0) - xv) < 0.1f);

    //check #4:
    if((r + x0 - (nextPhase.r.y + nextPhase.x0.y)) * -sign > nextPhase.s.translation.y * -sign - 1.f)
    {
        //r + x0  = nextPhase.s.translation.y + 1.f * sign + (nextPhase.r.y + nextPhase.x0.y);
        const float px0 = nextPhase.s.translation.y + 1.f * sign + (nextPhase.r.y + nextPhase.x0.y);

        // r + x0 = px0
        // r + x0 * cosh(k * t) = px
        // x0 * k * sinh(k * t) = pxv

        // We can't solve this system of equations for r and x0 since px might be equal px0.
        // So, we will keep r and change x and xv:

        x0 = px0 - r;
        if(x0 * -sign < 1.f)
        {
            x0 = 1.f * -sign;
            r = px0 - x0;
        }

        // r + x0 * cosh(k * t0) = px
        // x0 * k * sinh(k * t0) = pxv

        px = r + x0 * cosh(k * t0);
        xv = x0 * k * sinh(k * t0);
        x = px - r;
    }
    else
    {
        ASSERT(fabs(r + x - px) < 0.1f);
    }

    ASSERT(x * -sign >= 1.f);
    ASSERT(x0 * -sign >= 1.f);
    ASSERT(fabs(r + x0 * cosh(k * t0) - px) < 0.1f);
    ASSERT(fabs(x0 * k * sinh(k * t0) - xv) < 0.1f);
    ASSERT((r + x0 - (nextPhase.r.y + nextPhase.x0.y)) * -sign < nextPhase.s.translation.y * -sign);

    phase.r.y = r;
    phase.x0.y = x;
    phase.xv0.y = xv;
}

void WalkingEngine::updatePendulumParametersY(PendulumPhase& phase, PendulumPhase& nextPhase) const
{
    if(phase.toStand)
    {
        ASSERT(nextPhase.x0.y == -nextPhase.r.y);
        ASSERT(nextPhase.xv0.y == 0.f);

        // find x, r and xv? in:
        // r + x = px
        // r + x * cosh(k * td) + xv * sinh(k * td) / k = npx
        // x * k * sinh(k * td) + xv * cosh(k * td) = npxv

        struct Data : public FunctionMinimizer
        {
            float px;
            float npx;
            float npxv;
            float k;
            float coshKTd;
            float sinhKTd;

            virtual float func(float r) const
            {
                const float x = px - r;
                const float xv = (npxv - x * k * sinhKTd) / coshKTd;
                return fabs(r + x * coshKTd + xv * sinhKTd / k - npx);
            }
        } d;

        d.px = phase.r.y + phase.x0.y;
        d.npx = nextPhase.r.y + nextPhase.x0.y;
        d.npxv = nextPhase.xv0.y;
        d.k = phase.k.y;
        d.coshKTd = cosh(d.k * phase.td);
        d.sinhKTd = sinh(d.k * phase.td);

        bool clipped;
        const float r = d.minimize(-1000000.f, 1000000.f, phase.r.y, 0.1f, 0.05f, clipped, "updatePendulumParametersYtoStand");
        float x = d.px - r;
        float xv = (d.npxv - x * d.k * d.sinhKTd) / d.coshKTd;
        if(clipped)
        {
            // find x and xv in:
            // r + x * cosh(k * td) + xv * sinh(k * td) / k = npx
            // x * k * sinh(k * td) + xv * cosh(k * td) = npxv

            // x * cosh(k * td) + xv * sinh(k * td) / k = npx - r
            // x * k * sinh(k * td) + xv * cosh(k * td) = npxv

            // x * cosh(k * td) + xv * sinh(k * td) / k = npx - r
            // xv * cosh(k * td) = npxv - x * k * sinh(k * td)

            // x * cosh(k * td) * k + xv * sinh(k * td) = (npx - r) * k
            // xv = npxv / cosh(k * td) - x * k * sinh(k * td) / cosh(k * td)

            // x * cosh(k * td) * k + (npxv / cosh(k * td) - x * k * sinh(k * td) / cosh(k * td)) * sinh(k * td) = (npx - r) * k

            // x * cosh(k * td) * k + npxv / cosh(k * td) * sinh(k * td) - x * k * sinh^2(k * td) / cosh(k * td) = (npx - r) * k

            // x * cosh(k * td) * k - x * k * sinh^2(k * td) / cosh(k * td) = (npx - r) * k - npxv / cosh(k * td) * sinh(k * td)

            // x * k * (cosh(k * td) - sinh^2(k * td) / cosh(k * td)) = (npx - r) * k - npxv / cosh(k * td) * sinh(k * td)

            x = ((d.npx - r) * d.k - d.npxv / d.coshKTd * d.sinhKTd) / (d.k * (d.coshKTd - d.sinhKTd * d.sinhKTd / d.coshKTd));
            xv = (d.npxv - x * d.k * d.sinhKTd) / d.coshKTd;
        }

        phase.r.y = r;
        phase.x0.y = x;
        phase.xv0.y = xv;
        return;
    }

    // step #1: find t in:
    // x * k * sinh(k * t0) + xv * cosh(k * t0) = 0

    // x * k * sinh(k * t0) + xv * cosh(k * t0) = 0
    // x * k * sinh(k * t0) / cosh(k * t0) + xv  = 0
    // x * k * t0anh(k * t0) + xv  = 0
    // x * k * t0anh(k * t0) = -xv
    // tanh(k * t0) = -xv / (x * k)
    // t0 = atanh(-xv / (x * k)) / k

    // find td, ntu in
    // r + x0 * cosh(k * td) + xv0 * sinh(k * td) / k = ns + nr + nx0 * cosh(nk * ntu)
    // x0 * k * sinh(k * td) + xv0 * cosh(k * td) = nx0 * nk * sinh(nk * ntu)
    // (td > t0)

    const float x = phase.x0.y;
    const float xv = phase.xv0.y;
    const float k = phase.k.y;
    const float tanhKTl1 = -xv / (x * k);
    ASSERT(x != 0.f && tanhKTl1 > -1.f && tanhKTl1 < 1.f);
    const float kT = atanh(tanhKTl1);
    const float t0 = kT / k;
    const float x0 = x * cosh(kT) + xv / k * sinh(kT);
    ASSERT(fabs(x0) > 0.5f);

    struct Data : public FunctionMinimizer
    {
        float x0;
        float xv0;
        float nx0;
        float nk;
        float k;
        float r;
        float nr;
        float ns;

        virtual float func(float td) const
        {
            const float kTd = k * td;
            const float sinhKTd = sinh(kTd);
            const float coshKTd = cosh(kTd);
            const float nkNtu = asinh((x0 * k * sinhKTd + xv0 * coshKTd) / (nx0 * nk));
            return fabs(r + x0 * coshKTd + xv0 * sinhKTd / k - (ns + nr + nx0 * cosh(nkNtu)));
        }
    } d;

    d.x0 = phase.x0.y;
    d.xv0 = phase.xv0.y;
    d.nx0 = nextPhase.x0.y;
    d.nk = nextPhase.k.y;
    d.k = phase.k.y;
    d.r = phase.r.y;
    d.nr = nextPhase.r.y;
    d.ns = nextPhase.s.translation.y;

    const float tdStart = t0 + asinh(d.nx0 * d.nk * sinh(d.nk * nextPhase.tu) / (x0 * d.k)) / d.k;

    bool clipped;
    const float td = d.minimize(t0, 1000000.f, max(t0 + 0.01f, tdStart), 0.001f, 0.05f, clipped, "updatePendulumParametersY");
    const float xvdy = d.x0 * d.k * sinh(d.k * td) + d. xv0 * cosh(d.k * td);
    const float ntu = asinh(xvdy / (d.nx0 * d.nk)) / d.nk;
    ASSERT(ntu < 0.f);
    ASSERT(!clipped || td > 0.8f);
    if(clipped)
    {
        ASSERT(fabs((d.r + phase.x0.y * cosh(d.k * td) + phase.xv0.y * sinh(d.k * td) / d.k) - (d.ns + d.nr + d.nx0 * cosh(d.nk * ntu))) < 1.f);
        ASSERT(fabs(d.x0 * d.k * sinh(d.k * td) + d. xv0 * cosh(d.k * td) - (d.nx0 * d.nk * sinh(d.nk * ntu))) < 0.1f);
        /*
        // the computation gets imprecise with large values for td.
        // so, we shift nextPhase.r.y slightly to please some assertions.
        const float kTd = d.k * td;
        const float sinhKTd = sinh(kTd);
        const float coshKTd = cosh(kTd);
        nextPhase.r.y = d.r + d.x0 * coshKTd + d.xv0 * sinhKTd / d.k - d.ns - d.nx0 * cosh(d.nk * ntu);
         */
    }

    phase.td = td;
    nextPhase.tu = ntu;
}

void WalkingEngine::updatePendulumParametersX(PendulumPhase& phase, PendulumPhase& nextPhase, bool init) const
{
    if(phase.toStand)
    {
        ASSERT(nextPhase.x0.x == 0);
        ASSERT(nextPhase.xv0.x == 0);
        ASSERT(nextPhase.r.x == walkRef.x);

        // find x, r and xv? in:
        // r + x = px
        // r + x * cosh(k * td) + xv * sinh(k * td) / k = npx
        // x * k * sinh(k * td) + xv * cosh(k * td) = npxv

        struct Data : public FunctionMinimizer
        {
            float px;
            float npx;
            float npxv;
            float k;
            float coshKTd;
            float sinhKTd;

            virtual float func(float r) const
            {
                const float x = px - r;
                const float xv = (npxv - x * k * sinhKTd) / coshKTd;
                return fabs(r + x * coshKTd + xv * sinhKTd / k - npx);
            }
        } d;

        d.px = phase.r.x + phase.x0.x;
        d.npx = nextPhase.r.x + nextPhase.x0.x;
        d.npxv = nextPhase.xv0.x;
        d.k = phase.k.x;
        d.coshKTd = cosh(d.k * phase.td);
        d.sinhKTd = sinh(d.k * phase.td);

        bool clipped;
        const float r = d.minimize(-1000000.f, 1000000.f, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersXtoStand");
        float x = d.px - r;
        float xv = (d.npxv - x * d.k * d.sinhKTd) / d.coshKTd;
        if(clipped)
        {
            x = ((d.npx - r) * d.k - d.npxv / d.coshKTd * d.sinhKTd) / (d.k * (d.coshKTd - d.sinhKTd * d.sinhKTd / d.coshKTd));
            xv = (d.npxv - x * d.k * d.sinhKTd) / d.coshKTd;
        }

        phase.r.x = r;
        phase.x0.x = x;
        phase.xv0.x = xv;
        return;
    }

    if(nextPhase.toStand)
    {
        //cout << "Next Phase TO STAND" << endl;
        // find x, r, nx, nr in:
        // r + x = px
        // r + x * cosh(k * td) + xv * sinh(k * td) / k = ns + nr + nx * cosh(nk * ntu)
        // x * k * sinh(k * td) + xv * cosh(k * td) = nx * nk * sinh(nk * ntu)
        // rn + nx = npx

        struct Data : public FunctionMinimizer
        {
            float px;
            float xv;
            float ns;
            float k;
            float coshKTd;
            float sinhKTd;
            float nk;
            float npx;
            float coshNkNtu;
            float sinhNkNtu;

            virtual float func(float r) const
            {
                const float x = px - r;
                const float nx = (x * k * sinhKTd + xv * coshKTd) / (nk * sinhNkNtu);
                const float nr = npx - nx;
                return fabs(r + x * coshKTd + xv * sinhKTd / k - (ns + nr + nx * coshNkNtu));
            }
        } d;

        d.px = phase.r.x + phase.x0.x;
        d.xv = phase.xv0.x;
        d.ns = nextPhase.s.translation.x;
        d.k = phase.k.x;
        d.coshKTd = cosh(d.k * phase.td);
        d.sinhKTd = sinh(d.k * phase.td);
        d.nk = nextPhase.k.x;
        d.npx = nextPhase.r.x + nextPhase.x0.x;
        d.coshNkNtu = cosh(d.nk * nextPhase.tu);
        d.sinhNkNtu = sinh(d.nk * nextPhase.tu);

        bool clipped;
        const float r = d.minimize(-1000000.f, 1000000.f, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersXnextToStand");
        const float x = d.px - r;
        const float nx = (x * d.k * d.sinhKTd + d.xv * d.coshKTd) / (d.nk * d.sinhNkNtu);
        const float nr = d.npx - nx;
        // if(clipped) // do nothing... !

        phase.r.x = r;
        phase.x0.x = x;
        nextPhase.r.x = nr;
        nextPhase.x0.x = nx;
        nextPhase.s.translation.x = d.ns;
        return;
    }

    ASSERT(!phase.toStand && !nextPhase.toStand);

    bool clipped;
    float r, x;
    float sinhKTd = sinh(phase.k.x * phase.td);
    float coshKTd = cosh(phase.k.x * phase.td);

    //if(init)
    {
        // find x, r in:
        // r + x = px
        // x * k * sinh(k * td) + xv * cosh(k * td) = nxvu

        struct Data : public FunctionMinimizer
        {
            float px;
            float xv;
            float sinhKTd;
            float coshKTd;
            float k;
            float nxvu;

            virtual float func(float r) const
            {
                const float x = px - r;
                return fabs(x * k * sinhKTd + xv * coshKTd - nxvu);
            }
        } d;

        d.px = phase.r.x + phase.x0.x;
        d.xv = phase.xv0.x;
        d.sinhKTd = sinhKTd;
        d.coshKTd = coshKTd;
        d.k = phase.k.x;
        ASSERT(nextPhase.x0.x == 0.f);
        d.nxvu = nextPhase.xv0.x * cosh(nextPhase.k.x * nextPhase.tu);

        RangeBH<> rLimit(phase.rOpt.x + (init ? walkRefXPlanningLimit.min : walkRefXLimit.min), phase.rOpt.x + (init ? walkRefXPlanningLimit.max : walkRefXLimit.max));
        r = d.minimize(rLimit.min, rLimit.max, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersXinit");
        x = d.px - r;
    }
    /*
       else
       {
    // equation (4.108)
    // find x, r, nxv in:
    // r + x = px
    // r + x * cosh(k * td) + xv * sinh(k * td) / k = ns + nr + nxv * sinh(nk * ntu) / nk
    // x * k * sinh(k * td) + xv * cosh(k * td) = nxv * cosh(nk * ntu)

    struct Data : public FunctionMinimizer
    {
    float px;
    float xv;
    float ns;
    float nr;
    float k;
    float nk;
    float sinhKTd;
    float coshKTd;
    float coshNkNtu;
    float sinhNkNtu_nk;

    virtual float func(float r) const
    {
    const float x = px - r;
    const float nxv = (x * k * sinhKTd + xv * coshKTd) / coshNkNtu;
    return fabs(r + x * coshKTd + xv * sinhKTd / k - (ns + nr + nxv * sinhNkNtu_nk));
    }
    } d;

    d.px = phase.r.x + phase.x0.x;
    d.xv = phase.xv0.x;
    d.ns = nextPhase.s.translation.x;
    d.nr = nextPhase.r.x;
    d.k = phase.k.x;
    d.nk = nextPhase.k.x;
    d.sinhKTd = sinhKTd;
    d.coshKTd = coshKTd;
    d.coshNkNtu = cosh(d.nk * nextPhase.tu);
    d.sinhNkNtu_nk = sinh(d.nk * nextPhase.tu) / d.nk;

    RangeBH<> rLimit(phase.rOpt.x + (init ? walkRefXPlanningLimit.min : walkRefXLimit.min), phase.rOpt.x + (init ? walkRefXPlanningLimit.max : walkRefXLimit.max));
    r = d.minimize(rLimit.min, rLimit.max, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersX");
    x = d.px - r;
    nextPhase.xv0.x = (x * d.k * d.sinhKTd + d.xv * d.coshKTd) / d.coshNkNtu;
    }
     */

    if(clipped)
    {
        float xvd = x * phase.k.x * sinhKTd + phase.xv0.x * coshKTd;

        // limit step size
        const RangeBH<>& xvdxLimit = init ? walkXvdXPlanningLimit : walkXvdXLimit;
        if(!xvdxLimit.isInside(xvd))
        {
            xvd = xvdxLimit.limit(xvd);

            // find x, r in:
            // r + x = px
            // x * k * sinh(k * td) + xv * cosh(k * td) = xvd

            struct Data : public FunctionMinimizer
            {
                float px;
                float xv;
                float xvd;
                float k;
                float sinhKTd;
                float coshKTd;

                virtual float func(float r) const
                {
                    const float x = px - r;
                    return fabs(x * k * sinhKTd + xv * coshKTd - xvd);
                }
            } d2;

            d2.px = phase.r.x + phase.x0.x;;
            d2.xv = phase.xv0.x;
            d2.xvd = xvd;
            d2.k = phase.k.x;
            d2.sinhKTd = sinhKTd;
            d2.coshKTd = coshKTd;

            r = d2.minimize(-1000000.f, 1000000.f, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersXklhglk");
            x = d2.px - r;
            if(clipped)
                xvd = x * d2.k * d2.sinhKTd + d2.xv * d2.coshKTd;

            // TODO: improve this: reduce nsy? reduce td?
        }

        const float nxv0 = xvd / cosh(nextPhase.k.x * nextPhase.tu);

        nextPhase.xv0.x = nxv0;
    }

    const float ns = r + x * coshKTd + phase.xv0.x * sinhKTd / phase.k.x - (nextPhase.r.x + nextPhase.xv0.x * sinh(nextPhase.k.x * nextPhase.tu) / nextPhase.k.x);
    nextPhase.s.translation.x = ns;
    phase.r.x = r;
    phase.x0.x = x;

    if(init)
        phase.rRef = phase.r;


    /*
    // equation (4.108)
    // find x, r, nxv in:
    // r + x = px
    // r + x * cosh(k * td) + xv * sinh(k * td) / k = ns + nr + nxv * sinh(nk * ntu) / nk
    // x * k * sinh(k * td) + xv * cosh(k * td) = nxv * cosh(nk * ntu)

    struct Data : public FunctionMinimizer
    {
    float px;
    float xv;
    float ns;
    float nr;
    float td;
    float k;
    float nk;
    float ntu;

    virtual float func(float r) const
    {
    const float x = px - r;
    const float nxv = (x * k * sinh(k * td) + xv * cosh(k * td)) / cosh(nk * ntu);
    return fabs(r + x * cosh(k * td) + xv * sinh(k * td) / k - (ns + nr + nxv * sinh(nk * ntu) / nk));
    }
    } d;

    d.px = phase.r.x + phase.x0.x;
    d.xv = phase.xv0.x;
    d.ns = nextPhase.sOpt.translation.x;
    d.nr = nextPhase.r.x;
    d.td = phase.td;
    d.k = phase.k.x;
    d.nk = nextPhase.k.x;
    d.ntu = nextPhase.tu;

    bool clipped;
    RangeBH<> rLimit(phase.rOpt.x + (init ? walkRefXSoftLimit.min : walkRefXHardLimit.min), phase.rOpt.x + (init ? walkRefXSoftLimit.max : walkRefXHardLimit.max));
    float r = d.minimize(rLimit.min, rLimit.max, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersX");
    float x = d.px - r;
    float nxv = (x * d.k * sinh(d.k * d.td) + d.xv * cosh(d.k * d.td)) / cosh(d.nk * d.ntu);
    float ns = d.ns;

    if(clipped)
    {
    float xvd = x * d.k * sinh(d.k * d.td) + d.xv * cosh(d.k * d.td);
    nxv = xvd / cosh(d.nk * d.ntu);
    ns =  r + x * cosh(d.k * d.td) + d.xv * sinh(d.k * d.td) / d.k - (d.nr + nxv * sinh(d.nk * d.ntu) / d.nk);

    // limit step size
    const RangeBH<>& xvdxLimit = init ? walkXvdXSoftLimit : walkXvdXHardLimit;
    if(!xvdxLimit.isInside(xvd))
    {
    xvd = xvdxLimit.limit(xvd);

    // find x, r in:
    // r + x = px
    // x * k * sinh(k * td) + xv * cosh(k * td) = xvd

    struct Data : public FunctionMinimizer
    {
    float px;
    float xv;
    float xvd;
    float k;
    float td;

    virtual float func(float r) const
    {
    const float x = px - r;
    return fabs(x * k * sinh(k * td) + xv * cosh(k * td) - xvd);
    }
} d2;

d2.px = phase.r.x + phase.x0.x;;
d2.xv = phase.xv0.x;
d2.xvd = xvd;
d2.k = phase.k.x;
d2.td = phase.td;

r = d2.minimize(-1000000.f, 1000000.f, phase.r.x, 0.1f, 0.05f, clipped, "updatePendulumParametersXklhglk");
x = d2.px - r;
nxv = (x * d2.k * sinh(d2.k * d2.td) + d2.xv * cosh(d2.k * d2.td)) / cosh(d.nk * d.ntu);

ns =  r + x * cosh(d2.k * d2.td) + d2.xv * sinh(d2.k * d2.td) / d2.k - d.nr - nxv * sinh(d.nk * d.ntu) / d.nk;

// TODO: improve this: reduce nsy? reduce td?
// note: since sRef the soft/hard limit idea does not work anymore.
}
}

phase.r.x = r;
phase.x0.x = x;
nextPhase.xv0.x = nxv;
nextPhase.s.translation.x = ns;

*/
}

const Vector3BH<> WalkingEngine::drawFootPoints[] =
{
    Vector3BH<>(0.00897f, 0.03318f, -0.04517f),
    Vector3BH<>(0.0577f, 0.03877f, -0.04526f),
    Vector3BH<>(0.07395f, 0.03729f, -0.04528f),
    Vector3BH<>(0.08765f, 0.03148f, -0.04531f),
    Vector3BH<>(0.09687f, 0.0188f, -0.04532f),
    Vector3BH<>(0.09942f, 0.01015f, -0.04533f),

    Vector3BH<>(0.09899f, -0.00869f, -0.04533f),
    Vector3BH<>(0.094f, -0.02418f, -0.04532f),
    Vector3BH<>(0.08454f, -0.0361f, -0.04531f),
    Vector3BH<>(0.06568f, -0.04615f, -0.04527f),
    Vector3BH<>(0.04991f, -0.04818f, -0.04525f),
    Vector3BH<>(0.00956f, -0.03881f, -0.04518f),

    Vector3BH<>(-0.00842f, -0.03954f, -0.04515f),
    Vector3BH<>(-0.02199f, -0.04758f, -0.04513f),
    Vector3BH<>(-0.03125f, -0.05002f, -0.04511f),
    Vector3BH<>(-0.04905f, -0.0376f, -0.04508f),

    Vector3BH<>(-0.05072f, 0.02138f, -0.04507f),
    Vector3BH<>(-0.04262f, 0.0306f, -0.04509f),
    Vector3BH<>(-0.03297f, 0.03435f, -0.0451f),
    Vector3BH<>(-0.00901f, 0.03272f, -0.04514f),
};
const unsigned int WalkingEngine::drawNumOfFootPoints = sizeof(drawFootPoints) / sizeof(*drawFootPoints);

#define DRAW_FOOT(id, pose, left, color) \
    for(unsigned int i = 0; i < drawNumOfFootPoints; ++i) \
{ \
    Vector3BH<> p1 = drawFootPoints[i] * 1000.f; \
    Vector3BH<> p2 = drawFootPoints[(i + 1) % drawNumOfFootPoints] * 1000.f; \
    if(left) \
    { \
        p1.y = -p1.y; \
        p2.y = -p2.y; \
    } \
    p1 = (pose) * p1; \
    p2 = (pose) * p2; \
    LINE3D(id, p1.x, p1.y, 0.f, p2.x, p2.y, 0.f, 2, color); \
}

void WalkingEngine::drawZmp()
{
    Vector3BH<> leftFoot(0.f, standComPosition.y, 0.f);
    Vector3BH<> rightFoot(0.f, -standComPosition.y, 0.f);

    if(pendulumPlayer.phase.type == leftSupportPhase)
        rightFoot += Vector3BH<>(pendulumPlayer.nextPhase.s.translation.x, pendulumPlayer.nextPhase.s.translation.y, 0.f);
    if(pendulumPlayer.phase.type == rightSupportPhase)
        leftFoot += Vector3BH<>(pendulumPlayer.nextPhase.s.translation.x, pendulumPlayer.nextPhase.s.translation.y, 0.f);

    DRAW_FOOT("module:WalkingEngine:zmp", Pose3DBH(leftFoot), true, ColorRGBA(0, 0, 0));
    DRAW_FOOT("module:WalkingEngine:zmp", Pose3DBH(rightFoot), false, ColorRGBA(0, 0, 0));

    Vector2BH<> r = pendulumPlayer.phase.r;
    Vector2BH<> rOpt = pendulumPlayer.phase.rOpt;
    Vector2BH<> px = r + pendulumPlayer.phase.x0;

    SPHERE3D("module:WalkingEngine:zmp", px.x, px.y, walkHeight.y, 6, ColorRGBA(0, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", measuredPx.x, measuredPx.y, walkHeight.y, 6, ColorRGBA(0, 255, 0));

    LINE3D("module:WalkingEngine:zmp", r.x, r.y, 0.f, px.x, px.y, walkHeight.y, 2, ColorRGBA(0, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", r.x, r.y, 0.f, 1, ColorRGBA(0, 0, 0));

    SPHERE3D("module:WalkingEngine:zmp", pendulumPlayer.phase.rOpt.x, pendulumPlayer.phase.rOpt.y, 0.f, 1, ColorRGBA(0, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", pendulumPlayer.phase.rRef.x, pendulumPlayer.phase.rRef.y, 0.f, 1, ColorRGBA(0, 0, 255));
    SPHERE3D("module:WalkingEngine:zmp", measuredR.x, measuredR.y, 0.f, 1, ColorRGBA(0, 255, 0));

    SPHERE3D("module:WalkingEngine:zmp", rOpt.x + walkRefXLimit.max, rOpt.y, 0.f, 1, ColorRGBA(255, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", rOpt.x + walkRefXLimit.min, rOpt.y, 0.f, 1, ColorRGBA(255, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", rOpt.x, rOpt.y + walkRefYLimit.max, 0.f, 1, ColorRGBA(255, 0, 0));
    SPHERE3D("module:WalkingEngine:zmp", rOpt.x, rOpt.y + walkRefYLimit.min, 0.f, 1, ColorRGBA(255, 0, 0));
}
