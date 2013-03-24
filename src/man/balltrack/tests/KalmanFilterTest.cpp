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
