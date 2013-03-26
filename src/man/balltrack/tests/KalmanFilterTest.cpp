#include "gtest/gtest.h"

#include "../KalmanFilter.h"
#include "../NaiveBallFilter.h"

using namespace man::balltrack;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (KalmanFilterTest, FilterCanUpdate) {
    KalmanFilter *kf = new KalmanFilter();
    kf->isUpdated();
    bool yes = true;
    bool no = false;
    ASSERT_EQ(no, kf->isUpdated());
    kf->setUpdate(true);
    ASSERT_EQ(yes, kf->isUpdated());

    int a =1;
    int b =1;
    ASSERT_EQ(a,b);
}

TEST (KalmanFilterTest, FilterCanPredict) {
    KalmanFilter *kf = new KalmanFilter();
    kf->initialize();
    float initX = 0.f;
    float initY = 0.f;
    float initXVel = 1.f;
    float initYVel = 0.5f;
    float finalX = 4.f;
    float finalY = 2.f;
    ASSERT_EQ(initX, kf->getRelXPosEst());
    ASSERT_EQ(initY, kf->getRelYPosEst());

    messages::RobotLocation odometry;
    odometry.set_x(0.f);
    odometry.set_y(0.f);
    odometry.set_h(0.f);
    kf->predict(odometry, 4.f);

    ASSERT_EQ(finalX, kf->getRelXPosEst());
    ASSERT_EQ(finalY, kf->getRelYPosEst());
}
