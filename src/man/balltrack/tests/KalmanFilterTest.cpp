#include "gtest/gtest.h"

#include "../KalmanFilter.h"
#include "../MMKalmanFilter.h"
#include "../NaiveBallFilter.h"

using namespace man::balltrack;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (KalmanFilterTest, FilterCanUpdate) {
    KalmanFilter *kf = new KalmanFilter(true);
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
    KalmanFilter *kf = new KalmanFilter(false);
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

TEST (KalmanFilterTest, KalmanFilterCanInitialize) {
    KalmanFilter *kf = new KalmanFilter();
    kf->initialize();
    ASSERT_EQ(kf->getRelXPosEst(), 0.f);
    ASSERT_EQ(kf->getRelYPosEst(), 0.f);
    ASSERT_EQ(kf->getRelXVelEst(), 1.f);
    ASSERT_EQ(kf->getRelYVelEst(), 0.5f);

    ufvector4 initX = boost::numeric::ublas::zero_vector<float> (4);
    initX(0) = 10.f;
    initX(1) = 11.f;
    initX(2) = 12.f;
    initX(3) = 13.f;
    ufmatrix4 initCov = boost::numeric::ublas::identity_matrix<float> (4);
    initCov(0,0) = 1.f;
    initCov(1,1) = 2.f;
    initCov(2,2) = 3.f;
    initCov(3,3) = 4.f;

    kf->initialize(initX, initCov);

    ASSERT_EQ(kf->getRelXPosEst(), 10.f);
    ASSERT_EQ(kf->getRelYPosEst(), 11.f);
    ASSERT_EQ(kf->getRelXVelEst(), 12.f);
    ASSERT_EQ(kf->getRelYVelEst(), 13.f);

    ASSERT_EQ(kf->getCovXPosEst(), 1.f);
    ASSERT_EQ(kf->getCovYPosEst(), 2.f);
    ASSERT_EQ(kf->getCovXVelEst(), 3.f);
    ASSERT_EQ(kf->getCovYVelEst(), 4.f);
}

TEST (KalmanFilterTest, Works) {
    KalmanFilter *kf = new KalmanFilter(true);

    ufvector4 initX = boost::numeric::ublas::zero_vector<float> (4);
    initX(0) = 50.f;
    initX(1) = 0.f;
    initX(2) = 0.f;
    initX(3) = 0.f;
    ufmatrix4 initCov = boost::numeric::ublas::identity_matrix<float> (4);
    initCov(0,0) = 5.f;
    initCov(1,1) = 5.f;
    initCov(2,2) = 0.f;
    initCov(3,3) = 0.f;

    kf->initialize(initX, initCov);

    kf->predict(kf->genOdometry(-5.f, 0.f, 0.f),10.f);
    kf->updateWithObservation(kf->genVisBall(58.f, 0.f));

    kf->predict(kf->genOdometry(0.f, 0.f, 0.f),.1f);
    kf->updateWithObservation(kf->genVisBall(55.f, .05f));
    kf->predict(kf->genOdometry(0.f, 0.f, 0.f),.1f);
    kf->updateWithObservation(kf->genVisBall(55.f, .05f));
    kf->predict(kf->genOdometry(0.f, 0.f, 0.f),.1f);
    kf->updateWithObservation(kf->genVisBall(55.f, .05f));
    kf->predict(kf->genOdometry(0.f, 0.f, 0.f),.1f);
    kf->updateWithObservation(kf->genVisBall(83.f, .05f));
}

TEST (KalmanFilterTest, MMWorks) {
    MMKalmanFilter *kf = new MMKalmanFilter();
    KalmanFilter *poor = new KalmanFilter();

    kf->initialize(50.f, 0.f, 5.f, 5.f);

    messages::Motion inpMotion;
    inpMotion.mutable_odometry()->CopyFrom(poor->genOdometry(0.f,0.f,0.f));

    kf->update(poor->genVisBall(55.5f, 0.f),
               inpMotion);

    kf->update(poor->genVisBall(62.f, 0.f),
               inpMotion);

    kf->update(poor->genVisBall(64.f, 0.f),
               inpMotion);

    kf->update(poor->genVisBall(71.f, 0.f),
               inpMotion);
    kf->printEst();
/*
    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);


    kf->update(poor->genVisBall(74.f, 0.f),
               inpMotion);

    kf->update(poor->genVisBall(76.f, 0.f),
               inpMotion);

    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);
    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);
    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);
    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);
    kf->update(poor->genVisBall(75.f, 0.f),
               inpMotion);

 */




}
