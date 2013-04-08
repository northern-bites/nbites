#include "BallTrackModule.h"

#include "NBMath.h"

namespace man
{
namespace balltrack
{

BallTrackModule::BallTrackModule() :
    portals::Module(),
    ballLocationOutput(base())
{
    ballFilter = new BallFilter();

    ballX = 1.f;
    ballY = 2.f;
}

BallTrackModule::~BallTrackModule()
{
}

void BallTrackModule::run_()
{
    // Latch
    visionBallInput.latch();

    // Update the Ball filter
    ballFilter->update(visionBallInput.message());

    // Fill the ballMessage with the filters representation
    portals::Message<messages::FilteredBall> ballMessage(0);

    // Use the Weighted Naive Estimate
    ballMessage.get()->set_distance(ballFilter->getWeightedNaiveEstimate().dist);
    ballMessage.get()->set_bearing(ballFilter->getWeightedNaiveEstimate().bear);
    ballMessage.get()->set_bearing_deg(TO_DEG * ballFilter->getWeightedNaiveEstimate().bear);
    ballMessage.get()->set_rel_x(ballFilter->getCartesianWeightedNaiveEstimate().relX);
    ballMessage.get()->set_rel_y(ballFilter->getCartesianWeightedNaiveEstimate().relY);

    // Use the Exponential Filter Estimate
    // ballMessage.get()->set_distance(ballFilter->getExponentialEstimate().dist);
    // ballMessage.get()->set_bearing(ballFilter->getExponentialEstimate().bear);
    // ballMessage.get()->set_rel_x(ballFilter->getExponentialWeightedNaiveEstimate().relX);
    // ballMessage.get()->set_rel_y(ballFilter->getExponentialWeightedNaiveEstimate().relY);

    ballMessage.get()->mutable_vis()->CopyFrom(visionBallInput.message());

    ballLocationOutput.setMessage(ballMessage);
}

} //namespace balltrack
} //namespace man
